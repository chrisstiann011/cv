#include "../../Precompiled.h"

namespace
{

	constexpr float ZEUS_RANGE = 125.0f;
	constexpr float ZEUS_TARGET_LIMIT = 125.0f;
}

bool CZeusBot::CanAttack()
{
	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController || !Globals::m_pCmd)
		return false;

	auto pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return false;

	auto pWeapon = pWeaponServices->m_hActiveWeapon().Get();
	if (!pWeapon)
		return false;

	auto pWeaponData = pWeapon->GetWeaponBaseVData();
	if (!pWeaponData || pWeaponData->m_WeaponType() != WEAPONTYPE_TASER)
		return false;

	if (Globals::m_pLocalPlayerPawn->m_iHealth() <= 0)
		return false;

	if (!Interfaces::m_pGlobalVariables || pWeapon->m_nNextPrimaryAttackTick() > Interfaces::m_pGlobalVariables->m_nTickCount)
		return false;

	return true;
}

bool CZeusBot::IsEnemyInRange(C_CSPlayerPawn* pEnemy, float flRange)
{
	if (!pEnemy || !Globals::m_pLocalPlayerPawn)
		return false;

	Vector vecLocalPos = Globals::m_pLocalPlayerPawn->GetEyePosition();
	Vector vecEnemyPos = pEnemy->GetEyePosition();

	return (vecEnemyPos - vecLocalPos).Length() <= flRange;
}

C_CSPlayerPawn* CZeusBot::GetBestTarget()
{
	if (!Globals::m_pLocalPlayerPawn || !Interfaces::m_pGameResourceService || !Interfaces::m_pGameResourceService->pGameEntitySystem)
		return nullptr;

	C_CSPlayerPawn* pBestTarget = nullptr;
	float flBestDistance = FLT_MAX;
	const int iLocalTeam = Globals::m_pLocalPlayerPawn->m_iTeamNum();
	Vector vecLocalPos = Globals::m_pLocalPlayerPawn->GetEyePosition();

	for (int i = 1; i <= 64; ++i)
	{
		CCSPlayerController* pController = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<CCSPlayerController>(i);
		if (!pController || !pController->m_bPawnIsAlive())
			continue;

		C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn().Get();
		if (!pPawn)
			continue;

		if (pPawn->m_iTeamNum() == iLocalTeam)
			continue;

		if (!Globals::m_pLocalPlayerPawn->IsEnemy(pPawn))
			continue;

		auto pSceneNode = pPawn->m_pGameSceneNode();
		if (!pSceneNode || pSceneNode->m_bDormant())
			continue;

		Vector vecEnemyPos = pPawn->GetEyePosition();
		float flDistance = (vecEnemyPos - vecLocalPos).Length();

		if (flDistance > ZEUS_TARGET_LIMIT)
			continue;

		if (flDistance < flBestDistance)
		{
			flBestDistance = flDistance;
			pBestTarget = pPawn;
		}
	}

	return pBestTarget;
}

void CZeusBot::Run()
{
	if (!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled))
		return;
	if (!Config::b(g_Variables.m_Ragebot.m_bTaserBot))
		return;

	if (!CanAttack())
	{
		m_pLastTarget = nullptr;
		return;
	}

	C_CSPlayerPawn* pTarget = GetBestTarget();
	if (!pTarget)
	{
		m_pLastTarget = nullptr;
		return;
	}

	if (!IsEnemyInRange(pTarget, ZEUS_RANGE))
		return;

	Vector vecLocalPos = Globals::m_pLocalPlayerPawn->GetEyePosition();
	Vector vecTargetPos = pTarget->GetEyePosition();

	QAngle angToTarget;
	Math::VectorAngles(vecTargetPos - vecLocalPos, angToTarget);

	auto SetAttackHistory = [&]()
		{
			const int nEntryIdx = Globals::m_pCmd->m_csgoUserCmd.input_history_size() - 1;
			CSGOInputHistoryEntryPB* pEntry = (nEntryIdx >= 0) ? Globals::m_pCmd->m_csgoUserCmd.mutable_input_history(nEntryIdx) : nullptr;

			if (pEntry)
			{
				CMsgQAngle* pAngles = pEntry->mutable_view_angles();
				if (pAngles)
				{
					pAngles->set_x(angToTarget.x);
					pAngles->set_y(angToTarget.y);
					pAngles->set_z(angToTarget.z);
				}

				Globals::m_pCmd->m_csgoUserCmd.set_attack1_start_history_index(nEntryIdx);
				return;
			}

			Vector angVec(angToTarget.x, angToTarget.y, angToTarget.z);
			Globals::SetBaseViewAngles(angVec);

			Globals::m_pCmd->m_csgoUserCmd.set_attack1_start_history_index(-1);
		};

	SetAttackHistory();

	Globals::m_pCmd->m_nButtons.m_nValue |= IN_ATTACK;
	Globals::m_pCmd->m_nButtons.m_nValueChanged |= IN_ATTACK;

	m_pLastTarget = pTarget;
}

