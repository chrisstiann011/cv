#include "../Precompiled.h"

inline static void WriteConVarType( HANDLE hFile, const uint32_t nType )
{
	switch (( EConVarType )nType)
	{
	case EConVarType_Bool:
		::WriteFile( hFile, X( "[bool] " ), static_cast<DWORD>(CRT::StringLength( X( "[bool] " ) )), nullptr, nullptr );
		break;
	case EConVarType_Int16:
		::WriteFile( hFile, X( "[int16] " ), static_cast<DWORD>(CRT::StringLength( X( "[int16] " ) )), nullptr, nullptr );
		break;
	case EConVarType_UInt16:
		::WriteFile( hFile, X( "[uint16] " ), static_cast<DWORD>(CRT::StringLength( X( "[uint16] " ) )), nullptr, nullptr );
		break;
	case EConVarType_Int32:
		::WriteFile( hFile, X( "[int32] " ), static_cast<DWORD>(CRT::StringLength( X( "[int32] " ))), nullptr, nullptr );
		break;
	case EConVarType_UInt32:
		::WriteFile( hFile, X( "[uint32] " ), static_cast<DWORD>(CRT::StringLength( X( "[uint32] " ))), nullptr, nullptr );
		break;
	case EConVarType_Int64:
		::WriteFile( hFile, X( "[int64] " ), static_cast<DWORD>(CRT::StringLength( X( "[int64] " ))), nullptr, nullptr );
		break;
	case EConVarType_UInt64:
		::WriteFile( hFile, X( "[uint64] " ), static_cast<DWORD>(CRT::StringLength( X( "[uint64] " ))), nullptr, nullptr );
		break;
	case EConVarType_Float32:
		::WriteFile( hFile, X( "[float32] " ), static_cast<DWORD>(CRT::StringLength( X( "[float32] " ))), nullptr, nullptr );
		break;
	case EConVarType_Float64:
		::WriteFile( hFile, X( "[float64] " ), static_cast<DWORD>(CRT::StringLength( X( "[float64] " ))), nullptr, nullptr );
		break;
	case EConVarType_String:
		::WriteFile( hFile, X( "[string] " ), static_cast<DWORD>(CRT::StringLength( X( "[string] " ))), nullptr, nullptr );
		break;
	case EConVarType_Color:
		::WriteFile( hFile, X( "[color] " ), static_cast<DWORD>(CRT::StringLength( X( "[color] " ))), nullptr, nullptr );
		break;
	case EConVarType_Vector2:
		::WriteFile( hFile, X( "[vector2] " ), static_cast<DWORD>(CRT::StringLength( X( "[vector2] " ))), nullptr, nullptr );
		break;
	case EConVarType_Vector3:
		::WriteFile( hFile, X( "[vector3] " ), static_cast<DWORD>(CRT::StringLength( X( "[vector3] " ))), nullptr, nullptr );
		break;
	case EConVarType_Vector4:
		::WriteFile( hFile, X( "[vector4] " ), static_cast<DWORD>(CRT::StringLength( X( "[vector4] " ))), nullptr, nullptr );
		break;
	case EConVarType_Qangle:
		::WriteFile( hFile, X( "[qangle] " ), static_cast<DWORD>(CRT::StringLength( X( "[qangle] " ))), nullptr, nullptr );
		break;
	default:
		::WriteFile( hFile, X( "[unknown-type] " ), static_cast<DWORD>(CRT::StringLength( X( "[unknown-type] " ))), nullptr, nullptr );
		break;
	}
}

inline static void WriteConVarFlags( HANDLE hFile, const uint32_t nFlags )
{
	if (nFlags & FCVAR_CLIENTDLL)
		::WriteFile( hFile, X( "[client.dll] " ), static_cast<DWORD>(CRT::StringLength( X( "[client.dll] " ))), nullptr, nullptr );
	else if (nFlags & FCVAR_GAMEDLL)
		::WriteFile( hFile, X( "[games's dll] " ), static_cast<DWORD>(CRT::StringLength( X( "[games's dll] " ) )), nullptr, nullptr );

	if (nFlags & FCVAR_PROTECTED)
		::WriteFile( hFile, X( "[protected] " ), static_cast<DWORD>(CRT::StringLength( X( "[protected] " ))), nullptr, nullptr );

	if (nFlags & FCVAR_CHEAT)
		::WriteFile( hFile, X( "[cheat] " ), static_cast<DWORD>(CRT::StringLength( X( "[cheat] " ))), nullptr, nullptr );

	if (nFlags & FCVAR_HIDDEN)
		::WriteFile( hFile, X( "[hidden] " ), static_cast<DWORD>(CRT::StringLength( X( "[hidden] " ))), nullptr, nullptr );

	if (nFlags & FCVAR_DEVELOPMENTONLY)
		::WriteFile( hFile, X( "[devonly] " ), static_cast<DWORD>(CRT::StringLength( X( "[devonly] " ))), nullptr, nullptr );

	::WriteFile( hFile, X( "\n" ), static_cast<DWORD>(CRT::StringLength( X( "\n" ))), nullptr, nullptr );
}

bool Convar::Dump( const wchar_t* wszFileName )
{

	return true;
}

bool Convar::Setup( )
{
	bool bSuccess = true;

	m_pitch = Interfaces::m_pCvar->Find( FNV1A::HashConst( "m_pitch" ) );
	bSuccess &= m_pitch != nullptr;

	m_yaw = Interfaces::m_pCvar->Find( FNV1A::HashConst( "m_yaw" ) );
	bSuccess &= m_yaw != nullptr;

	sensitivity = Interfaces::m_pCvar->Find( FNV1A::HashConst( "sensitivity" ) );
	bSuccess &= sensitivity != nullptr;

	game_type = Interfaces::m_pCvar->Find( FNV1A::HashConst( "game_type" ) );
	bSuccess &= game_type != nullptr;

	game_mode = Interfaces::m_pCvar->Find( FNV1A::HashConst( "game_mode" ) );
	bSuccess &= game_mode != nullptr;

	mp_teammates_are_enemies = Interfaces::m_pCvar->Find( FNV1A::HashConst( "mp_teammates_are_enemies" ) );
	bSuccess &= mp_teammates_are_enemies != nullptr;

	weapon_accuracy_nospread = Interfaces::m_pCvar->Find(FNV1A::HashConst("weapon_accuracy_nospread"));
	bSuccess &= weapon_accuracy_nospread != nullptr;

	sv_autobunnyhopping = Interfaces::m_pCvar->Find( FNV1A::HashConst( "sv_autobunnyhopping" ) );
	bSuccess &= sv_autobunnyhopping != nullptr;

	mp_damage_scale_ct_head = Interfaces::m_pCvar->Find(FNV1A::HashConst("mp_damage_scale_ct_head"));
	bSuccess &= mp_damage_scale_ct_head != nullptr;

	mp_damage_scale_t_head = Interfaces::m_pCvar->Find(FNV1A::HashConst("mp_damage_scale_t_head"));
	bSuccess &= mp_damage_scale_t_head != nullptr;

	mp_damage_scale_ct_body = Interfaces::m_pCvar->Find(FNV1A::HashConst("mp_damage_scale_ct_body"));
	bSuccess &= mp_damage_scale_ct_body != nullptr;

	mp_damage_scale_t_body = Interfaces::m_pCvar->Find(FNV1A::HashConst("mp_damage_scale_t_body"));
	bSuccess &= mp_damage_scale_t_body != nullptr;

	mp_damage_headshot_only = Interfaces::m_pCvar->Find(FNV1A::HashConst("mp_damage_headshot_only"));
	bSuccess &= mp_damage_headshot_only != nullptr;

	inferno_flame_lifetime = Interfaces::m_pCvar->Find(FNV1A::HashConst("inferno_flame_lifetime"));
	bSuccess &= inferno_flame_lifetime != nullptr;

	molotov_throw_detonate_time = Interfaces::m_pCvar->Find(FNV1A::HashConst("molotov_throw_detonate_time"));
	bSuccess &= molotov_throw_detonate_time != nullptr;

	weapon_molotov_maxdetonateslope = Interfaces::m_pCvar->Find(FNV1A::HashConst("weapon_molotov_maxdetonateslope"));
	bSuccess &= weapon_molotov_maxdetonateslope != nullptr;

	sv_maxunlag = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_maxunlag"));
	bSuccess &= sv_maxunlag != nullptr;

	sv_gravity = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_gravity"));
	bSuccess &= sv_gravity != nullptr;

	sv_airaccelerate = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_airaccelerate"));
	bSuccess &= sv_airaccelerate != nullptr;

	sv_air_max_wishspeed = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_air_max_wishspeed"));
	bSuccess &= sv_air_max_wishspeed != nullptr;

	sv_cheats = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_cheats"));
	bSuccess &= sv_cheats != nullptr;

	sv_quantize_movement_input = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_quantize_movement_input"));
	bSuccess &= sv_quantize_movement_input != nullptr;

	suspicious_hit_strategy = Interfaces::m_pCvar->Find(FNV1A::HashConst("suspicious_hit_strategy"));
	bSuccess &= suspicious_hit_strategy != nullptr;

	suspicious_hit_odds_threshold = Interfaces::m_pCvar->Find(FNV1A::HashConst("suspicious_hit_odds_threshold"));
	bSuccess &= suspicious_hit_odds_threshold != nullptr;

	suspicious_hit_player_radius = Interfaces::m_pCvar->Find(FNV1A::HashConst("suspicious_hit_player_radius"));
	bSuccess &= suspicious_hit_player_radius != nullptr;

	sv_showimpacts = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_showimpacts"));
	bSuccess &= sv_showimpacts != nullptr;

	cl_drawhud = Interfaces::m_pCvar->Find(FNV1A::HashConst("cl_drawhud"));
	bSuccess &= cl_drawhud != nullptr;

	return bSuccess;
}