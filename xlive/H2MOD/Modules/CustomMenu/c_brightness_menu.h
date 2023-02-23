#pragma once

#pragma once

#include "CustomMenuGlobals.h"
#include "c_brightness_menu.h"
#include "c_list_widget.h"

#define BRIGHTNESS_MENU_ID 272

class c_brightness_level_edit_list : public c_list_widget
{
public:
	char edit_list[132 * 4];
	char edit_list_info[28];

	// todo maybe implement the actual constructor

	c_brightness_level_edit_list::c_brightness_level_edit_list(int a2) :
		c_list_widget(a2, false)
	{
		void* old_vtbl = *(void**)this;
		// this will replace the vtable
		auto p_initialize_button_list = Memory::GetAddressRelative<void(__thiscall*)(void*, int)>(0x650E22); // c_brightness_level_edit_list
		p_initialize_button_list(this, a2);
		*(void**)this = old_vtbl;
	}

	// overrides, only needed ones
	virtual int custom_deconstructor(BYTE flags) override
	{
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->* * pFn)(flags);
	}

	virtual int IUnkFunc2() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(1);
		return (this->* * pFn)();
	}

	virtual void IUnkFunc22() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(21);
		return (this->* * pFn)();
	}

	virtual char* get_item_list() override
	{
		typedef char* (class_type::** fnT)();
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(22);
		return (this->* * pFn)();
	}

	virtual int get_list_item_count() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(23);
		return (this->* * pFn)();
	}

	virtual void get_label(int a1, int a2) override
	{
		typedef void(class_type::** fnT)(int, int);
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(24);
		return (this->* * pFn)(a1, a2);
	}

	virtual bool IUnkFunc26(int a1) override
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_brightness_level_edit_list_base_vtable_get_func_ptr<fnT>(25);
		return (this->* * pFn)(a1);
	}

private:
	typedef c_brightness_level_edit_list class_type;

	template<typename T>
	static T c_brightness_level_edit_list_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7D972C)[idx]);
	}
};

static_assert(offsetof(c_brightness_level_edit_list, edit_list) == 176);

class c_brightness_menu : protected c_screen_with_menu
{
public:
	c_brightness_menu(int _ui_channel, int a4, __int16 _flags);

	c_brightness_menu::~c_brightness_menu()
	{
	}

	typedef c_brightness_menu class_type;

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<int(class_type::**)(int)>(0);
		return (this->**pFn)(flags);
	};

	virtual int IUnkFunc2() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(1);
		return (this->**pFn)();
	}

	virtual void IUnkFunc3() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(2);
		return (this->**pFn)();
	}

	virtual int IUnkFunc4() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(3);
		return (this->**pFn)();
	}

	virtual void IUnkFunc5_used_by_virtual_kb(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(4);
		(this->**pFn)(a2);
	}

	virtual DWORD IUnkFunc6(DWORD* a2) override
	{
		typedef DWORD(class_type::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(5);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc7(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(6);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc8() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(7);
		return (this->**pFn)();
	}

	virtual DWORD* IUnkFunc9(DWORD* a2) override
	{
		typedef DWORD* (class_type::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(8);
		return (this->**pFn)(a2);
	}

	virtual void IUnkFunc10(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(9);
		return (this->**pFn)(a1);
	}

	virtual int IUnkFunc11() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(10);
		return (this->**pFn)();
	}

	virtual int IUnkFunc12() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(11);
		return (this->**pFn)();
	}

	virtual int IUnkFunc13(int* a2) override
	{
		typedef int(class_type::** fnT)(int*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(12);
		return (this->**pFn)(a2);
	}

	virtual int get_top_most_parent_widget_ui_channel() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(13);
		return (this->**pFn)();
	}

	virtual int get_top_most_parent_widget_index() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(14);
		return (this->**pFn)();
	}

	virtual void IUnkFunc16_maybe_debug(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(15);
		return (this->**pFn)(a1);
	}

	virtual void IUnkFunc17_maybe_debug(int a1) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(16);
		return (this->**pFn)(a1);
	}

	virtual int IUnkFunc18(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(17);
		return (this->**pFn)(a2);
	}

	virtual bool IUnkFunc19() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(18);
		return (this->**pFn)();
	}

	virtual int IUnkFunc20() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(19);
		return (this->**pFn)();
	}

	virtual bool IUnkFunc21() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(20);
		return (this->**pFn)();
	}

	virtual void IUnkFunc22_maybe_debug() override
	{
		typedef void(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(21);
		return (this->**pFn)();
	}

	virtual int IUnkFunc23(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(22);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc24() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(23);
		return (this->**pFn)();
	}

	virtual BYTE* IUnkFunc25() override
	{
		typedef BYTE* (class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(24);
		return (this->**pFn)();
	}

	virtual int IUnkFunc26() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(25);
		return (this->**pFn)();
	}

	virtual int IUnkFunc27() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(26);
		return (this->**pFn)();
	}

	virtual int IUnkFunc28(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(27);
		return (this->**pFn)(a2);
	}

	virtual bool IUnkFunc29(int* a2) override
	{
		typedef bool(class_type::** fnT)(int*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(28);
		return (this->**pFn)(a2);
	}

	virtual bool IUnkFunc30(int a2) override
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(29);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc31(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(30);
		return (this->**pFn)(a2);
	}

	virtual bool IUnkFunc32(int a2) override
	{
		typedef bool(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(31);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc33() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(32);
		return (this->**pFn)();
	}

	virtual int IUnkFunc34() override
	{
		typedef int(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(33);
		return (this->**pFn)();
	}

	virtual unsigned int IUnkFunc35(int a2) override
	{
		typedef unsigned int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(34);
		return (this->**pFn)(a2);
	}

	virtual void IUnkFunc36_maybe_debug(int a2) override
	{
		typedef void(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(35);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc37(DWORD* a2) override
	{
		typedef int(class_type::** fnT)(DWORD*);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(36);
		return (this->**pFn)(a2);
	}

	virtual int IUnkFunc38(int a2) override
	{
		typedef int(class_type::** fnT)(int);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(37);
		return (this->**pFn)(a2);
	}

	virtual void* get_open_menu_cb() override
	{
		typedef void*(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(38);
		return (this->**pFn)();
	}

	virtual bool IUnkFunc40() override
	{
		typedef bool(class_type::** fnT)();
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(39);
		return (this->**pFn)();
	}

	virtual void IUnkFunc41(bool a2) override
	{
		typedef void(class_type::** fnT)(bool);
		auto pFn = c_brightness_menu_base_vtable_get_func_ptr<fnT>(40);
		(this->**pFn)(a2);
	}

	c_brightness_level_edit_list list_widgets;

	
	static void* open_brightness_menu(s_new_ui_screen_parameters* a1);

private:
	template<typename T>
	static T c_brightness_menu_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddressRelative<void**>(0x7D7ABC)[idx]);
	}
};

static_assert(sizeof(c_brightness_menu) == 3388);
static_assert(offsetof(c_brightness_menu, list_widgets) == 2656);

void replace_brightness_menu();
