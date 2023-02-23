#pragma once
#include "Blam\Engine\Players\PlayerActions.h"
#include "Blam\Cache\DataTypes\StringID.h"

struct s_unit_control_data
{
	string_id animation_state;
	unsigned __int16 aiming_speed;
	unsigned __int16 weapon_set_identifier;
	unsigned __int8 field_8;
	unsigned __int8 field_9;
	WORD grenade_index;
	unsigned __int16 zoom_level;
	char gap_E[2];
	DWORD control_flag0;
	DWORD control_flag1;
	real_vector3d throttle;
	float trigger;
	float secondary_trigger;
	real_vector3d desired_facing;
	real_vector3d desired_aiming;
	real_vector3d desired_looking;
	DWORD field_50;
	DWORD field_54;
	DWORD field_58;
	s_aim_assist_targetting_data target_info;

};
CHECK_STRUCT_SIZE(s_unit_control_data, 0x80);