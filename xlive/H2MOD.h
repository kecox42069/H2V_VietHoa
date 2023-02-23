#pragma once

#include "Blam\Engine\Objects\Objects.h"
#include "Blam\Engine\Objects\ObjectPlacementData.h"

#include "Blam\Engine\Game\GameOptions.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\Common\Common.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"

#include "H2MOD\Variants\Variants.h"

enum e_sound_type
{
	_sound_type_team_change,
	_sound_type_gained_the_lead,
	_sound_type_lost_the_lead,
	_sound_type_tied_leader,
	_sound_type_slayer,
};
#define ALL_SOUNDS_NO_SLAYER (FLAG(_sound_type_team_change) | FLAG(_sound_type_gained_the_lead) | FLAG(_sound_type_lost_the_lead) | FLAG(_sound_type_tied_leader))

enum e_static_lod : DWORD
{
	disable = 0,
	super_low,
	low,
	medium,
	high,
	super_high,
	cinematic
};

void call_give_player_weapon(int PlayerIndex, datum WeaponId, bool bReset);

class H2MOD
{
public:
	void Initialize();
	void Deinitialize();
	void ApplyHooks();
	void ApplyFirefightHooks();
	void RegisterEvents();

	void team_player_indicator_visibility(bool toggle);
	BYTE* get_player_unit_from_player_index(int playerIndex);
	datum get_player_datum_index_from_controller_index(int controller_index);
	wchar_t* get_local_player_name(int local_player_index);
	real_point3d* get_player_unit_coords(int player_index);
	float get_distance(int, int);
	int get_player_index_from_unit_datum_index(datum unit_datum_index);
	BYTE get_unit_team_index(datum unit_datum_index);
	void set_unit_speed_patch(bool hackit);
	void set_local_team_index(int local_player_index, int team);
	void set_local_clan_tag(int local_player_index, XUID tag);
	BYTE get_local_team_index();
	void set_player_unit_grenades_count(int playerIndex, e_grenades type, BYTE count, bool resetEquipment);
	void disable_sounds(int sound);
	void custom_sound_play(const wchar_t* soundName, int delay);
	void disable_weapon_pickup(bool b_Enable);
	void set_local_rank(BYTE rank);
	void cine_start_tex();

	e_engine_type GetEngineType() { return engineType; }
	void SetCurrentEngineType(e_engine_type value) { engineType = value; }

	std::deque<std::wstring> CustomSounds;

	std::mutex sound_mutex;

	std::set<int> hookedObjectDefs;
	bool drawTeamIndicators = true;

private:
	e_engine_type engineType;
};

extern H2MOD* h2mod;
extern s_data_array* game_state_actors;
