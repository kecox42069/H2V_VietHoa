#pragma once

enum class hs_type : WORD {
	unparsed,
	special_form,
	function_name,
	passthrough,
	nothing,
	boolean,
	real,
	hs_short,
	hs_long,
	string,
	script,
	string_id,
	unit_seat_mapping,
	trigger_volume,
	cutscene_flag,
	cutscene_camera_point,
	cutscene_title,
	cutscene_recording,
	device_group,
	ai,
	ai_command_list,
	ai_command_script,
	ai_behavior,
	ai_orders,
	starting_profile,
	conversation,
	structure_bsp,
	navpoint,
	point_reference,
	style,
	hud_message,
	object_list,
	sound,
	effect,
	damage,
	looping_sound,
	animation_graph,
	damage_effect,
	object_definition,
	bitmap,
	shader,
	render_model,
	structure_definition,
	lightmap_definition,
	game_difficulty,
	team,
	actor_type,
	hud_corner,
	model_state,
	network_event,
	// objects
	object,
	unit,
	vehicle,
	weapon,
	device,
	scenery,
	object_name,
	unit_name,
	vehicle_name,
	weapon_name,
	device_name,
	scenery_name
};


static std::unordered_map <const hs_type, std::string> hs_type_string{
	{ hs_type::unparsed,	"unparsed" },
	{ hs_type::special_form,	"special form" },
	{ hs_type::function_name,	"function name" },
	{ hs_type::passthrough,	"passthrough" },
	{ hs_type::nothing,	"void" },
	{ hs_type::boolean,	"boolean" },
	{ hs_type::real,	"real" },
	{ hs_type::hs_short,	"short" },
	{ hs_type::hs_long,	"long" },
	{ hs_type::string,	"string" },
	{ hs_type::script,	"script" },
	{ hs_type::string_id,	"string_id" },
	{ hs_type::unit_seat_mapping,	"unit_seat_mapping" },
	{ hs_type::trigger_volume,	"trigger_volume" },
	{ hs_type::cutscene_flag,	"cutscene_flag" },
	{ hs_type::cutscene_camera_point,	"cutscene_camera_point" },
	{ hs_type::cutscene_title,	"cutscene_title" },
	{ hs_type::cutscene_recording,	"cutscene_recording" },
	{ hs_type::device_group,	"device_group" },
	{ hs_type::ai,	"ai" },
	{ hs_type::ai_command_list,	"ai_command_list" },
	{ hs_type::ai_command_script,	"ai_command_script" },
	{ hs_type::ai_behavior,	"ai_behavior" },
	{ hs_type::ai_orders,	"ai_orders" },
	{ hs_type::starting_profile,	"starting_profile" },
	{ hs_type::conversation,	"conversation" },
	{ hs_type::structure_bsp,	"structure_bsp" },
	{ hs_type::navpoint,	"navpoint" },
	{ hs_type::point_reference,	"point reference" },
	{ hs_type::style,	"style" },
	{ hs_type::hud_message,	"hud_message" },
	{ hs_type::object_list,	"object_list" },
	{ hs_type::sound,	"sound" },
	{ hs_type::effect,	"effect" },
	{ hs_type::damage,	"damage" },
	{ hs_type::looping_sound,	"looping_sound" },
	{ hs_type::animation_graph,	"animation_graph" },
	{ hs_type::damage_effect,	"damage_effect" },
	{ hs_type::object_definition,	"object_definition" },
	{ hs_type::bitmap,	"bitmap" },
	{ hs_type::shader,	"shader" },
	{ hs_type::render_model,	"render model" },
	{ hs_type::structure_definition,	"structure definition" },
	{ hs_type::lightmap_definition,	"lightmap definition" },
	{ hs_type::game_difficulty,	"game_difficulty" },
	{ hs_type::team,	"team" },
	{ hs_type::actor_type,	"actor_type" },
	{ hs_type::hud_corner,	"hud_corner" },
	{ hs_type::model_state,	"model_state" },
	{ hs_type::network_event,	"network_event" },
	// objects
	{ hs_type::object,	"object" },
	{ hs_type::unit,	"unit" },
	{ hs_type::vehicle,	"vehicle" },
	{ hs_type::weapon,	"weapon" },
	{ hs_type::device,	"device" },
	{ hs_type::scenery,	"scenery" },
	{ hs_type::object_name,	"object_name" },
	{ hs_type::unit_name,	"unit_name" },
	{ hs_type::vehicle_name,	"vehicle_name" },
	{ hs_type::weapon_name,	"weapon_name" },
	{ hs_type::device_name,	"device_name" },
	{ hs_type::scenery_name,	"scenery_name" },
};


struct HaloScriptGlobal
{
	hs_type type;
	BYTE padding[2];
	void* data;
};

typedef void** (__cdecl* hs_func_impl)(int hs_opcode, void* script_engine, char a3);

struct HaloScriptCommand
{
	hs_type type;
	WORD unk;
	hs_func_impl func;
	char* usage;
	WORD arg_count;
	hs_type arg_array[1]; // array size is dynamic, but always at least one
};
