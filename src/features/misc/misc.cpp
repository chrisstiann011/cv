#include "misc.h"

void CMisc::AutoPurchase() {
	if (!Config::b(g_Variables.m_Misc.m_bAutoPurchase))
		return;

	std::string buy;

	switch (Config::i(g_Variables.m_Misc.m_iAutoPurchase1))
	{
	case 0:
	default:
		break;
	case 1:
		buy += X("buy g3sg1; buy scar20; ");
		break;
	case 2:
		buy += X("buy awp; ");
		break;
	case 3:
		buy += X("buy ssg08; ");
		break;
	}

	switch (Config::i(g_Variables.m_Misc.m_iAutoPurchase2))
	{
	case 0:
	default:
		break;
	case 1:
		buy += X("buy elite; ");
		break;
	case 2:
		buy += X("buy fiveseven; buy tec9; ");
		break;
	case 3:
		buy += X("buy deagle; ");
		break;
	case 4:
		buy += X("buy revolver; ");
		break;
	}

	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_ARMOR))
		buy += X("buy vesthelm; buy vest; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_TASER))
		buy += X("buy taser; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_GRENADES))
		buy += X("buy molotov; buy hegrenade; buy smokegrenade; ");
	if (Config::vb(g_Variables.m_Misc.m_uAutoPurchase3).at(EAutoPurchase::AUTOBUY_DEFUSER))
		buy += X("buy defuser; ");

	Interfaces::m_pEngine->ExecuteClientCmdUnrestricted(buy.c_str());
}

void CMisc::AutoPistol() {
	if (!Config::b(g_Variables.m_Misc.m_bAutoPistol))
		return;

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerPawn->m_pWeaponServices())
		return;

	C_BasePlayerWeapon* pWeapon = Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	if (!pWeapon || !pWeapon->GetWeaponBaseVData())
		return;

	C_AttributeContainer* pAttrib = pWeapon->m_AttributeManager();
	if (!pAttrib)
		return;

	C_EconItemView* pItem = pAttrib->m_Item();
	if (!pItem)
		return;

	int nWeaponID = pItem->m_iItemDefinitionIndex();
	if (!nWeaponID)
		return;

	if (nWeaponID == WEAPON_DUAL_BERETTAS)
		return;

	if (nWeaponID == WEAPON_R8_REVOLVER)
		return;

	if (!(Globals::m_pCmd->m_nButtons.m_nValue & IN_ATTACK))
		return;

	bool bIsPistol = pWeapon->GetWeaponBaseVData()->m_WeaponType() == WEAPONTYPE_PISTOL;
	bool bIsSSG08 = nWeaponID == WEAPON_SSG_08;

	if (!bIsPistol && !bIsSSG08)
		return;

	bool bCanShoot = (Interfaces::m_pGlobalVariables->m_nTickCount >= pWeapon->m_nNextPrimaryAttackTick());

	static bool bShouldFire = false;
	if (bCanShoot) {
		CBaseUserCmdPB* pBaseCmd = Globals::m_pBaseCmd;

		if (bShouldFire) {
			auto shoot_cmd = pBaseCmd->add_subtick_moves();
			shoot_cmd->set_button(IN_ATTACK);
			shoot_cmd->set_pressed(true);
			shoot_cmd->set_when(0.999f);

			auto release_cmd = pBaseCmd->add_subtick_moves();
			release_cmd->set_button(IN_ATTACK);
			release_cmd->set_pressed(false);
			release_cmd->set_when(0.999f);

			bShouldFire = false;
		}
		else {
			bShouldFire = true;
		}
	}
}

void CMisc::StraightThrow(const QAngle& pViewAngles) {

}

inline Vector2D rcs_old_punch{};

void standalone_rcs(CUserCmd* cmd, int shots_fired, const Vector2D& punch, float scale)
{
	if (shots_fired <= 1)
	{
		rcs_old_punch = punch;
		return;
	}
	Vector2D delta = (punch - rcs_old_punch) * scale;

	QAngle view_angles = Interfaces::m_pInput->GetViewAngles();
	view_angles -= QAngle(delta.x, delta.y, 0.f);

	view_angles.Clamp();

	Interfaces::m_pInput->SetViewAngle(view_angles);

	rcs_old_punch = punch;
}

void CMisc::RunRCS(C_CSPlayerPawn* local, CUserCmd* cmd, int shots_fired, float scale)
{
	if (!local)
		return;
	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;
	const QAngle& punch = local->m_aimPunchAngle();

	if (punch.IsZero())
		return;

	Vector2D punch_angle = {
		punch.x,
		punch.y
	};

	standalone_rcs(cmd, shots_fired, punch_angle, scale);
}

void CMisc::FreeLook(CCSGOInput* pInput, CUserCmd* pUserCmd)
{
	if (!Input::HandleInput(Config::kb(g_Variables.m_Misc.m_kbFreeLook)))
	{
		if (Globals::m_pLocalPlayerPawn)
			SetFreeLookOffset(Globals::m_pLocalPlayerPawn->GetEyePosition());
		return;
	}

	Vector vCurMove, vForward, vRight, vUp;

	QAngle ViewAngles = pInput->GetViewAngles();

	SetFreeLookAngle(ViewAngles);

	Math::AngleVectors(ViewAngles, vForward, vRight, vUp);

	if (pUserCmd->m_nButtons.m_nValue & IN_MOVELEFT)
		vCurMove = vCurMove + vRight * -1.0f;

	if (pUserCmd->m_nButtons.m_nValue & IN_MOVERIGHT)
		vCurMove = vCurMove + vRight * 1.0f;

	if (pUserCmd->m_nButtons.m_nValue & IN_FORWARD)
		vCurMove = vCurMove + vForward * 1.0f;

	if (pUserCmd->m_nButtons.m_nValue & IN_BACK)
		vCurMove = vCurMove + vForward * -1.0f;

	vCurMove = vCurMove * Config::f(g_Variables.m_Misc.m_flFreeLookSpeed);

	SetFreeLookOffset(GetFreeLookOffset() + vCurMove);

	if (Globals::m_pBaseCmd)
	{
		Globals::m_pBaseCmd->set_forwardmove(0.f);
		Globals::m_pBaseCmd->set_leftmove(0.f);
		Globals::m_pBaseCmd->set_upmove(0.f);
	}
}

void CMisc::ShowImpacts() {
	if (!Interfaces::m_pEngine || !Interfaces::m_pEngine->IsInGame())
		return;

	static bool bLastState = false;
	bool bCurrentState = Config::b(g_Variables.m_Misc.m_bShowImpacts);

	if (bCurrentState != bLastState) {
		if (bCurrentState) {

			if (Convar::sv_showimpacts) {
				Convar::sv_showimpacts->m_value.m_i32 = 1;
			}
			if (Interfaces::m_pEngine) {
				Interfaces::m_pEngine->ExecuteClientCmdUnrestricted("sv_showimpacts 1");
			}
		}
		else {

			if (Convar::sv_showimpacts) {
				Convar::sv_showimpacts->m_value.m_i32 = 0;
			}
			if (Interfaces::m_pEngine) {
				Interfaces::m_pEngine->ExecuteClientCmdUnrestricted("sv_showimpacts 0");
			}
		}
		bLastState = bCurrentState;
	}
}

void CMisc::ShowHud() {
	if (!Interfaces::m_pEngine || !Interfaces::m_pEngine->IsInGame())
		return;

	static bool bLastState = false;
	bool bCurrentState = Config::b(g_Variables.m_Misc.m_bShowHud);

	if (bCurrentState != bLastState) {
		if (bCurrentState) {

			if (Convar::cl_drawhud) {
				Convar::cl_drawhud->m_value.m_i32 = 0;
			}
			if (Interfaces::m_pEngine) {
				Interfaces::m_pEngine->ExecuteClientCmdUnrestricted("cl_drawhud 0");
			}
		}
		else {

			if (Convar::cl_drawhud) {
				Convar::cl_drawhud->m_value.m_i32 = 1;
			}
			if (Interfaces::m_pEngine) {
				Interfaces::m_pEngine->ExecuteClientCmdUnrestricted("cl_drawhud 1");
			}
		}
		bLastState = bCurrentState;
	}
}

void CMisc::DotPenetration() {
	if (!Config::b(g_Variables.m_Misc.m_bDotPenetration))
		return;

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	if (!LocalPlayerData::m_pWeapon || !LocalPlayerData::m_pWeaponBaseVData)
		return;

	const Vector vecStart = Globals::m_pLocalPlayerPawn->GetEyePosition(true);
	QAngle angView = Interfaces::m_pInput->GetViewAngles();

	Vector vForward, vRight, vUp;
	Math::AngleVectors(angView, vForward, vRight, vUp);

	const float flMaxDistance = MAX_DISTANCE;

	C_CSPlayerPawn* pTargetPawn = nullptr;
	float flClosestDistance = FLT_MAX;

	for (auto& objEntity : g_Entities->m_vecPlayersOnly)
	{
		CCSPlayerController* pController = (CCSPlayerController*)objEntity.m_pEntity;
		if (!pController)
			continue;

		if (pController == Globals::m_pLocalPlayerController)
			continue;

		C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn().Get();
		if (!pPawn)
			continue;

		if (pPawn == Globals::m_pLocalPlayerPawn)
			continue;

		if (!pController->m_bPawnIsAlive())
			continue;

		if (!Globals::m_pLocalPlayerPawn->IsEnemy(pPawn))
			continue;

		if (!pPawn->m_pGameSceneNode())
			continue;

		CSkeletonInstance* pSkeletonCheck = pPawn->m_pGameSceneNode()->GetSkeletonInstance();
		if (!pSkeletonCheck || !pSkeletonCheck->m_pBoneCache)
			continue;

		Vector vTargetPos = pPawn->m_pGameSceneNode()->GetBonePosition(6);
		if (!vTargetPos.IsValid() || vTargetPos.IsZero())
			continue;

		Vector vDirectionToTarget = (vTargetPos - vecStart).Normalized();
		float flDotProduct = vForward.DotProduct(vDirectionToTarget);

		if (flDotProduct < 0.0f)
			continue;

		float flDistance = (vTargetPos - vecStart).Length();
		if (flDistance < flClosestDistance)
		{
			flClosestDistance = flDistance;
			pTargetPawn = pPawn;
		}
	}

	bool bCanPenetrate = false;
	if (pTargetPawn)
	{

		CSkeletonInstance* pSkeletonCheck = pTargetPawn->m_pGameSceneNode()->GetSkeletonInstance();
		if (!pSkeletonCheck || !pSkeletonCheck->m_pBoneCache)
			return;

		Vector vHeadPos = pTargetPawn->m_pGameSceneNode()->GetBonePosition(6);
		Vector vChestPos = pTargetPawn->m_pGameSceneNode()->GetBonePosition(4);

		if (vHeadPos.IsValid() && !vHeadPos.IsZero())
		{
			penetration_data_t pen_data{};
			if (g_Autowall->m_Functions.m_FireBullet.FireBullet(vecStart, vHeadPos, &pen_data, pTargetPawn))
			{
				bCanPenetrate = true;
			}
		}

		if (!bCanPenetrate && vChestPos.IsValid() && !vChestPos.IsZero())
		{
			penetration_data_t pen_data{};
			if (g_Autowall->m_Functions.m_FireBullet.FireBullet(vecStart, vChestPos, &pen_data, pTargetPawn))
			{
				bCanPenetrate = true;
			}
		}
	}

	ImVec2 vScreenSize = ImGui::GetIO().DisplaySize;
	ImVec2 vScreenCenter = ImVec2(vScreenSize.x * 0.5f, vScreenSize.y * 0.5f);

	Color colCircle = bCanPenetrate ? Color(0, 255, 0, 255) : Color(255, 0, 0, 255);

	Draw::AddCircle(vScreenCenter, 8.0f, colCircle, 32, DRAW_CIRCLE_FILLED);
}

void CMisc::LSDMode() {
	if (!Config::b(g_Variables.m_WorldVisuals.m_bLSDMode))
		return;

	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn)
		return;

	ImVec2 vScreenSize = ImGui::GetIO().DisplaySize;
	ImVec2 vScreenCenter = ImVec2(vScreenSize.x * 0.5f, vScreenSize.y * 0.5f);

	static float flTime = 0.f;
	if (Interfaces::m_pGlobalVariables)
		flTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;

	int nLinesH = 150;
	int nLinesV = 120;
	float flLineThickness = 3.5f;

		for (int y = 0; y < nLinesH; y++)
		{
			float flYBase = (float)y / (float)nLinesH * vScreenSize.y;

			for (int x = 0; x < 250; x++)
			{
				float flX1 = (float)x / 250.0f * vScreenSize.x;
				float flX2 = (float)(x + 1) / 250.0f * vScreenSize.x;

				float flWave1 = sinf(flTime * 2.5f + flX1 * 0.03f + y * 0.15f) * 25.0f;
				float flWave2 = sinf(flTime * 2.5f + flX2 * 0.03f + y * 0.15f) * 25.0f;

				float flY1 = flYBase + flWave1;
				float flY2 = flYBase + flWave2;

				float flDist = sqrtf((flX1 - vScreenCenter.x) * (flX1 - vScreenCenter.x) + (flY1 - vScreenCenter.y) * (flY1 - vScreenCenter.y));
				int nHue = (int)(flTime * 80.0f + flDist * 0.08f + y * 4.5f) % 360;

				float flR = sinf(nHue * M_PI / 180.0f) * 0.5f + 0.5f;
				float flG = sinf((nHue + 120) * M_PI / 180.0f) * 0.5f + 0.5f;
				float flB = sinf((nHue + 240) * M_PI / 180.0f) * 0.5f + 0.5f;

				Color colLine = Color((int)(flR * 255), (int)(flG * 255), (int)(flB * 255), 160);
				Draw::AddLine(ImVec2(flX1, flY1), ImVec2(flX2, flY2), colLine, flLineThickness);
			}
		}

		for (int x = 0; x < nLinesV; x++)
		{
			float flXBase = (float)x / (float)nLinesV * vScreenSize.x;

			for (int y = 0; y < 250; y++)
			{
				float flY1 = (float)y / 250.0f * vScreenSize.y;
				float flY2 = (float)(y + 1) / 250.0f * vScreenSize.y;

				float flWave1 = cosf(flTime * 2.5f + flY1 * 0.03f + x * 0.15f) * 25.0f;
				float flWave2 = cosf(flTime * 2.5f + flY2 * 0.03f + x * 0.15f) * 25.0f;

				float flX1 = flXBase + flWave1;
				float flX2 = flXBase + flWave2;

				float flDist = sqrtf((flX1 - vScreenCenter.x) * (flX1 - vScreenCenter.x) + (flY1 - vScreenCenter.y) * (flY1 - vScreenCenter.y));
				int nHue = (int)(flTime * 80.0f + flDist * 0.08f + x * 4.5f + 180) % 360;

				float flR = sinf(nHue * M_PI / 180.0f) * 0.5f + 0.5f;
				float flG = sinf((nHue + 120) * M_PI / 180.0f) * 0.5f + 0.5f;
				float flB = sinf((nHue + 240) * M_PI / 180.0f) * 0.5f + 0.5f;

				Color colLine = Color((int)(flR * 255), (int)(flG * 255), (int)(flB * 255), 160);
				Draw::AddLine(ImVec2(flX1, flY1), ImVec2(flX2, flY2), colLine, flLineThickness);
			}
		}
}