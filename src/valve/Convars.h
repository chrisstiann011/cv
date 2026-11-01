#pragma once

#define CONVAR(name) static auto name = Interfaces::m_pCvar->Find(FNV1A::Hash(#name))

namespace Convar
{

	bool Dump( const wchar_t* wszFileName );

	bool Setup( );

	inline CConVar* m_pitch = nullptr;
	inline CConVar* m_yaw = nullptr;
	inline CConVar* sensitivity = nullptr;

	inline CConVar* game_type = nullptr;
	inline CConVar* game_mode = nullptr;

	inline CConVar* mp_teammates_are_enemies = nullptr;
	inline CConVar* weapon_accuracy_nospread = nullptr;

	inline CConVar* sv_autobunnyhopping = nullptr;

	inline CConVar* cl_interp = nullptr;
	inline CConVar* cl_updaterate = nullptr;

	inline CConVar* mp_damage_scale_ct_head = nullptr;
	inline CConVar* mp_damage_scale_t_head = nullptr;
	inline CConVar* mp_damage_scale_ct_body = nullptr;
	inline CConVar* mp_damage_scale_t_body = nullptr;
	inline CConVar* mp_damage_headshot_only = nullptr;

	inline CConVar* inferno_flame_lifetime = nullptr;
	inline CConVar* molotov_throw_detonate_time = nullptr;
	inline CConVar* weapon_molotov_maxdetonateslope = nullptr;

	inline CConVar* sv_maxunlag = nullptr;
	inline CConVar* sv_gravity = nullptr;
	inline CConVar* sv_airaccelerate = nullptr;
	inline CConVar* sv_cheats = nullptr;
	inline CConVar* sv_air_max_wishspeed = nullptr;

	inline CConVar* suspicious_hit_strategy = nullptr;
	inline CConVar* suspicious_hit_odds_threshold = nullptr;
	inline CConVar* suspicious_hit_player_radius = nullptr;

	inline CConVar* sv_quantize_movement_input = nullptr;
	inline CConVar* sv_showimpacts = nullptr;
	inline CConVar* cl_drawhud = nullptr;

}