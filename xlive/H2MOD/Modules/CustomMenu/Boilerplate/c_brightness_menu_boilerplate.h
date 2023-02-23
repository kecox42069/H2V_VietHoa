#pragma once

#include "CustomMenuGlobals.h"
#include "c_brightness_menu.h"
#include "c_list_widget.h"

// ** rename the class when creating new ui screens
class c_brightness_level_edit_list : public c_list_widget
{
public:
	// 4 items displayed at once in the list
	char item_list[132 * 4];
	int field_2C0;
	c_slot2<c_brightness_level_edit_list> slot_2_unk;
	// button handler callback

	c_brightness_level_edit_list::c_brightness_level_edit_list(int _flags);

	virtual char* get_item_list() override
	{
		// returns pointer to edit list
		return item_list;
	}

	virtual int get_list_item_count() override
	{
		// returns edit list count
		return 4;
	}

	void get_label(int a1, int a2) override
	{
		// here list item update label code
		auto p_sub_211909 = Memory::GetAddress<int(__thiscall*)(int, int, int, int)>(0x211909);
		auto p_sub_21bf85 = Memory::GetAddress<void(__thiscall*)(int, int)>(0x21bf85);

		// a1 = ptr to account_list_items[idx]
		__int16 list_item_index = *(WORD*)(a1 + 112);
		int v3 = p_sub_211909(a1, 6, 0, 0);
		if (v3)
		{
			set_widget_label_from_string_id_reimpl(v3, list_item_index + 1, CMLabelMenuId_AccountList);
		}
	}

	// button handler
	void button_handler(int* a2, int* a3);

private:
};

static_assert(offsetof(c_brightness_level_edit_list, edit_list) == 176);

class c_brightness_menu : protected c_screen_with_menu
{
public:
	static void* __cdecl open(s_new_ui_screen_parameters* a1);

	c_brightness_menu::c_brightness_menu(int a3, int a4, int a5, bool account_removal_mode);

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		// here add code that should execute when menu closes
		return c_screen_with_menu::custom_deconstructor(flags);
	};

	// c_screen_with_menu specific interface
	virtual int IUnkFunc23(int a2) override
	{
		// title and description code
		
		// int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
		// return sub_2111ab_CMLTD((int)this, a2, CMLabelMenuId_AccountList, 0xFFFFFFF0, 0xFFFFFFF1);
		// return c_screen_with_menu::IUnkFunc23(a2);
	}

	virtual int IUnkFunc24() override
	{
		// default button selected (if needed)
		account_edit_list.set_selected_list_button_idx(0);
		return c_screen_with_menu::IUnkFunc24();
	}

	virtual void* get_open_menu_cb() override
	{
		// menu open callback
		return c_brightness_menu::open;
	}

	c_brightness_level_edit_list list_widgets;

private:
};

// static_assert(sizeof(c_brightness_menu) == 3388);
static_assert(offsetof(c_brightness_menu, list_widgets) == 2656);
