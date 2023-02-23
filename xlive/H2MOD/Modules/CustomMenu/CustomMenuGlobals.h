#pragma once

#include "c_list_widget.h"
#include "c_screen_with_menu.h"

struct s_new_ui_screen_parameters;

typedef void(__thiscall* button_handler_cb_t)(void* context, int a1, int* a2);
typedef void* (__cdecl* proc_ui_screen_load_cb_t)(s_new_ui_screen_parameters*);

enum e_ui_channel
{
	_ui_channel_hardware_error = 0,
	_ui_channel_game_error = 1,
	_ui_channel_virtual_keyboard = 2,
	_ui_channel_gameshell_dialog = 3,
	_ui_channel_gameshell_screen = 5,
	_ui_channel_gameshell_background = 6,
};

struct __declspec(align(4)) s_new_ui_screen_parameters
{
	void data_new(__int16 _flags1, __int16 _flags2, int ui_channel, int a4, proc_ui_screen_load_cb_t ui_screen_load_cb)
	{
		this->flags = ((DWORD)_flags1 | (DWORD)(_flags2 << 16));
		this->ui_channel = ui_channel;
		this->field_8 = a4;
		this->field_10 = -1;
		this->field_14 = -1;
		this->field_18 = -1;
		this->proc_ui_screen_load_cb = ui_screen_load_cb;
	}

	void* ui_screen_load_proc_exec()
	{
		return proc_ui_screen_load_cb(this);
	}

	DWORD flags;
	DWORD ui_channel;
	int field_8;
	DWORD context; // this might be wrong, but it looks like it holds screen context data
	DWORD field_10;
	DWORD field_14;
	DWORD field_18;
	proc_ui_screen_load_cb_t proc_ui_screen_load_cb;
};
static_assert(sizeof(s_new_ui_screen_parameters) == 0x20);

template<typename T>
struct c_slot2
{
	void* c_slot_vtbl;
	int field_8[3]; // c_slot data?
	T* ui_screen_edit_list;
	void(T::* button_handler_cb)(int* a2, int* a3);
};

extern DWORD H2BaseAddr;
const int CreditsMenu_ID = 0xFF000006;

BYTE* ui_memory_pool_allocate(int size, int a2);
int __cdecl user_interface_register_screen_to_channel(void* ui_memory, s_new_ui_screen_parameters* parameters);
int __cdecl user_interface_back_out_from_channel(int ui_channel, int screen_idx);

void __stdcall set_widget_label_from_string_id_reimpl(int thisptr, int label_id, int label_menu_id);

int __stdcall sub_20F790_CM(int thisptr, __int16 selected_button_id);
int __cdecl sub_250E22_CM(int thisptr, int a2, DWORD* menu_vftable_1, DWORD menu_button_handler, int number_of_buttons);
int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description);
void __stdcall sub_2101a4_CMLTD(int thisptr, int label_id, wchar_t* rtn_label, int label_menu_id);
char __stdcall sub_21bb0b_CMLTD(void* thisptr, __int16 a2, int* aa3, int label_menu_id, int label_id_description);
void __cdecl sub_3e3ac_CMLTD(int a1, int label_id, wchar_t* rtn_label, int label_menu_id);

// vkeyboard related
int __stdcall sub_23ae3c_CMLTD(void* thisptr, int label_menu_id, int label_id_title, int label_id_description);
char __stdcall sub_210a44_CMLTD(int thisptr, int a2, int* a3, int label_menu_id, int label_id_title, int label_id_description);

void CallWgit(proc_ui_screen_load_cb_t WgitScreenfunctionPtr);
void CallWgit(proc_ui_screen_load_cb_t WgitScreenfunctionPtr, int open_method);
void CallWgit(proc_ui_screen_load_cb_t WgitScreenfunctionPtr, int open_method, int menu_wgit_type);
int __cdecl CustomMenu_CallHead(s_new_ui_screen_parameters* a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type);