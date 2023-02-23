#include "stdafx.h"

#include "Blam\Common\Common.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "imgui.h"
#include "imgui_handler.h"
#include "Util\Hooks\Hook.h"

namespace imgui_handler
{
	namespace iMessageBox
	{
		namespace
		{
			std::string message;
		}
		void Render(bool* p_open)
		{
			ImGuiIO& io = ImGui::GetIO();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoResize;
			//window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x * 0.5f, viewport->WorkSize.y * 0.5f), ImGuiCond_::ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
			//ImGui::PushFont(font2);
			ImGui::SetNextWindowSize(ImVec2(650, 250), ImGuiCond_Appearing);
			ImGui::SetNextWindowSizeConstraints(ImVec2(610, 250), ImVec2(1920, 1080));
			if (h2mod->GetEngineType() == _main_menu)
				ImGui::SetNextWindowBgAlpha(1);
			if (ImGui::Begin("Message", p_open, window_flags))
			{
				ImGui::TextWrapped(message.c_str());
				ImGui::SetCursorPosY(190);
				if (ImGui::Button("Ok", ImVec2(610, 50)))
				{
					*p_open = false;
					Close();
				}
			}
			// Pop style var
			ImGui::PopStyleVar();
			ImGui::End();
			
		}
		void SetMessage(std::string Message)
		{
			message = Message;
		}

		void Open()
		{
			WriteValue<byte>(Memory::GetAddress(0x9712cC), 1);
			ImGuiToggleInput(true);
			PlayerControl::DisableLocalCamera(true);
		}
		void Close()
		{
			WriteValue<byte>(Memory::GetAddress(0x9712cC), 0);
			ImGuiToggleInput(false);
			PlayerControl::DisableLocalCamera(false);
		}
	}
}