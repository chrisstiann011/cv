#include "../../Precompiled.h"

std::vector<EHitBoxes> CLegitbot::LegitbotHB_To_HB(ELegitbotHitboxes nLegitbotHB)
{
	std::vector<EHitBoxes> vecToReturn;

	switch (nLegitbotHB)
	{
	case LEGITHITBOX_HEAD:
		vecToReturn.push_back(EHitBoxes::HITBOX_HEAD);
		break;
	case LEGITHITBOX_CHEST:
		vecToReturn.push_back(EHitBoxes::HITBOX_CHEST);
		break;
	case LEGITHITBOX_STOMACH:
		vecToReturn.push_back(EHitBoxes::HITBOX_STOMACH);
		break;
	case LEGITHITBOX_PELVIS:
		vecToReturn.push_back(EHitBoxes::HITBOX_PELVIS);
		break;
	case LEGITHITBOX_ARMS:
		vecToReturn.push_back(EHitBoxes::HITBOX_LEFT_FOREARM);
		vecToReturn.push_back(EHitBoxes::HITBOX_RIGHT_FOREARM);
		vecToReturn.push_back(EHitBoxes::HITBOX_LEFT_UPPER_ARM);
		vecToReturn.push_back(EHitBoxes::HITBOX_RIGHT_UPPER_ARM);
		break;
	case LEGITHITBOX_LEGS:
		vecToReturn.push_back(EHitBoxes::HITBOX_LEFT_CALF);
		vecToReturn.push_back(EHitBoxes::HITBOX_RIGHT_CALF);
		vecToReturn.push_back(EHitBoxes::HITBOX_LEFT_THIGH);
		vecToReturn.push_back(EHitBoxes::HITBOX_RIGHT_THIGH);
		break;
	case LEGITHITBOX_FEET:
		vecToReturn.push_back(EHitBoxes::HITBOX_LEFT_FOOT);
		vecToReturn.push_back(EHitBoxes::HITBOX_RIGHT_FOOT);
		break;
	default:
		break;
	}

	return vecToReturn;
}

void CLegitbot::AutoRevolver() {
	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pCmd)
		return;

	if (!Config::b(g_Variables.m_Legitbot.m_bAutoRevolver))
		return;

	CPlayer_WeaponServices* pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return;

	CCSPlayer_WeaponServices* pPlayerWeaponServices = static_cast<CCSPlayer_WeaponServices*>(Globals::m_pLocalPlayerPawn->m_pWeaponServices());
	if (!pPlayerWeaponServices)
		return;

	C_CSWeaponBase* m_pWeapon = pWeaponServices->m_hActiveWeapon().Get();
	if (!m_pWeapon)
		return;

	C_EconItemView* item = m_pWeapon->m_AttributeManager()->m_Item();
	if (!item)
		return;

	if (item->m_iItemDefinitionIndex() != WEAPON_R8_REVOLVER)
		return;

	auto time = TICKS_TO_TIME(Globals::m_pLocalPlayerController->m_nTickBase() + 1);

	if (time >= pPlayerWeaponServices->m_flNextAttack()) {
		auto secondTick = time >= TICKS_TO_TIME(m_pWeapon->m_nNextPrimaryAttackTick()) && time > TICKS_TO_TIME(m_pWeapon->m_nPostponeFireReadyTicks());

		if (secondTick && time < TICKS_TO_TIME(m_pWeapon->m_nNextSecondaryAttackTick()))
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_ATTACK2;
		else if (!secondTick)
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_ATTACK;
	}

	if (!m_pWeapon->m_iClip1() && m_pWeapon->m_pReserveAmmo() > 0) {
		Globals::m_pCmd->m_nButtons.m_nValue &= ~IN_ATTACK;
		Globals::m_pCmd->m_nButtons.m_nValue |= IN_RELOAD;
	}
}

void CLegitbot::CreateMove()
{
	AutoRevolver();

	if (Config::b(g_Variables.m_Legitbot.m_bSlowWalk) && Input::HandleInput(Config::kb(g_Variables.m_Legitbot.m_kbSlowWalk)))
	{
		int iSpeed = Config::i(g_Variables.m_Legitbot.m_iSlowWalkSpeed);

		float flModifier = 1.0f - (static_cast<float>(iSpeed) / 100.0f);

		if (flModifier < 0.0f) flModifier = 0.0f;

		if (Globals::m_pCmd)
		{

			if (Globals::m_pBaseCmd)
			{
				float flForward = Globals::m_pBaseCmd->forwardmove();
				float flSide = Globals::m_pBaseCmd->leftmove();

				Globals::m_pBaseCmd->set_forwardmove(flForward * flModifier);
				Globals::m_pBaseCmd->set_leftmove(flSide * flModifier);
			}
		}
	}

	bool bWeaponOnCooldown = LocalPlayerData::m_pWeapon && LocalPlayerData::m_pWeapon->m_nNextPrimaryAttackTick() > Interfaces::m_pGlobalVariables->m_nTickCount;
	bool bIsShooting = Globals::m_pCmd && (Globals::m_pCmd->m_nButtons.m_nValue & IN_ATTACK);

	if (bWeaponOnCooldown || bIsShooting)
	{
		this->m_Data.m_bWants2DMoveHalt = false;
		this->m_Data.m_vbAutostop = std::vector<bool>(EAutostop::AUTOSTOP_MAX, false);
	}

	if (!Config::b(g_Variables.m_Legitbot.m_bEnabled))
		return;
	if (!Input::HandleInput(Config::kb(g_Variables.m_Legitbot.m_bAimbot)))
		return;
	if (LocalPlayerData::m_pWeapon->m_iClip1() <= 0)
		return;

	if (LocalPlayerData::m_pWeapon->m_nNextPrimaryAttackTick() > Interfaces::m_pGlobalVariables->m_nTickCount)
		return;

	if (Config::vb(g_Variables.m_Legitbot.m_vbConditions).at(ELegitbotConditions::CONDITION_NO_FLASH) &&
		Globals::m_pLocalPlayerPawn->m_flFlashAlpha() > 0.f
		)
		return;

	EMenuWeaponType eWeaponType = GetWeaponMenuType((EItemDefinitionIndexes)LocalPlayerData::m_nWeaponDefinitionIndex);
	if (eWeaponType == EMenuWeaponType::NONE)
		return;

	this->m_Data.m_bWants2DMoveHalt = false;
	this->m_Data.m_vbAutostop = std::vector<bool>(EAutostop::AUTOSTOP_MAX, false);

	int iFovValue = 10;
	int iSmoothness = 5;
	std::vector<bool> vbHitboxes(ELegitbotHitboxes::LEGITHITBOX_MAX, false);
	bool bAutostop = false;
	std::vector<bool> vbAutostop(EAutostop::AUTOSTOP_MAX, false);

	switch (eWeaponType)
	{
	case EMenuWeaponType::LIGHT_PISTOL:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_LIGHT_PISTOL_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbAutostop);
		break;
	case EMenuWeaponType::DEAGLE:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_DEAGLE_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_DEAGLE_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_DEAGLE_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_DEAGLE_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_DEAGLE_vbAutostop);
		break;
	case EMenuWeaponType::REVOLVER:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_REVOLVER_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_REVOLVER_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_REVOLVER_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_REVOLVER_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_REVOLVER_vbAutostop);
		break;
	case EMenuWeaponType::SMG:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_SMG_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_SMG_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_SMG_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_SMG_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_SMG_vbAutostop);
		break;
	case EMenuWeaponType::LMG:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_LMG_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_LMG_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_LMG_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_LMG_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_LMG_vbAutostop);
		break;
	case EMenuWeaponType::AR:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_AR_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_AR_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_AR_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_AR_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_AR_vbAutostop);
		break;
	case EMenuWeaponType::SHOTGUN:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_SHOTGUN_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_SHOTGUN_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_SHOTGUN_vbAutostop);
		break;
	case EMenuWeaponType::SCOUT:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_SCOUT_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_SCOUT_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_SCOUT_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_SCOUT_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_SCOUT_vbAutostop);
		break;
	case EMenuWeaponType::AUTOSNIPER:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_AUTOSNIPER_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_AUTOSNIPER_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_AUTOSNIPER_vbAutostop);
		break;
		case EMenuWeaponType::AWP:
		iFovValue = Config::i(g_Variables.m_Legitbot.m_AWP_iFov);
		iSmoothness = Config::i(g_Variables.m_Legitbot.m_AWP_iSmoothness);
		vbHitboxes = Config::vb(g_Variables.m_Legitbot.m_AWP_vbHitboxes);
		bAutostop = Config::b(g_Variables.m_Legitbot.m_AWP_bAutostop);
		vbAutostop = Config::vb(g_Variables.m_Legitbot.m_AWP_vbAutostop);
		break;
	default:
		return;
	}

	this->m_Data.m_vbAutostop = vbAutostop;

	if (iFovValue == -1)
	{

		const float flMinFov = 10.0f;
		const float flMaxFov = 100.0f;
		const float flMaxSpeed = 250.0f;

		float flSpeed = 0.0f;
		if (Globals::m_pLocalPlayerPawn)
		{
			Vector vecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
			flSpeed = vecVelocity.Length2D();
		}

		float flSpeedRatio = std::clamp(flSpeed / flMaxSpeed, 0.0f, 1.0f);
		float flCalculatedFov = flMinFov + (flMaxFov - flMinFov) * flSpeedRatio;
		iFovValue = static_cast<int>(flCalculatedFov);
	}

	float flFovDiameter = static_cast<float>(iFovValue) / 4.f;

	LegitbotPlayer_t objBestPlayer = { nullptr, nullptr, { 0, 0, 0 }, 0, INT_MAX };

	for (auto& objPlayer : this->GetPlayers())
	{
		if (!objPlayer.m_pPawn || !objPlayer.m_pPawn->m_pGameSceneNode())
			continue;

		CGameSceneNode* pGameSceneNode = objPlayer.m_pPawn->m_pGameSceneNode();
		CSkeletonInstance* pSkeleton = pGameSceneNode->GetSkeletonInstance();
		if (!pSkeleton)
			continue;

		CModel* pModel = pSkeleton->m_modelState().m_modelHandle;
		if (!pModel)
			continue;

		std::vector<CHitBox*> vecHitboxes;

		const int nHitboxCount = pModel->GetHitBoxCount();
		if (nHitboxCount <= 0 || nHitboxCount > EHitBoxes::HITBOX_MAX)
			continue;

		for (int i = 0; i < ELegitbotHitboxes::LEGITHITBOX_MAX; i++)
		{
			if (!vbHitboxes.at(i))
				continue;

			auto vecSelectedHitboxes = LegitbotHB_To_HB((ELegitbotHitboxes)i);
			for (auto& nHitboxIndex : vecSelectedHitboxes)
			{

				if (nHitboxIndex < 0 || nHitboxIndex >= nHitboxCount)
					continue;

				CHitBox* pHitBox = pModel->GetHitBox(nHitboxIndex);
				if (pHitBox)
					vecHitboxes.push_back(pHitBox);
			}
		}

		if (vecHitboxes.empty())
			continue;

		GameTrace_t trace = GameTrace_t();
		TraceFilter_t filter = TraceFilter_t(0x1C3003, Globals::m_pLocalPlayerPawn, nullptr, 4);
		Ray_t ray = Ray_t();

		QAngle aBestHitboxAngle = { };
		Vector vBestHitboxPosition = { };
		float flBestHitboxDistance = FLT_MAX;

		for (auto& pHitbox : vecHitboxes)
		{
			if (!pHitbox)
				continue;

			uintptr_t ptrValue = reinterpret_cast<uintptr_t>(pHitbox);
			if (ptrValue < 0x10000 || ptrValue > 0x7FFFFFFFFFFF)
				continue;

			int nHitboxIndex = static_cast<int>(pHitbox->m_nHitboxIndex);

			if (nHitboxIndex < 0 || nHitboxIndex > 18)
				continue;

			int nBoneIndex = HitboxToBoneIndex(nHitboxIndex);
			if (nBoneIndex < 0)
				continue;

			CSkeletonInstance* pSkeletonCheck = pGameSceneNode->GetSkeletonInstance();
			if (!pSkeletonCheck || !pSkeletonCheck->m_pBoneCache)
				continue;

			Vector vHitboxPos = pGameSceneNode->GetBonePosition(nBoneIndex);

			if (!vHitboxPos.IsValid() || vHitboxPos.IsZero())
				continue;

			QAngle aHitboxAngle = Math::CalcAngle(Globals::m_pLocalPlayerPawn->GetEyePosition(true), vHitboxPos);
			float flDistance = Math::GetFOV(Interfaces::m_pInput->GetViewAngles(), aHitboxAngle);

			if (!std::isfinite(flDistance))
				continue;

			if (flDistance < flBestHitboxDistance)
			{
				flBestHitboxDistance = flDistance;
				vBestHitboxPosition = vHitboxPos;
				aBestHitboxAngle = aHitboxAngle;
			}
		}

		if (vBestHitboxPosition.IsZero() || aBestHitboxAngle.IsZero())
			continue;

		Interfaces::m_pVPhys2World->TraceShape(&ray, Globals::m_pLocalPlayerPawn->GetEyePosition(), vBestHitboxPosition, &filter, &trace);
		if (trace.m_pHitEntity != objPlayer.m_pPawn)
			continue;

		if (Config::vb(g_Variables.m_Legitbot.m_vbConditions).at(ELegitbotConditions::CONDITION_VISIBLE) &&
			!trace.IsVisible()
			)
			continue;

		if (flBestHitboxDistance > flFovDiameter)
			continue;

		if (Config::vb(g_Variables.m_Legitbot.m_vbConditions).at(ELegitbotConditions::CONDITION_NO_SMOKE) &&
			Utilities::LineGoesThroughSmoke(Globals::m_pLocalPlayerPawn->GetEyePosition(), vBestHitboxPosition, 0.9f))
			continue;

		objPlayer.m_iFov_CenterToAimpos = flBestHitboxDistance;
		objPlayer.m_aShootAngle = aBestHitboxAngle;

		if (flBestHitboxDistance < objBestPlayer.m_iFov_CenterToAimpos)
		{
			objBestPlayer = objPlayer;
		}
	}

	this->ScreenSpaceAim(flFovDiameter, iSmoothness);

	if (!objBestPlayer.m_pController ||
		!objBestPlayer.m_pPawn ||
		objBestPlayer.m_aShootAngle.IsZero())
	{

		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (bAutostop && LocalPlayerData::m_pWeapon)
	{
		float flCurrentSpeed = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D();
		float flMaxSpeed = LocalPlayerData::m_pWeapon->GetMaxSpeed();

		float flMinSpeedThreshold = flMaxSpeed * 0.30f;

		float flStopThreshold = flMaxSpeed * 0.15f;

		this->m_Data.m_bWants2DMoveHalt = (flCurrentSpeed > flMinSpeedThreshold) && (flCurrentSpeed > flStopThreshold);
	}
	else
	{
		this->m_Data.m_bWants2DMoveHalt = false;
	}

	QAngle aCurrentAngle = Interfaces::m_pInput->GetViewAngles();

	QAngle aDelta = objBestPlayer.m_aShootAngle - aCurrentAngle;

	aDelta.Normalize();

	QAngle aSmoothedAngle = aCurrentAngle + (aDelta / static_cast<float>(iSmoothness));

	aSmoothedAngle.Normalize();

	Interfaces::m_pInput->SetViewAngle(aSmoothedAngle);
}

void CLegitbot::ScreenSpaceAim(float flFovDiameter, int iSmoothness)
{

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	if (Gui::m_bOpen)
		return;

	const ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
	const ImVec2 vecScreenCenter = ImVec2(vecDisplaySize.x * 0.5f, vecDisplaySize.y * 0.5f);

	ImVec2 vecClosestDelta = ImVec2(0.0f, 0.0f);
	float flClosestDistance = FLT_MAX;
	bool bFoundTarget = false;

	for (auto& objEntity : g_Entities->m_vecPlayersOnly)
	{
		CCSPlayerController* pController = (CCSPlayerController*)objEntity.m_pEntity;
		if (!pController)
			continue;

		if (pController == Globals::m_pLocalPlayerController)
			continue;

		C_CSPlayerPawn* pTargetPawn = pController->m_hPlayerPawn().Get();
		if (!pTargetPawn)
			continue;

		if (pTargetPawn == Globals::m_pLocalPlayerPawn)
			continue;

		if (!pController->m_bPawnIsAlive())
			continue;

		if (!Globals::m_pLocalPlayerPawn->IsEnemy(pTargetPawn))
			continue;

		if (!pTargetPawn->m_pGameSceneNode())
			continue;

		CSkeletonInstance* pSkeleton = pTargetPawn->m_pGameSceneNode()->GetSkeletonInstance();
		if (!pSkeleton)
			continue;

		CSkeletonInstance* pSkeletonCheck = pTargetPawn->m_pGameSceneNode()->GetSkeletonInstance();
		if (!pSkeletonCheck || !pSkeletonCheck->m_pBoneCache)
			continue;

		const int nHeadBoneIndex = 6;
		if (nHeadBoneIndex < 0 || nHeadBoneIndex >= 128)
			continue;

		Vector vecHeadPos = pTargetPawn->m_pGameSceneNode()->GetBonePosition(nHeadBoneIndex);
		if (vecHeadPos.IsZero() || !vecHeadPos.IsValid())
			continue;

		ImVec2 vecScreenPos;
		if (!Draw::WorldToScreen(vecHeadPos, vecScreenPos))
			continue;

		ImVec2 vecDelta = ImVec2(
			vecScreenPos.x - vecScreenCenter.x,
			vecScreenPos.y - vecScreenCenter.y
		);

		float flDistance = std::sqrtf(vecDelta.x * vecDelta.x + vecDelta.y * vecDelta.y);

		float flMaxScreenDistance = (vecDisplaySize.x * 0.5f) * (flFovDiameter / 90.0f);
		if (flDistance > flMaxScreenDistance)
			continue;

		if (flDistance < flClosestDistance)
		{
			flClosestDistance = flDistance;
			vecClosestDelta = vecDelta;
			bFoundTarget = true;
		}
	}

	if (bFoundTarget)
	{

		float flSmooth = static_cast<float>(iSmoothness);
		if (flSmooth < 1.0f)
			flSmooth = 1.0f;

		ImVec2 vecSmoothedDelta = ImVec2(
			vecClosestDelta.x / flSmooth,
			vecClosestDelta.y / flSmooth
		);

		mouse_event(MOUSEEVENTF_MOVE,
			static_cast<DWORD>(vecSmoothedDelta.x),
			static_cast<DWORD>(vecSmoothedDelta.y), 0, 0);
	}
}

std::vector<CLegitbot::LegitbotPlayer_t> CLegitbot::GetPlayers()
{
	std::vector<CLegitbot::LegitbotPlayer_t> vecOut;

	for (auto& objEntity : g_Entities->m_vecPlayersOnly)
	{

		CCSPlayerController* pController = (CCSPlayerController*)objEntity.m_pEntity;
		if (!pController)
			continue;

		C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn().Get();
		if (!pPawn)
			continue;

		if (!pController->m_bPawnIsAlive())
			continue;

		if (!Globals::m_pLocalPlayerPawn->IsEnemy(pPawn))
			continue;

		LegitbotPlayer_t objLegitbotPlayer = { };

		objLegitbotPlayer.m_pController = pController;
		objLegitbotPlayer.m_pPawn = pPawn;

		objLegitbotPlayer.m_iHealth = objLegitbotPlayer.m_pPawn->m_iHealth();

		vecOut.emplace_back(objLegitbotPlayer);
	}

	return vecOut;
}

void CLegitbot::Triggerbot()
{
	static int delay_start_tick = 0;
	static int autostop_start_tick = 0;
	static bool bWaiting = false;
	static bool bAutostopActive = false;

	if (!Config::b(g_Variables.m_Legitbot.m_bTriggerbotEnabled))
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (!Input::HandleInput(Config::kb(g_Variables.m_Legitbot.m_kbTriggerbot)))
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (!Globals::m_pLocalPlayerController ||
		!Globals::m_pLocalPlayerPawn ||
		!Globals::m_pLocalPlayerController->m_bPawnIsAlive() ||
		!LocalPlayerData::m_pWeapon)
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (LocalPlayerData::m_pWeapon->m_iClip1() <= 0 ||
		LocalPlayerData::m_pWeapon->m_nNextPrimaryAttackTick() > Interfaces::m_pGlobalVariables->m_nTickCount)
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (Config::b(g_Variables.m_Legitbot.m_bTriggerbotOnlyWhenScoped) &&
		!Globals::m_pLocalPlayerPawn->m_bIsScoped())
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	if (!Globals::m_pCmd)
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	EMenuWeaponType eWeaponType = GetWeaponMenuType((EItemDefinitionIndexes)LocalPlayerData::m_nWeaponDefinitionIndex);
	if (eWeaponType == EMenuWeaponType::NONE)
	{
		bWaiting = false;
		bAutostopActive = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		return;
	}

	int iTriggerbotDelay = 0;
	switch (eWeaponType)
	{
	case EMenuWeaponType::LIGHT_PISTOL:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iTriggerbotDelay);
		break;
	case EMenuWeaponType::DEAGLE:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_DEAGLE_iTriggerbotDelay);
		break;
	case EMenuWeaponType::REVOLVER:
		iTriggerbotDelay = 0;
		break;
	case EMenuWeaponType::SMG:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_SMG_iTriggerbotDelay);
		break;
	case EMenuWeaponType::LMG:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_LMG_iTriggerbotDelay);
		break;
	case EMenuWeaponType::AR:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_AR_iTriggerbotDelay);
		break;
	case EMenuWeaponType::SHOTGUN:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iTriggerbotDelay);
		break;
	case EMenuWeaponType::SCOUT:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_SCOUT_iTriggerbotDelay);
		break;
	case EMenuWeaponType::AUTOSNIPER:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iTriggerbotDelay);
		break;
	case EMenuWeaponType::AWP:
		iTriggerbotDelay = Config::i(g_Variables.m_Legitbot.m_AWP_iTriggerbotDelay);
		break;
	default:
		bWaiting = false;
		return;
	}

	float flDelaySeconds = (float)iTriggerbotDelay / 1000.f;

	const Vector vecStart = Globals::m_pLocalPlayerPawn->GetEyePosition(true);
	QAngle angView = Interfaces::m_pInput->GetViewAngles();

	Vector vForward, vRight, vUp;
	Math::AngleVectors(angView, vForward, vRight, vUp);

	const float flMaxDistance = MAX_DISTANCE;
	const Vector vecEnd = vecStart + vForward * flMaxDistance;

	Ray_t ray{};
	GameTrace_t trace{};
	TraceFilter_t filter(0x1C3003, Globals::m_pLocalPlayerPawn, nullptr, 4);

	Interfaces::m_pVPhys2World->TraceShape(&ray, vecStart, vecEnd, &filter, &trace);

	C_CSPlayerPawn* pTargetPawn = nullptr;
	bool bUsingAutowall = Config::b(g_Variables.m_Legitbot.m_bTriggerbotAutowall) && Input::HandleInput(Config::kb(g_Variables.m_Legitbot.m_kbTriggerbotAutowall));

	if (bUsingAutowall)
	{

		float flClosestDistance = FLT_MAX;
		float flBestDotProduct = 0.0f;

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

			Vector vTargetCenter = pPawn->m_pGameSceneNode()->m_vecAbsOrigin();
			vTargetCenter.z += 40.0f;

			Vector vDirectionToTarget = (vTargetCenter - vecStart).Normalized();
			float flDotProduct = vForward.DotProduct(vDirectionToTarget);

			if (flDotProduct < 0.966f)
				continue;

			float flDistance = (vTargetCenter - vecStart).Length();

			if (flDotProduct > flBestDotProduct || (flDotProduct == flBestDotProduct && flDistance < flClosestDistance))
			{
				flClosestDistance = flDistance;
				flBestDotProduct = flDotProduct;
				pTargetPawn = pPawn;
			}
		}

		if (pTargetPawn)
		{

			const int arrBonesToCheck[] = {
				6,
				4, 5,
				2,
				1,
				7, 8,
				9, 10,
				11, 12,
				13, 14,
				15, 16,
				17, 18
			};
			const int nBoneCount = sizeof(arrBonesToCheck) / sizeof(arrBonesToCheck[0]);

			bool bCanHit = false;

			if (pTargetPawn->m_pGameSceneNode())
			{

				CSkeletonInstance* pSkeletonCheck = pTargetPawn->m_pGameSceneNode()->GetSkeletonInstance();
				if (pSkeletonCheck && pSkeletonCheck->m_pBoneCache)
				{

					for (int i = 0; i < nBoneCount && !bCanHit; i++)
					{
						int nBoneIndex = arrBonesToCheck[i];

						if (nBoneIndex < 0 || nBoneIndex >= 128)
							continue;

						Vector vBonePos = pTargetPawn->m_pGameSceneNode()->GetBonePosition(nBoneIndex);

						if (!vBonePos.IsValid() || vBonePos.IsZero())
							continue;

						penetration_data_t pen_data{};
						if (g_Autowall->m_Functions.m_FireBullet.FireBullet(vecStart, vBonePos, &pen_data, pTargetPawn))
						{
							bCanHit = true;
							break;
						}
					}
				}
			}

			if (!bCanHit)
				pTargetPawn = nullptr;
		}
	}
	else
	{

		if (trace.m_pHitEntity && trace.m_pHitEntity->IsPlayerPawn())
			pTargetPawn = static_cast<C_CSPlayerPawn*>(trace.m_pHitEntity);
	}

	if (!pTargetPawn)
	{
		bWaiting = false;

		if (bAutostopActive)
		{
			int currentTick = Interfaces::m_pGlobalVariables->m_nTickCount;
			int ticksElapsed = currentTick - autostop_start_tick;

			if (ticksElapsed < 13)
			{

				this->m_Data.m_bWants2DMoveHalt = true;
				return;
			}
			else
			{

				bAutostopActive = false;
				this->m_Data.m_bWants2DMoveHalt = false;
			}
		}
		else
		{
			this->m_Data.m_bWants2DMoveHalt = false;
		}
		return;
	}

	auto pTargetController = static_cast<CCSPlayerController*>(pTargetPawn->m_hController().Get());
	if (!pTargetController ||
		!pTargetController->m_bPawnIsAlive() ||
		!Globals::m_pLocalPlayerPawn->IsEnemy(pTargetPawn))
	{
		bWaiting = false;

		if (bAutostopActive)
		{
			int currentTick = Interfaces::m_pGlobalVariables->m_nTickCount;
			int ticksElapsed = currentTick - autostop_start_tick;

			if (ticksElapsed < 13)
			{

				this->m_Data.m_bWants2DMoveHalt = true;
				return;
			}
			else
			{

				bAutostopActive = false;
				this->m_Data.m_bWants2DMoveHalt = false;
			}
		}
		else
		{
			this->m_Data.m_bWants2DMoveHalt = false;
		}
		return;
	}

	if (LocalPlayerData::m_pWeapon)
	{
		float flCurrentSpeed = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D();
		float flMaxSpeed = LocalPlayerData::m_pWeapon->GetMaxSpeed();

		float flMinSpeedThreshold = flMaxSpeed * 0.30f;

		float flStopThreshold = flMaxSpeed * 0.15f;

		bool bShouldActivateAutostop = (flCurrentSpeed > flMinSpeedThreshold) && (flCurrentSpeed > flStopThreshold);

		if (bShouldActivateAutostop)
		{

			if (!bAutostopActive)
			{
				bAutostopActive = true;
				autostop_start_tick = Interfaces::m_pGlobalVariables->m_nTickCount;
			}
			this->m_Data.m_bWants2DMoveHalt = true;
		}
		else
		{

			this->m_Data.m_bWants2DMoveHalt = false;
		}
	}
	else
	{
		this->m_Data.m_bWants2DMoveHalt = false;
	}

	if (!bWaiting)
	{
		bWaiting = true;
		delay_start_tick = Interfaces::m_pGlobalVariables->m_nTickCount;

		if (flDelaySeconds > 0.f)
			return;
	}

	int currentTick = Interfaces::m_pGlobalVariables->m_nTickCount;
	int ticksElapsed = currentTick - delay_start_tick;
	float flElapsed = ticksElapsed * INTERVAL_PER_TICK;

	if (flElapsed < flDelaySeconds)
		return;

	bWaiting = false;

	auto pCmd = Globals::m_pCmd;

	if (!(Interfaces::m_pInput->nKeyboardPressed & IN_ATTACK))
		Interfaces::m_pInput->nKeyboardPressed |= IN_ATTACK;

	if (!(pCmd->m_nButtons.m_nValue & IN_ATTACK))
		pCmd->m_nButtons.m_nValue |= IN_ATTACK;

	pCmd->m_csgoUserCmd.mutable_base()->set_client_tick(Globals::m_pLocalPlayerController->m_nTickBase());

	int nEntryIdx = pCmd->m_csgoUserCmd.input_history_size() - 1;
	if (nEntryIdx >= 0)
	{
		pCmd->m_csgoUserCmd.set_attack1_start_history_index(nEntryIdx);
		pCmd->m_csgoUserCmd.set_attack3_start_history_index(nEntryIdx);
	}
}