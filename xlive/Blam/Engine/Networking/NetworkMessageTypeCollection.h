#pragma once

#include "Blam\Engine\Networking\Session\NetworkSession.h"

#define player_identifier_size_bits (sizeof(XUID) * CHAR_BIT)

enum e_network_message_type_collection : unsigned int
{
	_ping,
	_pong,
	_broadcast_search,
	_broadcast_reply,
	_connect_request,
	_connect_refuse,
	_connect_establish,
	_connect_closed,
	_join_request,
	_join_abort,
	_join_refuse,
	_leave_session,
	_leave_acknowledge,
	_session_disband,
	_session_boot,
	_host_handoff,
	_peer_handoff,
	_host_transition,
	_host_reestablish,
	_host_decline,
	_peer_reestablish,
	_peer_establish,
	_election,
	_election_refuse,
	_time_synchronize,
	_membership_update,
	_peer_properties,
	_delegate_leadership,
	_boot_machine,
	_player_add,
	_player_refuse,
	_player_remove,
	_player_properties,
	_parameters_update,
	_parameters_request,
	_countdown_timer,
	_mode_acknowledge,
	_virtual_couch_update,
	_virtual_couch_request,
	_vote_update,
	_view_establishment,
	_player_acknowledge,
	_synchronous_update,
	_synchronous_actions,
	_synchronous_join,
	_synchronous_gamestate,
	_game_results,
	_text_chat,
	_test,

	// custom network meesages bellow
	_request_map_filename,
	_custom_map_filename,
	_team_change,
	_rank_change,
	_anti_cheat,
	_custom_variant_settings,

	_network_message_type_collection_end
};

static const char* network_message_type_collection_name[] = {
	"ping",
	"pong",
	"broadcast_search",
	"broadcast_reply",
	"connect_request",
	"connect_refuse",
	"connect_establish",
	"connect_closed",
	"join_request",
	"join_abort",
	"join_refuse",
	"leave_session",
	"leave_acknowledge",
	"session_disband",
	"session_boot",
	"host_handoff",
	"peer_handoff",
	"host_transition",
	"host_reestablish",
	"host_decline",
	"peer_reestablish",
	"peer_establish",
	"election",
	"election_refuse",
	"time_synchronize",
	"membership_update",
	"peer_properties",
	"delegate_leadership",
	"boot_machine",
	"player_add",
	"player_refuse",
	"player_remove",
	"player_properties",
	"parameters_update",
	"parameters_request",
	"countdown_timer",
	"mode_acknowledge",
	"virtual_couch_update",
	"virtual_couch_request",
	"vote_update",
	"view_establishment",
	"player_acknowledge",
	"synchronous_update",
	"synchronous_actions",
	"synchronous_join",
	"synchronous_gamestate",
	"game_results",
	"text_chat",
	"test",

	//custom packets bellow
	"request_map_filename",
	"map_file_name",
	"team_change",
	"rank_change",
	"anti_cheat",
	"custom_variant_settings",

	"end"
};

struct s_custom_map_filename
{
	wchar_t file_name[32];
	int map_download_id;
};

struct s_request_map_filename
{
	XUID user_identifier;
	int map_download_id;
};

struct s_team_change
{
	DWORD team_index;
};

struct s_rank_change
{
	BYTE rank;
};

struct s_anti_cheat
{
	bool enabled;
};

#pragma pack(push, 1)
struct s_text_chat
{
	XNKID session_id;
	unsigned int routed_players_mask;
	unsigned int metadata;
	bool source_is_server;
	XUID source_player_id;
	XUID destination_players_ids[16];
	BYTE gap_99[3];
	int destination_player_count;
	wchar_t text[122];
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_text_chat, 404);

const char* GetNetworkMessageName(int enumVal);
void register_network_message(void *, int, char*, int, int, int, void*, void*, void*);

namespace NetworkMessage
{
	void ApplyGamePatches();
	void SendRequestMapFilename(int mapDownloadId);
	void SendTeamChange(int peerIdx, int teamIdx);
	void SendRankChange(int peerIdx, BYTE rank);
	void SendAntiCheat(int peerIdx);
}