#pragma once
#include "memory/Memory.h"

#define CLIENT_DLL X(L"client.dll")
#define SERVER_DLL X(L"server.dll")
#define ENGINE2_DLL X(L"engine2.dll")
#define SCHEMASYSTEM_DLL X(L"schemasystem.dll")
#define INPUTSYSTEM_DLL X(L"inputsystem.dll")
#define SDL3_DLL X(L"SDL3.dll")
#define TIER0_DLL X(L"tier0.dll")
#define NAVSYSTEM_DLL X(L"navsystem.dll")
#define RENDERSYSTEM_DLL X(L"rendersystemdx11.dll")
#define LOCALIZE_DLL X(L"localize.dll")
#define FILESYSTEM_STDIO_DLL X(L"filesystem_stdio.dll")
#define DBGHELP_DLL X(L"dbghelp.dll")
#define PARTICLES_DLL X(L"particles.dll")
#define SCENESYSTEM_DLL X(L"scenesystem.dll")
#define MATERIAL_SYSTEM2_DLL X(L"materialsystem2.dll")
#define MATCHMAKING_DLL X(L"matchmaking.dll")
#define RESOURCESYSTEM_DLL X(L"resourcesystem.dll")
#define GAMEOVERLAYRENDERER64_DLL X(L"gameoverlayrenderer64.dll")
#define ANIMATIONSYSTEM_DLL X(L"animationsystem.dll")
#define SOUNDSYSTEM_DLL X(L"soundsystem.dll")
#define WORLDRENDER_DLL X(L"worldrenderer.dll")

#define STEAM_API64_DLL X(L"steam_api64.dll")

#define INTERVAL_PER_TICK 0.015625f
#define TIME_TO_TICKS(TIME) (static_cast<int>(0.5f + static_cast<float>(TIME) / INTERVAL_PER_TICK))
#define TICKS_TO_TIME(TICKS) (INTERVAL_PER_TICK * static_cast<float>(TICKS))
#define ROUND_TO_TICKS(TIME) (INTERVAL_PER_TICK * TIME_TO_TICKS(TIME))
#define TICK_NEVER_THINK (-1)

#define MAX_COORD_FLOAT		16'384.f			// max world coordinates
#define MIN_COORD_FLOAT		-(MAX_COORD_FLOAT)	// min world coordinates
#define MAX_POSITION_FLT    32768.0f			// max entity position
#define MAX_DISTANCE		8'192.f
#define MAX_FOV				180.f
#define MAX_DAMAGE			500.f
#define MAX_WEAPONS			64

class CCSPlayerController;
class C_CSPlayerPawn;

class CClientNetworkData
{
public:
	struct CLocalData
	{
		QAngle m_angViewAngles;
		Vector m_vecShootPostion;
		__int32 m_nTargetEntityIndex;
		char pad_0x001C[0x20C];
	};

	MEM_PAD(0x4);
	std::int32_t m_nRenderTickCount;
	std::float_t m_flRenderTickFraction;
	std::int32_t m_nPlayerTickCount;
	std::float_t m_flPlayerTickFraction;
	MEM_PAD(0x4);
	CLocalData* m_pLocalData;
	MEM_PAD(1000);
};
namespace Globals
{
	inline HMODULE m_hDll = nullptr;
	inline bool m_bIsUnloading = false;

	inline CClientNetworkData m_NetClientInfo = { };

	inline CCSPlayerController* m_pLocalPlayerController = nullptr;
	inline C_CSPlayerPawn* m_pLocalPlayerPawn = nullptr;
	inline CUserCmd* m_pCmd = nullptr;
	inline CUserCmd* m_pNiggaRetardismCmd = nullptr;
	inline CBaseUserCmdPB* m_pBaseCmd = nullptr;

	inline bool				m_bIsPressingMovementKey = false;

	inline ViewMatrix_t		m_ViewMatrix = { };

	inline bool				m_bRequestFullUpdate = false;

	inline int nSequence = 0;
	inline int nLastCmdType = 0;
	inline bool bLastCmdPredicted = false;

	inline void SetBaseViewAngles(Vector& angViewAngles)
	{
		CMsgQAngle* pMutableViewAngles = Globals::m_pBaseCmd->mutable_viewangles();
		pMutableViewAngles->set_x(angViewAngles.x);
		pMutableViewAngles->set_y(angViewAngles.y);
		pMutableViewAngles->set_z(angViewAngles.z);
	}

	bool DuplicatePawn(C_CSPlayerPawn* pPawn, int nIndex);
}

class C_BasePlayerWeapon;
class CCSWeaponBaseVData;
class CGameSceneNode;
class C_CSWeaponBase;
namespace LocalPlayerData
{

	inline Vector m_vecOrigin = { };
	inline Vector m_vecAbsOrigin = { };
	inline Vector m_vecVelocity = { };
	inline Vector m_vecAbsVelocity = { };
	inline Vector m_vecEyePosition = { };

	inline float m_flSurfaceFriction = 0.f;
	inline float m_flMaxPlayerSpeed = 0.f;
	inline int m_nTickBase = 0;

	inline std::uint16_t m_nWeaponDefinitionIndex = 0U;
	inline bool m_bDidWeaponChange = false;
	inline bool m_bRageNeedsConfigUpdate = false;
	inline C_CSWeaponBase* m_pWeapon = nullptr;
	inline CCSWeaponBaseVData* m_pWeaponBaseVData = nullptr;

	bool Update();

	void Reset();
}

class CTimer
{
public:
	CTimer(const bool bStart = false)
	{
		if (bStart)
			Reset();
	}

	void Reset()
	{
		timePoint = std::chrono::high_resolution_clock::now();
	}

	template <class C = std::chrono::milliseconds>
	[[nodiscard]] auto Elapsed() const
	{
		return std::chrono::duration_cast<C>(std::chrono::high_resolution_clock::now() - timePoint).count();
	}

private:
	std::chrono::high_resolution_clock::time_point timePoint = { };
};