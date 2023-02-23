#include "stdafx.h"

#include "ServerList.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\Accounts\Accounts.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "..\xnet\IpManagement\XnIp.h"

#include "H2MOD\Utils\Utils.h"

using namespace rapidjson;

std::mutex CServerList::addServerMutex;
std::mutex CServerList::removeServerMutex;
std::mutex CServerList::getServerCountsMutex;

std::mutex serverListRequestMutex;
std::unordered_map<HANDLE, CServerList*> serverListRequests;

bool CServerList::CountResultsUpdated = false;

// Title specific XLocator service properties
_HALO2VISTA_TITLE_SERVICE_PROPERTIES h2v_service_properties;

HANDLE g_hXLocatorHandle = INVALID_HANDLE_VALUE;

CServerList* GetServerListQueryByHandle(HANDLE hHandle, bool lock)
{
	std::lock_guard lg(serverListRequestMutex);

	for (auto& request : serverListRequests)
	{
		if (request.first == hHandle)
		{
			// ugly af but it'll do for now
			// don't forget to UNLOCK after when you finish working with this
			if (lock)
				request.second->m_itemQueryMutex.lock();

			return request.second;
		}
	}

	return nullptr;
}

void RemoveServerListQueryByPtr(CServerList* serverListQuery)
{
	// remove from the list, to note memory doesn't get released
	// because thread might still be in progress
	std::lock_guard lg(serverListRequestMutex);

	for (auto it = serverListRequests.begin(); it != serverListRequests.end(); it++)
	{
		if (it->second == serverListQuery)
		{
			XCloseHandle(it->first);
			serverListRequests.erase(it);
			break;
		}
	}
}

void ServerListQueryCancelAll()
{
	std::lock_guard lg(serverListRequestMutex);

	for (auto& request : serverListRequests)
	{
		XCloseHandle(request.first);
		request.second->CancelOperation();
	}

	serverListRequests.clear();
}

static size_t BasicStrDownloadCb(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t ServerlistDownloadWriteCb(void* contents, size_t size, size_t nmemb, void* userp)
{
	auto* buffer = reinterpret_cast<std::shared_ptr<std::string>*>(userp);
	(*buffer)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void BadServer(ULONGLONG xuid, const char* log_catch)
{
	LOG_ERROR_XLIVE("{} - XUID: {} - Log Catch: {}", __FUNCTION__, xuid, log_catch);
}

DWORD ComputeXLocatorServerEnumeratorBufferSize(DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequredPropertiesIDs, DWORD* outStringBufferSize)
{
	if (cItems > 0)
	{
		unsigned int stringProperties = 0;

		DWORD result = (DWORD)((sizeof(XLOCATOR_SEARCHRESULT) * cItems) + (cItems * sizeof(XUSER_PROPERTY) * cRequiredPropertyIDs));

		if (pRequredPropertiesIDs)
		{
			for (int i = 0; i < cRequiredPropertyIDs; i++)
			{
				if ((pRequredPropertiesIDs[i] & 0xF0000000) == 0x40000000) // check if we have string properties
					++stringProperties;
			}
		}

		// string buffers are max 64 wide strings characters (128 bytes) and the null character, so in total 65 characters (130 bytes)
		result += (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties);

		if (outStringBufferSize)
		{
			*outStringBufferSize = (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties);
			LOG_INFO_XLIVE("{} : stringBufferSize: {}, stringBufferSize2: {}, cItems: {}, stringCount: {}", __FUNCTION__, *outStringBufferSize, (X_PROPERTY_UNICODE_BUFFER_SIZE * cItems * stringProperties), cItems, stringProperties);
		}

		return result;
	}
	else
	{
		if (outStringBufferSize)
			*outStringBufferSize = 0;

		return 0;
	}
}

int CServerList::GetItemLeftCount()
{
	return m_itemsLeftInDoc;
}

int CServerList::GetValidItemsFoundCount()
{
	return m_pageItemsFoundCount;
}

bool CServerList::SearchResultParseAndWrite(std::shared_ptr<std::string> serverResultData, XUID xuid, XLOCATOR_SEARCHRESULT* pOutSearchResult, XUSER_PROPERTY** propertiesBuffer, WCHAR** stringBuffer)
{
	XLOCATOR_SEARCHRESULT searchResult;
	ZeroMemory(&searchResult, sizeof(XLOCATOR_SEARCHRESULT));

	rapidjson::Document doc;
	doc.Parse(serverResultData->c_str());

	// operation successful or not
	bool result = false;

	if (!doc.HasMember("dwMaxPublicSlots")) {
		BadServer(xuid, "Missing Member: dwMaxPublicSlots");
		return result;
	}
	searchResult.dwMaxPublicSlots = doc["dwMaxPublicSlots"].GetUint();

	if (!doc.HasMember("dwFilledPublicSlots"))
	{
		BadServer(xuid, "Missing Member: dwFilledPublicSlots");
		return result;
	}
	searchResult.dwFilledPublicSlots = doc["dwFilledPublicSlots"].GetUint();

	if (!doc.HasMember("dwMaxPrivateSlots"))
	{
		BadServer(xuid, "Missing Member: dwFilledPublicSlots");
		return result;
	}
	searchResult.dwMaxPrivateSlots = doc["dwMaxPrivateSlots"].GetUint();

	if (!doc.HasMember("dwMaxFilledPrivateSlots"))
	{
		BadServer(xuid, "Missing Member: dwMaxFilledPrivateSlots");
		return result;
	}
	searchResult.dwFilledPrivateSlots = doc["dwMaxFilledPrivateSlots"].GetUint();

	if (!doc.HasMember("dwServerType"))
	{
		BadServer(xuid, "Missing Member: dwServerType");
		return result;
	}
	searchResult.dwServerType = doc["dwServerType"].GetUint();

#pragma region Xbox Network Address Reading
	if (!doc.HasMember("lanaddr") || !doc["lanaddr"].IsUint())
	{
		BadServer(xuid, "Missing Member: lanaddr");
		return result;
	}
	searchResult.serverAddress.ina.s_addr = doc["lanaddr"].GetUint();

	if (!doc.HasMember("xnaddr") || !doc["xnaddr"].IsUint())
	{
		BadServer(xuid, "Missing Member: xnaddr");
		return result;
	}
	searchResult.serverAddress.inaOnline.s_addr = htonl(doc["xnaddr"].GetUint());

	if (!doc.HasMember("dwPort"))
	{
		BadServer(xuid, "Missing Member: dwPort");
		return result;
	}
	searchResult.serverAddress.wPortOnline = htons(doc["dwPort"].GetUint());

	if (!doc.HasMember("abenet"))
	{
		BadServer(xuid, "Missing Member: abEnet");
		return result;
	}
	const char* abEnet_str = doc["abenet"].GetString();
	if (abEnet_str == NULL)
	{
		BadServer(xuid, "abEnet == NULL");
		return result;
	}
	HexStrToBytes(abEnet_str, searchResult.serverAddress.abEnet, sizeof(XNADDR::abEnet));

	if (!doc.HasMember("abonline"))
	{
		BadServer(xuid, "Missing Member: abOnline");
		return result;
	}
	const char* abOnline_str = doc["abonline"].GetString();
	if (abOnline_str == NULL)
	{
		BadServer(xuid, "abOnline == NULL");
		return result;
	}
	HexStrToBytes(abOnline_str, searchResult.serverAddress.abOnline, sizeof(XNADDR::abOnline));
#pragma endregion

#pragma region Xbox Transport Security Keys Reading
	if (!doc.HasMember("xnkid"))
	{
		BadServer(xuid, "Missing Member: xnkid");
		return result;
	}
	const char* xnkid_str = doc["xnkid"].GetString();
	if (xnkid_str == NULL)
	{
		BadServer(xuid, "xnkid == NULL");
		return result;
	}
	HexStrToBytes(xnkid_str, searchResult.xnkid.ab, sizeof(XNKID));

	if (!doc.HasMember("xnkey"))
	{
		BadServer(xuid, "Missing Member: xnkey");
		return result;
	}
	const char* xnkey_str = doc["xnkey"].GetString();
	if (xnkey_str == NULL)
	{
		BadServer(xuid, "xnkey == NULL");
		return result;
	}
	HexStrToBytes(xnkey_str, searchResult.xnkey.ab, sizeof(XNKEY));
#pragma endregion

	if (!doc.HasMember("xuid"))
	{
		BadServer(xuid, "Missing Member: xuid");
		return result;
	}
	searchResult.serverID = doc["xuid"].GetUint64();

	if (!doc.HasMember("pProperties") || !doc.HasMember("cProperties"))
	{
		BadServer(xuid, "Missing Member: cProperties or pProperties");
		return result;
	}

	searchResult.cProperties = 0;
	searchResult.pProperties = *propertiesBuffer;

	std::vector<DWORD> propertiesWritten;

	for (auto& property : doc["pProperties"].GetArray())
	{
		bool propertyNeeded = false;
		DWORD propertyId = property["dwPropertyId"].GetInt();

		for (int i = 0; i < m_searchPropertiesIdCount; i++)
		{
			if (propertyId == m_pSearchPropertyIds[i])
			{
				propertyNeeded = true;
				break;
			}
		}

		if (!propertyNeeded)
		{
			//LOG_INFO_XLIVE("{} - unrequested property ID: 0x{:X}, skipping", __FUNCTION__, propertyId);
			continue;
		}

		// temporary memory
		XUSER_PROPERTY tProperty;
		ZeroMemory(&tProperty, sizeof(XUSER_PROPERTY));

		tProperty.dwPropertyId = propertyId;
		tProperty.value.type = property["type"].GetInt();

		propertiesWritten.push_back(propertyId);

		const char* data = 0;
		int str_len = 0;

		std::wstring str;
		GenericStringBuffer<UTF16<> > buffer;
		Writer<GenericStringBuffer<UTF16<> >, UTF8<>, UTF16<> > writer(buffer);

		switch (property["type"].GetInt())
		{
		case XUSER_DATA_TYPE_INT32:
			tProperty.value.nData = property["value"].GetInt();
			break;

		case XUSER_DATA_TYPE_INT64:
			tProperty.value.i64Data = property["value"].GetInt64();
			break;

		case XUSER_DATA_TYPE_UNICODE:
			writer.String(property["value"].GetString());

			str.append(buffer.GetString());
			str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
			str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());

			if (!m_cancelOperation)
			{
				SecureZeroMemory(*stringBuffer, X_PROPERTY_UNICODE_BUFFER_SIZE);

				wcscpy(*stringBuffer, str.c_str());

				tProperty.value.string.cbData = wcsnlen(*stringBuffer, 64) * sizeof(WCHAR) + 2;
				tProperty.value.string.pwszData = *stringBuffer;

				*stringBuffer = (WCHAR*)((BYTE*)(*stringBuffer) + X_PROPERTY_UNICODE_BUFFER_SIZE);
			}

			break;

		case XUSER_DATA_TYPE_BINARY:
		case XUSER_DATA_TYPE_FLOAT:
		case XUSER_DATA_TYPE_DOUBLE:
		case XUSER_DATA_TYPE_DATETIME:
			//LOG_TRACE_XLIVE("{} - unimplemented data type");
			break;
		}

		if (!m_cancelOperation)
		{
			searchResult.pProperties[searchResult.cProperties] = tProperty;
			searchResult.cProperties++;
			(*propertiesBuffer)++;
		}

		for (auto property : propertiesWritten)
		{
			int i = 0;
			bool foundMatch = false;
			for (; i < m_searchPropertiesIdCount; i++)
			{
				if (m_pSearchPropertyIds[i] == property)
				{
					foundMatch = true;
					break;
				}
			}

			if (!foundMatch)
			{
				LOG_ERROR_XLIVE("{} - couldn't find property: 0x{:X}", __FUNCTION__, m_pSearchPropertyIds[i]);
			}
		}
	}

	if (!m_cancelOperation)
	{
		memcpy(pOutSearchResult, &searchResult, sizeof(XLOCATOR_SEARCHRESULT));
		result = true;
	}

	//LOG_ERROR_XLIVE("{} : string count: {}, properties: {}", __FUNCTION__, strings, nResult->cProperties);
	return result;
}

void CServerList::EnumerateFromHttp()
{
	CURL* curl;
	CURLM* curl_mhandle;
	CURLcode res;

	DWORD outStringBufferSize = 0;
	int itemsLeftToDownload, validItemsFound = 0;
	
	std::string serverlist_url(cartographerURL + "/live/server_list.php");

	auto cleanup = [this]()
	{
		RemoveServerListQueryByPtr(this);
		m_itemQueryMutex.lock();
		delete this;

		return;
	};

	m_pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	m_pOverlapped->InternalHigh = 0;
	m_pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	addDebugText("Requesting server list");
	curl = curl_interface_init_no_verify();
	if (!curl)
	{
		LOG_ERROR_XLIVE("{} - curl failed to initialize", __FUNCTION__);

		m_pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		m_pOverlapped->InternalHigh = 0;
		m_pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);

		cleanup();

		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL, serverlist_url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BasicStrDownloadCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_serverListToDownload);
	res = curl_easy_perform(curl);
	// clear curl resource after
	curl_easy_cleanup(curl);

	// parse json to document
	rapidjson::Document document;
	document.Parse(m_serverListToDownload.c_str());

	itemsLeftToDownload = document["servers"].Size();
	// in case we have just 1 serverlist 'page' to download
	int itemListMaxQueryCount = (std::min)(itemsLeftToDownload, XLOCATOR_SERVER_PAGE_REPORT_ITEM_COUNT_MIN);

	if (ComputeXLocatorServerEnumeratorBufferSize(itemListMaxQueryCount, m_searchPropertiesIdCount, m_pSearchPropertyIds, &outStringBufferSize) > m_resultBufferSize)
	{
		m_pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
		m_pOverlapped->InternalHigh = 0;
		m_pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

		cleanup();

		LOG_ERROR_XLIVE("{} - insufficient buffer for I/O operation!", __FUNCTION__);
		return;
	}

	m_itemsLeftInDoc = itemsLeftToDownload;

	curl_mhandle = curl_multi_init();
	curl_multi_setopt(curl_mhandle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

	std::vector<std::pair<CURL*, std::shared_ptr<std::string>>> itemsToDownloadQuery(itemListMaxQueryCount);

	bool itemQueryError = false;

	// build the list to download
	auto& serverXuidArray = document["servers"].GetArray();
	auto xuidStrItr = serverXuidArray.Begin();
	auto xuidStrWriteItemItr = serverXuidArray.Begin();

	while (!itemQueryError
		&& itemsLeftToDownload > 0
		&& !m_cancelOperation)
	{
		// wait for the game to signal again, for the next thread

		CHRONO_DEFINE_TIME_AND_CLOCK();

		m_pageItemsFoundCount = 0;
		_clock::time_point beforePauseTp = _clock::now();

		while (m_operationOnPause)
		{
			Sleep(100);

			if (_clock::now() - beforePauseTp > 5s)
			{
				m_cancelOperation = true;
				break;
			}
		}

		if (m_cancelOperation)
			break;

		int serverQueryIdx = 0;

		// this starts from where it left off
		for (; xuidStrItr != serverXuidArray.End(); ++xuidStrItr)
		{
			// we reached max ITEM download size, break out and download the servers
			if (serverQueryIdx < itemListMaxQueryCount)
			{
				// simply reuse the curl handle/allocated std::string buffer if already present
				if (itemsToDownloadQuery[serverQueryIdx].first == nullptr)
				{
					itemsToDownloadQuery[serverQueryIdx] = std::make_pair(curl_interface_init_no_verify(), std::make_shared<std::string>());
				}
				else
				{
					// remove the handle first, easy handle still valid, to update the easyopts
					curl_multi_remove_handle(curl_mhandle, itemsToDownloadQuery[serverQueryIdx].first);
					itemsToDownloadQuery[serverQueryIdx].second->clear();
				}

				auto& itemQuery = itemsToDownloadQuery[serverQueryIdx++];

				std::string server_url = std::string(cartographerURL + "/live/servers/" + xuidStrItr->GetString());

				// server_url is copied to another buffer when setting CURLOPT_URL
				curl_easy_setopt(itemQuery.first, CURLOPT_URL, server_url.c_str());
				curl_easy_setopt(itemQuery.first, CURLOPT_WRITEFUNCTION, ServerlistDownloadWriteCb);
				curl_easy_setopt(itemQuery.first, CURLOPT_WRITEDATA, &itemQuery.second);
				curl_easy_setopt(itemQuery.first, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
				curl_easy_setopt(itemQuery.first, CURLOPT_PIPEWAIT, 1L);

				// (re-)add the handle to the list
				curl_multi_add_handle(curl_mhandle, itemQuery.first);
			}
			else
			{
				break;
			}
		}

		// if we are at the last serverlist page to download,
		// close and remove the unused connections
		if (itemsLeftToDownload < itemListMaxQueryCount)
		{
			// after this we shouldn't have any other server to download
			for (auto it = itemsToDownloadQuery.begin() + itemsLeftToDownload; it != itemsToDownloadQuery.end(); )
			{
				curl_multi_remove_handle(curl_mhandle, it->first);
				curl_easy_cleanup(it->first);
				it = itemsToDownloadQuery.erase(it);
			}
		}

		int still_running = 0;
		do {
			CURLMcode mc = curl_multi_perform(curl_mhandle, &still_running);

			if (mc == CURLM_OK && still_running)
			{
				/* wait for activity, timeout or "nothing" */
				mc = curl_multi_poll(curl_mhandle, NULL, 0, 1000, NULL);
			}
			else if (mc) {
				LOG_ERROR_XLIVE("{} - curl_multi_poll() failed, code {}", __FUNCTION__, (int)mc);
				itemQueryError = true;
				break;
			}

		} while (still_running);

		if (!itemQueryError)
		{
			XLOCATOR_SEARCHRESULT* searchResults = reinterpret_cast<XLOCATOR_SEARCHRESULT*>(m_resultBuffer);
			XUSER_PROPERTY* propertiesBuffer = (XUSER_PROPERTY*)((BYTE*)searchResults + (sizeof(XLOCATOR_SEARCHRESULT) * itemListMaxQueryCount));
			WCHAR* stringBuffer = (WCHAR*)((BYTE*)propertiesBuffer + (sizeof(XUSER_PROPERTY) * m_searchPropertiesIdCount * itemListMaxQueryCount));

			int searchResultIdx = 0;
			// vector should be XLOCATOR_SERVER_PAGE_REPORT_ITEM_COUNT_MIN in size
			for (auto& itemQuery : itemsToDownloadQuery)
			{
				ZeroMemory(&searchResults[searchResultIdx], sizeof(XLOCATOR_SEARCHRESULT));
				if (SearchResultParseAndWrite(itemQuery.second, std::stoll(xuidStrWriteItemItr->GetString()), &searchResults[searchResultIdx], &propertiesBuffer, &stringBuffer))
				{
					m_pageItemsFoundCount++;

					// this holds all servers found count
					// not just per page
					validItemsFound++;

					searchResultIdx++;
				}

				// this counter count even bad servers
				itemsLeftToDownload--;

				xuidStrWriteItemItr++;
			}

			m_itemsLeftInDoc = itemsLeftToDownload;

			// if we wrote servers to buffer, report back to the game
			// this will signal the game to call XEnumerate again
			if (this->GetValidItemsFoundCount() > 0)
			{
				m_pOverlapped->InternalLow = ERROR_SUCCESS;
				m_pOverlapped->InternalHigh = this->GetValidItemsFoundCount();
				m_pOverlapped->dwExtendedError = 0;
			}

			m_operationOnPause = true;
		}
	}

	for (auto& itemQuery : itemsToDownloadQuery)
	{
		curl_multi_remove_handle(curl_mhandle, itemQuery.first);
		curl_easy_cleanup(itemQuery.first);
		itemQuery.first = nullptr;
		itemQuery.second.reset();
	}
	curl_multi_cleanup(curl_mhandle);

	LOG_TRACE_XLIVE("{} - found a total of: {} servers", __FUNCTION__, validItemsFound);
	addDebugText(L"Found %d servers", validItemsFound);

	cleanup();
	return;
}

void CServerList::GetServerCounts(PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lg(getServerCountsMutex);

	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_interface_init_no_verify();
	if (curl) {
		std::string url(cartographerURL + "/live/dedicount.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BasicStrDownloadCb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		rapidjson::Document document;
		document.Parse(readBuffer.c_str());

		if (document.HasMember("public_count"))
		{
			h2v_service_properties.total_count = document["total"].GetInt();
			h2v_service_properties.total_peer = document["peer_count"].GetInt();
			h2v_service_properties.total_peer_gold = document["peer_gold"].GetInt();
			h2v_service_properties.total_public = document["public_count"].GetInt();
			h2v_service_properties.total_public_gold = document["public_gold"].GetInt();

			// we updated the results, they can be used just fine
			CountResultsUpdated = true;
		}
	}
}

DWORD CServerList::Enumerate(HANDLE hHandle, DWORD cbBuffer, CHAR* pvBuffer, PXOVERLAPPED pOverlapped)
{
	bool lock_resource = true;

	CServerList* serverListQuery = GetServerListQueryByHandle(hHandle, lock_resource);
	if (serverListQuery == nullptr)
		return ERROR_INVALID_HANDLE;

	switch (serverListQuery->m_operationState)
	{
	case OperationPending:
		serverListQuery->m_operationState = OperationIncomplete;
		serverListQuery->SetNewPageBuffer(cbBuffer, pvBuffer);
		serverListQuery->m_pOverlapped = pOverlapped;
		serverListQuery->m_searchThread = std::thread(&CServerList::EnumerateFromHttp, serverListQuery);
		serverListQuery->m_searchThread.detach();
		break;

	case OperationIncomplete:
		// update:
		// this notifies the serverlist thread to download the next serverlist `page`

		if (serverListQuery->GetItemLeftCount() > 0)
		{
			serverListQuery->m_pOverlapped = pOverlapped;
			serverListQuery->SetNewPageBuffer(cbBuffer, pvBuffer);
			pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
			pOverlapped->InternalHigh = 0;
			pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);
			
			// continue writing servers
			serverListQuery->m_operationOnPause = false;
			
			// break if we still have servers to write
			break;
		}
		else
		{
			// if not, set the operation state to finished
			// and go to OperationFinished handler
			serverListQuery->m_operationState = OperationFinished;
		}

	case OperationFailed:
	case OperationFinished:
		// this query has finished, release resources and report back to the game
		// TODO maybe add support to release the resources in the thread after a timeout
		// in case the game doesn't check for the state anymore
		pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
		pOverlapped->InternalHigh = 0;
		pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
		serverListQuery->CancelOperation();

		RemoveServerListQueryByPtr(serverListQuery);
		break;

	default:
		assert(0);
	}

	if (lock_resource)
		serverListQuery->m_itemQueryMutex.unlock();

	return ERROR_IO_PENDING;
}

void CServerList::RemoveServer(PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lk(removeServerMutex);

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_interface_init_no_verify();
	if (curl)
	{
		rapidjson::Document document;
		document.SetObject();
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[0].xuid), document.GetAllocator());

		Value token(kStringType);
		token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());
		document.AddMember("token", token, document.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);

		std::string url(cartographerURL + "/live/del_server.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BasicStrDownloadCb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = 0;
}

void CServerList::AddServer(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	std::lock_guard<std::mutex> lk(addServerMutex);

	CURL *curl;
	CURLcode res;
	std::string readBuffer;

	pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;
	pOverlapped->InternalHigh = 0; // this shouldn't even be checked by game's code, but for some reason it gets in Halo 2, InternalHIgh is used for enumerating data, where it holds how many elemets were retreived
	pOverlapped->dwExtendedError = HRESULT_FROM_WIN32(ERROR_IO_INCOMPLETE);

	curl = curl_interface_init_no_verify();
	if (curl) {
		rapidjson::Document document;
		document.SetObject();

		Value token(kStringType);
		if (H2CurrentAccountLoginToken)
			token.SetString(H2CurrentAccountLoginToken, document.GetAllocator());

		Value xnkid_val(kStringType);
		xnkid_val.SetString(ByteToHexStr(xnkid.ab, sizeof(xnkid.ab)).c_str(), document.GetAllocator());

		Value xnkey_val(kStringType);
		xnkey_val.SetString(ByteToHexStr(xnkey.ab, sizeof(xnkey.ab)).c_str(), document.GetAllocator());

		XnIp* localUser = gXnIp.GetLocalUserXn();

		document.AddMember("token", token, document.GetAllocator());
		document.AddMember("xuid", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document.AddMember("dwServerType", Value().SetInt(dwServerType), document.GetAllocator());
		document.AddMember("dwMaxPublicSlots", Value().SetInt(dwMaxPublicSlots), document.GetAllocator());
		document.AddMember("dwFilledPublicSlots", Value().SetInt(dwFilledPublicSlots), document.GetAllocator());
		document.AddMember("dwMaxPrivateSlots", Value().SetInt(dwMaxPrivateSlots), document.GetAllocator());
		document.AddMember("dwMaxFilledPrivateSlots", Value().SetInt(dwFilledPrivateSlots), document.GetAllocator());
		document.AddMember("dwPort", Value().SetInt(H2Config_base_port), document.GetAllocator());
		if (localUser->bValid)
		{
			document.AddMember("lanaddr", Value().SetUint(localUser->xnaddr.ina.s_addr), document.GetAllocator());
		}
		document.AddMember("xnkid", xnkid_val, document.GetAllocator());
		document.AddMember("xnkey", xnkey_val, document.GetAllocator());
		document.AddMember("cProperties", Value().SetInt(cProperties + 3), document.GetAllocator());
		document.AddMember("pProperties", Value().SetArray(), document.GetAllocator());

		for (DWORD i = 0; i < cProperties; i++)
		{
			if (pProperties[i].dwPropertyId == XUSER_PROPERTY_USERNAME_2) // this data isn't set right by the game
				continue;

			Value property(kObjectType);
			property.AddMember("dwPropertyId", Value().SetInt(pProperties[i].dwPropertyId), document.GetAllocator());
			property.AddMember("type", Value().SetInt(pProperties[i].value.type), document.GetAllocator());

			switch (pProperties[i].value.type)
			{
			case XUSER_DATA_TYPE_INT32:
				property.AddMember("value", Value().SetInt(pProperties[i].value.nData), document.GetAllocator());
				break;

			case XUSER_DATA_TYPE_INT64:
				property.AddMember("value", Value().SetInt64(pProperties[i].value.i64Data), document.GetAllocator());
				break;


			case XUSER_DATA_TYPE_UNICODE:
				StringBuffer uni_input;
				Writer< StringBuffer, UTF16<> > writerUTF16(uni_input); // UTF-16 input
				writerUTF16.String(pProperties[i].value.string.pwszData);

				property.AddMember("value", Value().SetString(uni_input.GetString(), document.GetAllocator()), document.GetAllocator());
				break;

			}
			document["pProperties"].PushBack(property, document.GetAllocator());
		}

		char* name = strnlen_s(H2Config_dedi_server_name, XUSER_MAX_NAME_LENGTH) > 0 ? H2Config_dedi_server_name : usersSignInInfo[dwUserIndex].szUserName;

		/* For whatever reason the game is currently refusing to send the servername or player profile name so we're going to send it ourselves.*/
		Value serv_name_property(kObjectType);
		serv_name_property.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_SERVER_NAME), document.GetAllocator());
		serv_name_property.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_UNICODE), document.GetAllocator());
		serv_name_property.AddMember("value", Value().SetString(name, strnlen_s(name, XUSER_MAX_NAME_LENGTH), document.GetAllocator()), document.GetAllocator());

		document["pProperties"].PushBack(serv_name_property, document.GetAllocator());

		Value user_xuid(kObjectType);
		user_xuid.AddMember("dwPropertyId", Value().SetInt(XUSER_PROPERTY_XUID), document.GetAllocator());
		user_xuid.AddMember("type", Value().SetInt(XUSER_DATA_TYPE_INT64), document.GetAllocator());
		user_xuid.AddMember("value", Value().SetUint64(usersSignInInfo[dwUserIndex].xuid), document.GetAllocator());
		document["pProperties"].PushBack(user_xuid, document.GetAllocator());

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);

		std::string url(cartographerURL + "/live/add_server.php");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BasicStrDownloadCb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer.GetString());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = 0;
}

DWORD WINAPI XLocatorServerAdvertise(DWORD dwUserIndex, DWORD dwServerType, XNKID xnkid, XNKEY xnkey, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, DWORD dwFilledPublicSlots, DWORD dwFilledPrivateSlots, DWORD cProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	if (UserSignedOnline(dwUserIndex))
	{
		std::thread(&CServerList::AddServer, dwUserIndex, dwServerType, xnkid, xnkey, dwMaxPublicSlots, dwMaxPrivateSlots, dwFilledPublicSlots, dwFilledPrivateSlots, cProperties, pProperties, pOverlapped).detach();
		return HRESULT_FROM_WIN32(ERROR_IO_PENDING);
	}
	else
		return -1;
}

DWORD WINAPI XLocatorServerUnAdvertise(DWORD dwUserIndex, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XLocatorServerUnAdvertise()");
	if (UserSignedOnline(dwUserIndex))
	{
		std::thread(&CServerList::RemoveServer, pOverlapped).detach();
		return HRESULT_FROM_WIN32(ERROR_IO_PENDING);
	}
	else
		return -1;
}

// 5233: ??
DWORD WINAPI XLocatorGetServiceProperty(DWORD dwUserIndex, DWORD cNumProperties, PXUSER_PROPERTY pProperties, PXOVERLAPPED pOverlapped)
{
	// LOG_TRACE_XLIVE("XLocatorGetServiceProperty  (*** checkme ***) (dwUserIndex = {0:x}, cNumProperties = {1:x}, pProperties = {2:x}, pOverlapped = {3:x})",
	//		dwUserIndex, cNumProperties, pProperties, pOverlapped);

	// hacky but does the job, because the memory we are writing to is not global (at least in H2v)
	// some dumbass engineer at hired gun decided to keep the server counts in temporary heap memory
	// and if the async i/o operation is done and the menu is closed, it'll write in free'd memory most likely
	// so instead to write the data asynchronously, we store the properties in XLIVE memory, then pass it to the game when it needs it
	// and get the properties asynchronously

	if (UserSignedOnline(dwUserIndex))
		std::thread(&CServerList::GetServerCounts, pOverlapped).detach();

	// we simply just give the game the results synchronously, if we have any
	// it'll query the data each 5 seconds
	if (CServerList::CountResultsUpdated)
	{
		pProperties[0].value.nData = h2v_service_properties.total_count;
		pProperties[1].value.nData = h2v_service_properties.total_public;
		pProperties[2].value.nData = h2v_service_properties.total_peer_gold;
		pProperties[3].value.nData = h2v_service_properties.total_peer;
	}

	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->InternalHigh = 0;
	pOverlapped->dwExtendedError = 0;

	return S_OK;
}

// TODO: implement filters
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, DWORD cItems, DWORD cRequiredPropertyIDs, DWORD* pRequiredPropertyIDs, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("{}()", __FUNCTION__);

	std::lock_guard lg(serverListRequestMutex);

	if (!phEnum)
		return ERROR_INVALID_PARAMETER;

	if (cItems < XLOCATOR_SERVER_PAGE_MIN_ITEMS)
		cItems = XLOCATOR_SERVER_PAGE_MIN_ITEMS;
	if (cItems > XLOCATOR_SERVER_PAGE_MAX_ITEMS)
		cItems = XLOCATOR_SERVER_PAGE_MAX_ITEMS;

	CServerList* serverListRequest = new CServerList(cItems, cRequiredPropertyIDs, pRequiredPropertyIDs);
	*phEnum = serverListRequest->Handle = CreateMutex(NULL, NULL, NULL);
	*pcbBuffer = ComputeXLocatorServerEnumeratorBufferSize(cItems, cRequiredPropertyIDs, pRequiredPropertyIDs, nullptr);
	serverListRequests.insert(std::make_pair(serverListRequest->Handle, serverListRequest));

	LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);

	return ERROR_SUCCESS;
}


// 5238: ??
DWORD WINAPI XLocatorCreateKey(XNKID* pxnkid, XNKEY* pxnkey)
{
	LOG_TRACE_XLIVE("XLocatorCreateKey()");
	if (pxnkid && pxnkey) {
		XNetCreateKey(pxnkid, pxnkey);

		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_ONLINE_PEER;
	}

	return S_OK;
}


// 5235: ??
DWORD WINAPI XLocatorCreateServerEnumeratorByIDs(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8)
{
	LOG_TRACE_XLIVE("XLocatorCreateServerEnumeratorByIDs()");
	// not done - error now
	return ERROR_INVALID_PARAMETER;
}

// 5236: ??
DWORD WINAPI XLocatorServiceInitialize(DWORD a1, PHANDLE phLocatorService)
{
	LOG_TRACE_XLIVE("XLocatorServiceInitialize  (a1 = {:x}, phLocatorService = {:p})",
		a1, (void*)phLocatorService);

	if (phLocatorService) *phLocatorService = g_hXLocatorHandle = CreateMutex(NULL, NULL, NULL);

	/*
	Lost Planet, Gears of War
	- LocatorV1.434307DE.RTP.
	*/

	return ERROR_SUCCESS;
}

// 5237: ??
DWORD WINAPI XLocatorServiceUnInitialize(HANDLE xlocatorhandle)
{
	LOG_TRACE_XLIVE("XLocatorServiceUnInitialize(a1 = {})", xlocatorhandle);

	XCloseHandle(xlocatorhandle);
	ServerListQueryCancelAll();

	g_hXLocatorHandle = INVALID_HANDLE_VALUE;

	return ERROR_SUCCESS;
}
