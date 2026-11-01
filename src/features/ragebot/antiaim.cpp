#include "../../Precompiled.h"

#define CONFIG_GET(type, path) Config::Get<type>(path);

#define RED_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position,   { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 255, 0, 0     }, 0.1f)
#define GREEN_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 255, 0     }, 0.1f)
#define BLUE_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position,  { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 0, 255     }, 0.1f)
#define WHITE_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 255, 255, 255 }, 0.1f)
#define BLACK_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 0, 0       }, 0.1f)

#define LEFT_ANGLE -90.f
#define RIGHT_ANGLE 90.f
#define NO_ANGLE 0.f

struct HighestThreat_t
{
	QAngle m_aAngle;
	C_CSPlayerPawn* m_pPawn;
};

static HighestThreat_t GetHighestThreat( )
{
	Vector vLocalEyePos = Globals::m_pLocalPlayerPawn->GetEyePosition( true );
	QAngle aLocalAngles = g_Movement->m_angCameraAngles;
	Vector vViewForward, vViewRight, vViewUp;
	Math::AngleVectors( aLocalAngles, vViewForward, vViewRight, vViewUp );

	float flNearestFOV = FLT_MAX;
	QAngle aBestAngle = aLocalAngles;
	C_CSPlayerPawn* pBestPawn = nullptr;

	for ( EntityObject_t& EntityObject : g_Entities->m_vecPlayersOnly )
	{

		CCSPlayerController* pController = static_cast<CCSPlayerController*>( EntityObject.m_pEntity );
		if ( !pController || pController == Globals::m_pLocalPlayerController )
			continue;

		C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>( Interfaces::m_pGameResourceService->pGameEntitySystem->Get( pController->m_hPawn( ) ) );
		if ( !pPawn )
			continue;

		if ( !pController->m_bPawnIsAlive( ) || !pPawn->IsEnemy( Globals::m_pLocalPlayerPawn ) )
			continue;

		Vector vecTarget = pPawn->GetEyePosition( );

		Vector vDelta = vecTarget - vLocalEyePos;
		vDelta.NormalizeInPlace( );

		float flDot = vViewForward.DotProduct( vDelta );
		float flFOV = M_RAD2DEG( acosf( flDot ) );

		if ( flFOV < flNearestFOV )
		{
			flNearestFOV = flFOV;
			aBestAngle = Math::CalcAngle( vLocalEyePos, vecTarget );
			pBestPawn = pPawn;
		}
	}

	return { aBestAngle, pBestPawn };
}

static GameTrace_t TraceShapeWrapper(Vector vStart, Vector vEnd)
{
	GameTrace_t trace;
	TraceFilter_t filter(TRACE_MASK, Globals::m_pLocalPlayerPawn, nullptr, 4);
	Ray_t ray;

	Interfaces::m_pVPhys2World->TraceShape(&ray, vStart, vEnd, &filter, &trace);

	return trace;
}

static int GetFreeStandingYaw()
{
	if (!Config::b(g_Variables.m_Ragebot.m_bFreestanding))
		return 0;

	HighestThreat_t objHighestThreat = GetHighestThreat();

	bool bEnemyValid = objHighestThreat.m_pPawn;

	const Vector vEyePos = NRagebot::m_vShootPos;
	const Vector vEnemyEyePos = bEnemyValid ? objHighestThreat.m_pPawn->GetEyePosition() : Vector(0.f, 0.f, 0.f);

	bool bAtTarget = Config::b(g_Variables.m_Ragebot.m_bAtTarget);
	QAngle aViewAngles = bAtTarget ? QAngle(0.f, objHighestThreat.m_aAngle.y, 0.f) : QAngle(0.f, Interfaces::m_pInput->GetViewAngles().y, 0.f);

	Matrix3x4_t mViewMatrix = {};
	Math::AngleMatrix(aViewAngles, mViewMatrix);

	Vector vLeftHeadPosAALocal = { 0.f, -15.f, 0.f };

	Vector vRightHeadPosAALocal = { 0.f, 15.f, 0.f };

	Vector vLeftLocal = { 0.f, -25.f, 0.f };

	Vector vLeftFrontLocal = { 50.f, -25.f, 0.f };

	Vector vRightLocal = { 0.f, 25.f, 0.f };

	Vector vRightFrontLocal = { 50.f, 25.f, 0.f };

	Vector vLeftWorld, vRightWorld, vLeftFrontWorld, vRightFrontWorld, vLeftHeadPosAAWorld, vRightHeadPosAAWorld;
	Math::VectorRotate(vLeftLocal, mViewMatrix, vLeftWorld);
	Math::VectorRotate(vRightLocal, mViewMatrix, vRightWorld);
	Math::VectorRotate(vLeftFrontLocal, mViewMatrix, vLeftFrontWorld);
	Math::VectorRotate(vRightFrontLocal, mViewMatrix, vRightFrontWorld);
	Math::VectorRotate(vLeftHeadPosAALocal, mViewMatrix, vLeftHeadPosAAWorld);
	Math::VectorRotate(vRightHeadPosAALocal, mViewMatrix, vRightHeadPosAAWorld);

	vLeftWorld += vEyePos;
	vRightWorld += vEyePos;
	vLeftFrontWorld += vEyePos;
	vRightFrontWorld += vEyePos;

	if (Config::b(g_Variables.m_Misc.m_bDebug))
	{
		RED_DEBUG_BOX(vEyePos);
		BLUE_DEBUG_BOX(vLeftWorld);
		GREEN_DEBUG_BOX(vRightWorld);
		WHITE_DEBUG_BOX(vLeftFrontWorld);
		BLACK_DEBUG_BOX(vRightFrontWorld);
	}

	GameTrace_t objLeftGameTrace = TraceShapeWrapper(vLeftWorld, vLeftFrontWorld);
	GameTrace_t objRightGameTrace = TraceShapeWrapper(vRightWorld, vRightFrontWorld);

	int iReturnYaw = NO_ANGLE;

	if (objLeftGameTrace.m_flFraction != objRightGameTrace.m_flFraction)
	{
		if (objLeftGameTrace.m_flFraction > objRightGameTrace.m_flFraction)
		{

			if (bAtTarget && bEnemyValid)
			{
				GameTrace_t objGameTrace = TraceShapeWrapper(vEnemyEyePos, vLeftHeadPosAAWorld);
				if (!objGameTrace.IsVisible())
					iReturnYaw = LEFT_ANGLE;
			}
			else
			{
				iReturnYaw = LEFT_ANGLE;
			}
		}
		else
		{

			if (bAtTarget && bEnemyValid)
			{
				GameTrace_t objGameTrace = TraceShapeWrapper(vEnemyEyePos, vRightHeadPosAAWorld);
				if (!objGameTrace.IsVisible())
					iReturnYaw = RIGHT_ANGLE;
			}
			else
			{
				iReturnYaw = RIGHT_ANGLE;
			}
		}
	}

	return iReturnYaw;
}

float CAntiAim::GetYaw()
{
	int iYaw = Config::i(g_Variables.m_Ragebot.m_iYaw);

	KeyBind_t& leftKeybind = Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind);
	KeyBind_t& rightKeybind = Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind);

	static bool bPrevLeftActive = false;
	static bool bPrevRightActive = false;

	bool bLeftActive = Input::HandleInput(leftKeybind);
	bool bRightActive = Input::HandleInput(rightKeybind);

	if (bLeftActive && !bPrevLeftActive && leftKeybind.m_iMode == TOGGLE && rightKeybind.m_iMode == TOGGLE)
	{
		if (bRightActive)
		{
			Input::arrKeyBooleans[rightKeybind.m_iKey] = false;
			bRightActive = false;
		}
	}

	if (bRightActive && !bPrevRightActive && leftKeybind.m_iMode == TOGGLE && rightKeybind.m_iMode == TOGGLE)
	{
		if (bLeftActive)
		{
			Input::arrKeyBooleans[leftKeybind.m_iKey] = false;
			bLeftActive = false;
		}
	}

	bPrevLeftActive = bLeftActive;
	bPrevRightActive = bRightActive;

	if (bLeftActive)
	{
		iYaw -= 90.f;
		return iYaw;
	}

	else if (bRightActive)
	{
		iYaw += 90.f;
		return iYaw;
	}

	iYaw += GetFreeStandingYaw();

	return static_cast<float>(iYaw);
}

float CAntiAim::GetPitch()
{
	float m_iPitch = Config::i(g_Variables.m_Ragebot.m_iPitch);
	if (g_Variables.m_Ragebot.m_bAntiaimLeft)
	return m_iPitch;
}

float CAntiAim::GetRoll()
{
	return Config::i(g_Variables.m_Ragebot.m_iRoll);
}

static int iSpin = 0;
void CAntiAim::OnCreateMove()
{
	if (!Config::b(g_Variables.m_Ragebot.m_bAntiaim))
		return;

	if (!Globals::m_pCmd)
		return;

	if (Globals::m_pCmd->m_nButtons.m_nValue & IN_USE)
		return;

	if ( LocalPlayerData::m_pWeaponBaseVData->m_WeaponType( ) == EWeaponType::WEAPONTYPE_GRENADE ) {
		C_BaseCSGrenade* pNade = (C_BaseCSGrenade*)LocalPlayerData::m_pWeapon;
		const bool bPinPulled = pNade->m_bPinPulled( );
		const float fThrowTime = pNade->m_fThrowTime( );

		if ( !bPinPulled && fThrowTime > 0.f )
			return;
	}

	if (Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_LADDER || Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_NOCLIP)
		return;

	if (Interfaces::m_pGameRules->m_bFreezePeriod())
		return;

	Vector pModifiedViewAngles = Vector(Globals::m_pBaseCmd->viewangles().x(), Globals::m_pBaseCmd->viewangles().y(), Globals::m_pBaseCmd->viewangles().z());

	if (Config::b(g_Variables.m_Ragebot.m_bAtTarget) && !Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind)) && !Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind)))
	{
		pModifiedViewAngles.y = GetHighestThreat().m_aAngle.y;
	}

	pModifiedViewAngles.x = this->GetPitch();
	pModifiedViewAngles.y += this->GetYaw();
	pModifiedViewAngles.z = this->GetRoll();

	if (Config::b(g_Variables.m_Ragebot.m_bRandomizeYaw))
	{
		float flRange = Config::f(g_Variables.m_Ragebot.m_flRandomizeYawRange);
		float flRandomYaw = Math::fnRandomFloat(-flRange, flRange);
		pModifiedViewAngles.y += flRandomYaw;
	}

	if (Config::b(g_Variables.m_Ragebot.m_bUnhittableYaw))
	{
		auto nTickBase = Globals::m_pLocalPlayerController->m_nTickBase();
		if (nTickBase % 4)
		{
			pModifiedViewAngles.y -= Math::fnRandomFloat(60, 80);
		}
		else
		{
			pModifiedViewAngles.y += Math::fnRandomFloat(60, 80);
		}
	}

	if (Config::b(g_Variables.m_Ragebot.m_bYawJitter))
	{
		static float flYawJitterMult = 0.f;
		if (flYawJitterMult == 0.f)
			flYawJitterMult = 1.f;

		flYawJitterMult *= -1.f;

		pModifiedViewAngles.y += Config::i(g_Variables.m_Ragebot.m_iYawJitter) * flYawJitterMult;
	}

	if (Config::b(g_Variables.m_Ragebot.m_bSpinAA))
	{
		iSpin += Config::i(g_Variables.m_Ragebot.m_iSpinAASpeed);
	}
	else if (abs(iSpin) > 0)
	{
		iSpin = 0;
	}

	pModifiedViewAngles.y += iSpin;

	pModifiedViewAngles.Normalize();

	Globals::SetBaseViewAngles(pModifiedViewAngles);

}
