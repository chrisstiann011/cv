#include "../../Precompiled.h"

constexpr int kMovementKeyBits =
IN_TURNLEFT |
IN_FORWARD |
IN_BACK |
IN_TURNRIGHT |
IN_MOVELEFT |
IN_MOVERIGHT |
IN_JUMP;

float ApplyFriction(Vector& vVecVelocity)
{
	auto& pawn = Globals::m_pLocalPlayerPawn;
	auto& pMoveService = pawn->m_pMovementServices();

	CONVAR(sv_friction);
	CONVAR(sv_stopspeed);

	float flSpeed = 0.0f;

	if (Interfaces::m_pGlobalVariables->m_flCurrentTime > pMoveService->m_flOffsetTickCompleteTime())
		flSpeed = vVecVelocity.Length();
	else
		flSpeed = pMoveService->m_flOffsetTickStashedSpeed();

	float flSpeedReduction = 0.0f;

	if (flSpeed >= 0.1f) {
		if (pawn->m_hGroundEntity().Get()) {
			float friction = pMoveService->m_flSurfaceFriction() * sv_friction->GetFloat();
			flSpeedReduction = std::fmax(flSpeed, sv_stopspeed->GetFloat()) * friction * pawn->m_flFriction() * Interfaces::m_pGlobalVariables->m_flIntervalPerSubTick;
		}
		float flNewSpeed = fmaxf(flSpeed - flSpeedReduction, 0.f);
		if (flNewSpeed != flSpeed)
		{
			vVecVelocity *= flNewSpeed / flSpeed;
		}
	}
	return fmaxf(-(flSpeed - flSpeedReduction), 0.0);

}

Vector CMovement::SimulatePosAtStop(Vector vPos)
{
	Vector vVecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
	vVecVelocity.z = 0.f;

	bool bAutostopEnabled = Config::b(g_Variables.m_Ragebot.m_bAutostop) || g_Legitbot->m_Data.m_bWants2DMoveHalt;
	if (!bAutostopEnabled)
		return vPos;

	bool bAutostopEarly = Config::vb(g_Variables.m_Ragebot.m_vbAutostop).at(EAutostop::AUTOSTOP_EARLY) ||
	                      (g_Legitbot->m_Data.m_bWants2DMoveHalt && g_Legitbot->m_Data.m_vbAutostop.size() > EAutostop::AUTOSTOP_EARLY && g_Legitbot->m_Data.m_vbAutostop.at(EAutostop::AUTOSTOP_EARLY));
	if (!bAutostopEarly)
		return vPos;

	if (Convar::weapon_accuracy_nospread->m_value.m_i1)
		return vPos;

	if (!(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		return vPos;

	CONVAR(sv_accelerate);
	CONVAR(sv_accelerate_use_weapon_speed);
	CONVAR(sv_water_slow_amount);

	float flSurfaceFriction = Globals::m_pLocalPlayerPawn->m_pMovementServices()->m_flSurfaceFriction();

	int nMaxSimTicks = 2;

	auto& pPlayer = Globals::m_pLocalPlayerPawn;
	auto& pMoveService = pPlayer->m_pMovementServices();
	auto& pWeapon = LocalPlayerData::m_pWeapon;
	auto& pCmd = Globals::m_pCmd;

	Vector vExtrap = vPos;

	for (int i = 0; i < nMaxSimTicks; i++) {

		if (vVecVelocity.Length2D() <= LocalPlayerData::m_pWeapon->GetMaxSpeed() * 0.15f)
			break;

		float flFrictionDecel = ApplyFriction(vVecVelocity);

		Vector vWishDir = -vVecVelocity;

		float flWishSpeed = vWishDir.NormalizeInPlace();

		float flCurrentSpeed = vVecVelocity.DotProduct(vWishDir);

		int nButtonFlags = pCmd->m_nButtons.m_nValue;
		bool bIsDucking = (nButtonFlags & IN_DUCK) ||
			pMoveService->m_bDucking() ||
			(pPlayer->m_fFlags() & FL_DUCKING);
		bool bIsHoldingSprintKey = (nButtonFlags & IN_SPRINT) && !bIsDucking;

		float v19_clampedWishSpeed = std::max(250.0f, flWishSpeed);
		float v20_effectiveAbsMaxSpeed = v19_clampedWishSpeed;
		float v21_accelerationFactor = 1.0f;
		bool v23_weaponCausesSlowdown = false;

		if (sv_accelerate_use_weapon_speed->GetBool() && pWeapon) {
			float flWeaponMaxSpeed = pWeapon->GetMaxSpeed();
			auto pVData = pWeapon->GetWeaponBaseVData();

			if (pVData && pWeapon->m_zoomLevel() > 0 && pVData->m_nZoomLevels() > 1 && (flWeaponMaxSpeed * 0.51999998f) < 110.0f) {
				v23_weaponCausesSlowdown = true;
			}

			float flWeaponSpeedRatio = std::min(1.0f, flWeaponMaxSpeed / 250.0f);

			if ((!bIsDucking && !bIsHoldingSprintKey) || v23_weaponCausesSlowdown) {
				v21_accelerationFactor = flWeaponSpeedRatio;
			}

			v20_effectiveAbsMaxSpeed = v19_clampedWishSpeed * flWeaponSpeedRatio;
		}

		float v29_baseSpeedForAccelCalc = v19_clampedWishSpeed;

		unsigned int uWaterLevel = (pPlayer->m_flWaterLevel() * 4.f) + 1.f;

		if (uWaterLevel >= 2) {
			float flWaterSlowAmount = sv_water_slow_amount->GetFloat();
			v20_effectiveAbsMaxSpeed *= flWaterSlowAmount;

			if (bIsDucking) {
				v20_effectiveAbsMaxSpeed *= 0.34f;
				v21_accelerationFactor = std::min(0.34f, v21_accelerationFactor);
			}
			else {
				if (!bIsHoldingSprintKey) {
					v29_baseSpeedForAccelCalc = v19_clampedWishSpeed * flWaterSlowAmount;
				}
			}
		}
		else {
			if (bIsDucking) {
				v20_effectiveAbsMaxSpeed *= 0.34f;
				v21_accelerationFactor = std::min(0.34f, v21_accelerationFactor);
			}
		}

		float v29_finalSpeedCap = v29_baseSpeedForAccelCalc * v21_accelerationFactor;

		float flModifiedSvAccelerate = sv_accelerate->GetFloat();
		float flPositiveCurrentSpeed = std::max(0.0f, flCurrentSpeed);

		if (bIsHoldingSprintKey) {
			bool bIsCarryingHostage = pPlayer->m_pHostageServices() ? pPlayer->m_pHostageServices()->m_hCarriedHostage().Get() : false;

			if (!bIsCarryingHostage && !v23_weaponCausesSlowdown) {
				v29_finalSpeedCap *= 0.51999998f;
			}

			float flReducedAbsMaxSpeed = v20_effectiveAbsMaxSpeed * 0.51999998f;
			float flSprintSpeedThreshold = flReducedAbsMaxSpeed - 5.0f;

			if (flPositiveCurrentSpeed > flSprintSpeedThreshold) {
				float flNumerator = std::max(0.0f, flPositiveCurrentSpeed - flSprintSpeedThreshold);
				float flDenominator = std::max(0.001f, flReducedAbsMaxSpeed - flSprintSpeedThreshold);
				float flSpeedFactorRatio = flNumerator / flDenominator;
				float flAccelReductionFactor = std::min(1.0f, std::max(0.0f, 1.0f - flSpeedFactorRatio));
				flModifiedSvAccelerate *= flAccelReductionFactor;
			}
		}

		float flPotentialAccelGain = flModifiedSvAccelerate * INTERVAL_PER_TICK * v29_finalSpeedCap * flSurfaceFriction;
		float flAddSpeed = -flCurrentSpeed;
		float flAccelAmountToAdd = std::max(0.0f, std::min(flPotentialAccelGain, flAddSpeed));

		vVecVelocity.x += vWishDir.x * flAccelAmountToAdd;
		vVecVelocity.y += vWishDir.y * flAccelAmountToAdd;

		vExtrap.x += vVecVelocity.x * INTERVAL_PER_TICK;
		vExtrap.y += vVecVelocity.y * INTERVAL_PER_TICK;
	}

	return vExtrap;
}

void CMovement::StopMovement()
{
	CONVAR(weapon_accuracy_nospread);

	bool bQuickStop = (Config::b(g_Variables.m_Movement.m_bEnableQuickStop) && !Globals::m_bIsPressingMovementKey);
	if (!bQuickStop && weapon_accuracy_nospread->GetBool())
		return;

	if (bQuickStop || this->m_bWants2DMoveHalt)
	{
		bool bOnGround = Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND;

		bool bAutostopInAir = Config::vb(g_Variables.m_Ragebot.m_vbAutostop).at(EAutostop::AUTOSTOP_IN_AIR) ||
		                      (g_Legitbot->m_Data.m_bWants2DMoveHalt && g_Legitbot->m_Data.m_vbAutostop.size() > EAutostop::AUTOSTOP_IN_AIR && g_Legitbot->m_Data.m_vbAutostop.at(EAutostop::AUTOSTOP_IN_AIR));
		if (!bOnGround && !bAutostopInAir)
			return;

		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

		Vector vVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();

		pBaseCmd->set_leftmove(0.0f);
		pBaseCmd->set_forwardmove(vVelocity.Length2D() > 20.0f ? 1.0f : 0.0f);

		QAngle aViewAngles = this->m_angCameraAngles;
		float flYaw = Math::CalcAngle(Vector(0.0f, 0.0f, 0.0f), vVelocity).y + 180.0f;
		float flRotation = M_DEG2RAD(aViewAngles.y - flYaw);

		float flCosRotation = cos(flRotation);
		float flSinRotation = sin(flRotation);

		float flNewForwardMove = flCosRotation * pBaseCmd->forwardmove() - flSinRotation * pBaseCmd->leftmove();
		float flNewSideMove = flSinRotation * pBaseCmd->forwardmove() + flCosRotation * pBaseCmd->leftmove();

		pBaseCmd->set_forwardmove(flNewForwardMove);
		pBaseCmd->set_leftmove(-flNewSideMove);

		Globals::m_pCmd->m_nButtons.m_nValue &= ~kMovementKeyBits;
		Globals::m_pCmd->m_nButtons.m_nValueChanged &= ~kMovementKeyBits;
		Globals::m_pCmd->m_nButtons.m_nValueScroll &= ~kMovementKeyBits;

	}
}

#define RED_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position,   { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 255, 0, 0     }, 0.1f)
#define GREEN_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 255, 0     }, 0.1f)
#define BLUE_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position,  { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 0, 255     }, 0.1f)
#define WHITE_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 255, 255, 255 }, 0.1f)
#define BLACK_DEBUG_BOX(position) Interfaces::m_pClient->GetDebugOverlay()->AddBoxOverlay(position, { 1, 1, 1 }, { -1, -1, -1 }, { 0, 0, 0 }, { 0, 0, 0       }, 0.1f)
bool IsValidMovementTrace(GameTrace_t& tr, BBox_t bounds, TraceFilter_t* filter)
{
	GameTrace_t stuck;

	if (tr.m_bAllSolid)
	{
		return false;
	}

	if (tr.m_flFraction < 1.0f && fabs(tr.m_vecNormal.x) < FLT_EPSILON && fabs(tr.m_vecNormal.y) < FLT_EPSILON
		&& fabs(tr.m_vecNormal.z) < FLT_EPSILON)
	{
		return false;
	}

	if (fabs(tr.m_vecNormal.x) > 1.0f || fabs(tr.m_vecNormal.y) > 1.0f || fabs(tr.m_vecNormal.z) > 1.0f)
	{
		return false;
	}

	Interfaces::m_pVPhys2World->TracePlayerBBox(&tr.m_vecEndPos, &tr.m_vecEndPos, &bounds, filter, &stuck);
	if (stuck.m_bAllSolid || stuck.m_flFraction < 1.0f - FLT_EPSILON)
	{
		return false;
	}

	Interfaces::m_pVPhys2World->TracePlayerBBox(&tr.m_vecEndPos, &tr.m_vecStartPos, &bounds, filter, &stuck);

	if (stuck.m_bAllSolid )
	{
		return false;
	}

	return true;
}

void CMovement::BunnyHop()
{
	CONVAR(sv_autobunnyhopping);
	if (!Config::b(g_Variables.m_Movement.m_bEnableBunnyHop))
		return;

	if (Config::b(g_Variables.m_Movement.m_bEnableJumpBug))
		return;

	if (sv_autobunnyhopping->GetBool() || !(Globals::m_pCmd->m_nButtons.m_nValue & IN_JUMP))
		return;

	CUserCmd* pCmd = Globals::m_pCmd;
	CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

	auto pMovementServices = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	if ((g_Prediction->GetPostFlags() & FL_ONGROUND))
	{
		pCmd->m_nButtons.m_nValue &= ~IN_JUMP;
	}
}

void CMovement::BunnyHop2(CUserCmd* user_cmd)
{
	if (!Config::b(g_Variables.m_Movement.m_bEnableBunnyHop))
		return;

	const auto is_jumping = user_cmd->m_nButtons.m_nValue & IN_JUMP;
	if (!is_jumping)
		return;

	if (!(g_Prediction->GetPostFlags() & FL_ONGROUND))
	{
		user_cmd->m_nButtons.m_nValue &= ~IN_JUMP;
		user_cmd->m_nButtons.m_nValue &= ~IN_JUMP;
		user_cmd->m_nButtons.m_nValue &= ~IN_JUMP;
	}
	else
	{
		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

		auto cmd_push = pBaseCmd->add_subtick_moves();
		cmd_push->set_button(IN_JUMP);
		cmd_push->set_pressed(true);
		cmd_push->set_when(0.999f);

		auto cmd_release = pBaseCmd->add_subtick_moves();
		cmd_release->set_button(IN_JUMP);
		cmd_release->set_pressed(false);
		cmd_release->set_when(0.999f);
	}
}

void CMovement::JumpBug()
{
	CONVAR(sv_autobunnyhopping);

	if (!Config::b(g_Variables.m_Movement.m_bEnableJumpBug))
		return;

	if (sv_autobunnyhopping->GetBool() || !(Globals::m_pCmd->m_nButtons.m_nValue & IN_JUMP))
		return;

	CUserCmd* pCmd = Globals::m_pCmd;
	CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

	auto pMovementServices = Globals::m_pLocalPlayerPawn->m_pMovementServices();
	if (!pMovementServices)
		return;

	{
		static int iNextJump = 0;

		pBaseCmd->clear_subtick_moves();

		bool bOnGroundNextTick = (g_Prediction->GetPostFlags() & FL_ONGROUND) && !(g_Prediction->GetPreFlags() & FL_ONGROUND);
		bool bShouldJump = iNextJump - Globals::m_pLocalPlayerController->m_nTickBase() == 0;

		if (bOnGroundNextTick || bShouldJump)
		{
			CMoveData MoveData;

		}
		if (bShouldJump)
		{
			pCmd->m_nButtons.SetButtonState(IN_DUCK, CInButtonState::IN_BUTTON_UP);
			pCmd->m_nButtons.SetButtonState(IN_JUMP, CInButtonState::IN_BUTTON_UP_DOWN);

			CSubtickMoveStep* pJumpSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBaseCmd->mutable_subtick_moves());
			if (pJumpSubtick)
			{
				pJumpSubtick->set_when(0.f);
				pJumpSubtick->set_button(IN_JUMP);
				pJumpSubtick->set_pressed(true);

				pBaseCmd->mutable_subtick_moves()->AddAllocated(pJumpSubtick);
			}

			CSubtickMoveStep* pUnJumpSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBaseCmd->mutable_subtick_moves());
			if (pUnJumpSubtick)
			{
				pUnJumpSubtick->set_when(0.5f);
				pUnJumpSubtick->set_button(IN_JUMP);
				pUnJumpSubtick->set_pressed(false);

				pBaseCmd->mutable_subtick_moves()->AddAllocated(pUnJumpSubtick);
			}

		}
		else if (bOnGroundNextTick)
		{
			pCmd->m_nButtons.SetButtonState(IN_JUMP, CInButtonState::IN_BUTTON_UP);
			pCmd->m_nButtons.SetButtonState(IN_DUCK, CInButtonState::IN_BUTTON_UP_DOWN);

			CSubtickMoveStep* pDuckSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBaseCmd->mutable_subtick_moves());
			if (pDuckSubtick)
			{
				pDuckSubtick->set_when(0.f);
				pDuckSubtick->set_button(IN_JUMP);
				pDuckSubtick->set_pressed(true);
			}

			CSubtickMoveStep* pUnDuckSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBaseCmd->mutable_subtick_moves());
			if (pUnDuckSubtick)
			{
				pUnDuckSubtick->set_when(0.5f);
				pUnDuckSubtick->set_button(IN_JUMP);
				pUnDuckSubtick->set_pressed(false);

				pBaseCmd->mutable_subtick_moves()->AddAllocated(pUnDuckSubtick);
			}

			iNextJump = Globals::m_pLocalPlayerController->m_nTickBase() + 1;
		}
	}
}

void CMovement::DirectionalAirStrafe(Vector vVelocity, float flCurrentSpeed, float flFrameTime, bool bSubtickCalling)
{

	if (Config::b(g_Variables.m_Movement.m_bEnableAutoStrafeExploit) && !bSubtickCalling)
		return;

	if (!Config::b(g_Variables.m_Movement.m_bEnableAutoStrafe) && !bSubtickCalling)
		return;

	CONVAR(sv_airaccelerate);
	CONVAR(sv_air_max_wishspeed);

	auto pBase = Globals::m_pBaseCmd;
	auto pMoveService = Globals::m_pLocalPlayerPawn->m_pMovementServices();

	static uint64_t uLastPressed = 0;
	static uint64_t uLastButtons = 0;

	const uint64_t uCurrentButtons = Globals::m_pCmd->m_nButtons.m_nValue;

	const auto CheckButton = [&](const uint64_t uButton)
		{
			if (uCurrentButtons & uButton && (!(uLastButtons & uButton)
				|| (uButton & IN_MOVELEFT && !(uLastPressed & IN_MOVERIGHT))
				|| (uButton & IN_MOVERIGHT && !(uLastPressed & IN_MOVELEFT))
				|| (uButton & IN_FORWARD && !(uLastPressed & IN_BACK))
				|| (uButton & IN_BACK && !(uLastPressed & IN_FORWARD))))
			{
				if (uButton & IN_MOVELEFT)
					uLastPressed &= ~IN_MOVERIGHT;
				else if (uButton & IN_MOVERIGHT)
					uLastPressed &= ~IN_MOVELEFT;
				else if (uButton & IN_FORWARD)
					uLastPressed &= ~IN_BACK;
				else if (uButton & IN_BACK)
					uLastPressed &= ~IN_FORWARD;

				uLastPressed |= uButton;
			}
			else if (!(uCurrentButtons & uButton))
				uLastPressed &= ~uButton;
		};

	CheckButton(IN_MOVELEFT);
	CheckButton(IN_MOVERIGHT);
	CheckButton(IN_FORWARD);
	CheckButton(IN_BACK);

	uLastButtons = uCurrentButtons;

	if (uCurrentButtons & IN_SPRINT || Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_LADDER ||
		Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_NOCLIP ||
		g_Prediction->GetPostFlags() & FL_ONGROUND ||
		g_Prediction->GetPreFlags() & FL_ONGROUND)
		return;

	float flOffset = 0.f;
	if (uLastPressed & IN_MOVELEFT)
		flOffset += 90.f;

	if (uLastPressed & IN_MOVERIGHT)
		flOffset -= 90.f;

	if (uLastPressed & IN_FORWARD)
		flOffset *= 0.5f;

	else if (uLastPressed & IN_BACK)
		flOffset = -flOffset * 0.5f + 180.f;

	float flYaw = Math::FloatNormalize(Interfaces::m_pInput->GetViewAngles().y);

	flYaw += flOffset;

	pBase->set_forwardmove(0.f);
	pBase->set_leftmove(0.f);

	const float flVelocityAngle = Math::FloatNormalize(M_RAD2DEG(atan2f(vVelocity.y, vVelocity.x)));
	const float flSpeed = vVelocity.Length2D();
	const auto flIdeal = std::clamp(
		M_RAD2DEG(
			atan(
				fmax(
					15.f,
					30.f - (
						flCurrentSpeed *
						sv_airaccelerate->GetFloat() *
						pMoveService->m_flSurfaceFriction() *
						flFrameTime
						)
				) / flSpeed
			)
		),
		0.f,
		45.f
	);

	const float flVelocityDelta = Math::FloatNormalize(flYaw - flVelocityAngle);

	auto RotateMovement = [](CUserCmd* pCmd, CBaseUserCmdPB* pBaseCmd, float flTargetYaw)
		{
			const float flRotation = M_DEG2RAD(pBaseCmd->viewangles().y() - flTargetYaw);

			const float flNewForwardMove = cos(flRotation) * pBaseCmd->forwardmove() - sin(flRotation) * pBaseCmd->leftmove();
			const float flNewSideMove = sin(flRotation) * pBaseCmd->forwardmove() + cos(flRotation) * pBaseCmd->leftmove();

			pBaseCmd->set_forwardmove(std::clamp(flNewForwardMove, -1.f, 1.f));
			pBaseCmd->set_leftmove(std::clamp(flNewSideMove * -1.f, -1.f, 1.f));
		};

	if ((fabsf(flVelocityDelta) > 170.f && flSpeed > 80.f) || (flVelocityDelta > flIdeal && flSpeed > 80.f))
	{
		flYaw = flIdeal + flVelocityAngle;
		pBase->set_leftmove(-1.f);
		RotateMovement(Globals::m_pCmd, pBase, Math::FloatNormalize(flYaw));

		return;
	}

	static bool bSideSwitch = false;
	bSideSwitch = bSubtickCalling ? !bSideSwitch : Globals::nSequence % 2 == 0;

	if (-flIdeal <= flVelocityDelta || flSpeed <= 80.f) {
		if (bSideSwitch) {
			flYaw = flYaw - flIdeal;
			pBase->set_leftmove(-1.f);
		}
		else {
			flYaw = flIdeal + flYaw;
			pBase->set_leftmove(1.f);
		}
	}
	else {
		flYaw = flVelocityAngle - flIdeal;
		pBase->set_leftmove(1.f);
	}

	RotateMovement(Globals::m_pCmd, pBase, flYaw);
}

void CMovement::AirAccelerate(Vector& vVelocity, Vector vMoveImpulse, float& flStamina, float flFrameTime, float flYaw)
{
	CONVAR(sv_airaccelerate);
	CONVAR(sv_staminarecoveryrate);
	CONVAR(sv_air_max_wishspeed);

	float flCurrentSpeed = vVelocity.Length2D();
	if (flStamina > 0)
	{
		float flSpeedScale = std::clamp(1.0f - (flStamina / 100.f), 0.f, 1.f);
		flCurrentSpeed *= flSpeedScale * flSpeedScale;
		flStamina = fmaxf(flStamina - (flFrameTime * sv_staminarecoveryrate->GetFloat()), 0.f);
	}

	QAngle angViewAngles = { 0, flYaw, 0 };

	Vector vecForward, vecLeft, vecUp;
	angViewAngles.ToDirections(&vecForward, &vecLeft, &vecUp);
	vecForward.NormalizeInPlace();
	vecLeft.NormalizeInPlace();

	Vector vecWishDir(
		((vecForward.x * vMoveImpulse.x) * flCurrentSpeed) - ((vecLeft.x * vMoveImpulse.y) * flCurrentSpeed),
		((vecForward.y * vMoveImpulse.x) * flCurrentSpeed) - ((vecLeft.y * vMoveImpulse.y) * flCurrentSpeed),
		0.f
	);

	float flWishSpeed = vecWishDir.NormalizeInPlace();
	flWishSpeed = fminf(flWishSpeed, flCurrentSpeed);

	const float flSpeedAdd = fminf(flWishSpeed, sv_air_max_wishspeed->GetFloat() - vVelocity.DotProduct(vecWishDir));

	if (flSpeedAdd < 0.f)
		return;

	const float flAccelSpeed = fminf(flSpeedAdd, flWishSpeed * flFrameTime * sv_airaccelerate->GetFloat());
	vVelocity += (vecWishDir * flAccelSpeed) * 0.5f;

}

void CMovement::SubtickAirStrafe()
{

	CONVAR(sv_gravity);

	if (!Config::b(g_Variables.m_Movement.m_bEnableAutoStrafeExploit))
		return;

	if (Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_LADDER ||
		Globals::m_pLocalPlayerPawn->m_nActualMoveType() == MOVETYPE_NOCLIP ||
		(g_Prediction->GetPostFlags() & FL_ONGROUND) ||
		(g_Prediction->GetPreFlags() & FL_ONGROUND) ||
		(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		return;

	auto pBase = Globals::m_pBaseCmd;
	auto pLocal = Globals::m_pLocalPlayerPawn;
	auto pMoveService = pLocal->m_pMovementServices();

	pBase->clear_subtick_moves();

	float flFriction = pMoveService->m_flSurfaceFriction();
	float flStamina = pMoveService->m_flStamina();

	Vector vAbsVelocity = pLocal->m_vecAbsVelocity();
	Vector vLastImpulses = pMoveService->m_vecLastMovementImpulses();
	Vector vCMDMoveBackup = { pBase->forwardmove(), pBase->leftmove(), pBase->upmove() };

	const int nTicks = std::clamp(32 - pBase->subtick_moves_size(), 0, 32);
	const float flFrameTime = INTERVAL_PER_TICK / static_cast<float>(nTicks);
	float flMovementYaw = this->m_angModelAngles.y;

	for (int i = 0; i < nTicks; ++i)
	{
		const auto& pSubtick = Interfaces::m_pInput->CreateNewSubTickMoveStep(pBase->mutable_subtick_moves());
		if (!pSubtick)
			continue;

		pBase->set_forwardmove(vCMDMoveBackup.x);
		pBase->set_leftmove(vCMDMoveBackup.y);
		pBase->set_upmove(vCMDMoveBackup.z);

		this->AirAccelerate(vAbsVelocity, vLastImpulses, flStamina, flFrameTime, flMovementYaw);

		if (this->m_bWants2DMoveHalt) {
			this->StopMovement();
			this->MovementCorrection();
		}
		else {
			this->DirectionalAirStrafe(vAbsVelocity, vAbsVelocity.Length2D(), flFrameTime, true);
		}

		pSubtick->set_when(static_cast<float>(i) / static_cast<float>(nTicks));
		pSubtick->set_analog_forward_delta(pBase->forwardmove() - vLastImpulses.x);
		pSubtick->set_analog_left_delta(pBase->leftmove() - vLastImpulses.y);
		pSubtick->set_button(0);
		pSubtick->set_pressed(false);

		vLastImpulses.x += pSubtick->analog_forward_delta();
		vLastImpulses.y += pSubtick->analog_left_delta();

		pBase->mutable_subtick_moves()->AddAllocated(pSubtick);
	}
}

void CMovement::HandleMoveStop(bool bHaltMovement) {

	this->m_bWants2DMoveHalt = bHaltMovement;

	if (Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND)
	{
		this->StopMovement();
	}
}

void CMovement::HandleDirectionalStrafe()
{
	Config::b(g_Variables.m_Movement.m_bEnableAutoStrafeExploit) ? this->SubtickAirStrafe() : this->DirectionalAirStrafe(Globals::m_pLocalPlayerPawn->m_vecAbsVelocity(), LocalPlayerData::m_pWeapon->GetMaxSpeed(), 1.f, false);
}

void CMovement::MovementCorrection()
{
	auto& pBaseCmd = Globals::m_pBaseCmd;

	int sign = this->m_angModelAngles.x > 89.f ? -1.f : 1.f;
	this->m_angModelAngles.Normalize();

	Vector forward, right, up, old_forward, old_right, old_up;

	Math::AngleVectors(this->m_angModelAngles, forward, right, up);

	forward.z = right.z = up.x = up.y = 0.f;

	forward.NormalizeInPlace();
	right.NormalizeInPlace();
	up.NormalizeInPlace();

	Math::AngleVectors(this->m_angCameraAngles, old_forward, old_right, old_up);

	old_forward.z = old_right.z = old_up.x = old_up.y = 0.f;

	old_forward.NormalizeInPlace();
	old_right.NormalizeInPlace();
	old_up.NormalizeInPlace();

	forward *= pBaseCmd->forwardmove();
	right *= pBaseCmd->leftmove();
	up *= pBaseCmd->upmove();

	float fixed_forward_move = old_forward.DotProduct(right) + old_forward.DotProduct(forward) + old_forward.DotProduct(up, true);
	float fixed_side_move = old_right.DotProduct(right) + old_right.DotProduct(forward) + old_right.DotProduct(up, true);
	float fixed_up_move = old_up.DotProduct(right, true) + old_up.DotProduct(forward, true) + old_up.DotProduct(up);

	pBaseCmd->set_forwardmove(fixed_forward_move);
	pBaseCmd->set_leftmove(fixed_side_move);
	pBaseCmd->set_upmove(fixed_up_move);

	fixed_forward_move = sign * (old_forward.DotProduct(right) + old_forward.DotProduct(forward));
	fixed_side_move = old_right.DotProduct(right) + old_right.DotProduct(forward);

	pBaseCmd->set_forwardmove(std::clamp(fixed_forward_move, -1.f, 1.f));
	pBaseCmd->set_leftmove(std::clamp(fixed_side_move, -1.f, 1.f));
}
void CMovement::EdgeJump(CUserCmd* pCmd) {
	if (!Input::HandleInput(Config::kb(g_Variables.m_Movement.m_kEdgeJump)))
		return;

	if (!(Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND))
		pCmd->m_nButtons.SetButtonState(IN_JUMP, CInButtonState::EButtonState::IN_BUTTON_UP_DOWN);
}