#include "Precompiled.h"

constexpr int movementKeys =
IN_TURNLEFT |
IN_FORWARD |
IN_BACK |
IN_TURNRIGHT |
IN_MOVELEFT |
IN_MOVERIGHT |
IN_JUMP;

bool LocalPlayerData::Update()
{
	Globals::m_pLocalPlayerController = CCSPlayerController::GetLocalPlayerController();
	if (!Globals::m_pLocalPlayerController)
		return false;

	CUserCmd* pCmd = Globals::m_pCmd = Globals::m_pLocalPlayerController->GetUserCmd(Globals::m_pLocalPlayerController);
	if (!pCmd)
		return false;

	Globals::m_pBaseCmd = pCmd->m_csgoUserCmd.mutable_base();
	if (!Globals::m_pBaseCmd)
		return false;

	Globals::m_pLocalPlayerPawn = Globals::m_pLocalPlayerController->m_hPlayerPawn().Get();
	if (!Globals::m_pLocalPlayerPawn)
		return false;

	CClientNetworkData objClientNetworkData{};
	Interfaces::m_pEngine->GetClientNetworkData(&objClientNetworkData);
	Globals::m_NetClientInfo = objClientNetworkData;
	if (!Globals::m_NetClientInfo.m_pLocalData) {
		return false;
	}

	static int nLastEyeposUpdate = 0;
	if (nLastEyeposUpdate != Globals::m_pBaseCmd->client_tick()) {
		LocalPlayerData::m_vecEyePosition = objClientNetworkData.m_pLocalData->m_vecShootPostion;
		nLastEyeposUpdate = Globals::m_pBaseCmd->client_tick();
	}

	LocalPlayerData::m_vecOrigin = Globals::m_pLocalPlayerPawn->m_pGameSceneNode()->m_vecOrigin().m_vecValue;
	LocalPlayerData::m_vecAbsOrigin = Globals::m_pLocalPlayerPawn->m_pGameSceneNode()->m_vecAbsOrigin();
	LocalPlayerData::m_vecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
	LocalPlayerData::m_vecAbsVelocity = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity();

	if (LocalPlayerData::m_vecAbsOrigin.IsZero() || LocalPlayerData::m_vecOrigin.IsZero( ))
		return false;

	LocalPlayerData::m_nTickBase = Globals::m_pLocalPlayerController->m_nTickBase();
	if (LocalPlayerData::m_nTickBase <= 0)
		return false;

	LocalPlayerData::m_flSurfaceFriction = Globals::m_pLocalPlayerPawn->m_pMovementServices()->m_flSurfaceFriction();

	Globals::m_bIsPressingMovementKey = (pCmd->m_nButtons.m_nValue & movementKeys) != 0;

	CPlayer_WeaponServices* pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return false;

	m_pWeapon = pWeaponServices->m_hActiveWeapon().Get();
	if (!m_pWeapon)
		return false;

	m_pWeaponBaseVData = m_pWeapon->GetWeaponBaseVData();
	if (!m_pWeaponBaseVData)
		return false;
	LocalPlayerData::m_flMaxPlayerSpeed = Globals::m_pLocalPlayerPawn->m_bIsScoped() ? LocalPlayerData::m_pWeaponBaseVData->m_flMaxSpeed()[1] : LocalPlayerData::m_pWeaponBaseVData->m_flMaxSpeed()[0];

	C_AttributeContainer* pAttribute = m_pWeapon->m_AttributeManager();
	if (!pAttribute)
		return false;

	C_EconItemView* pEconItem = pAttribute->m_Item();
	if (!pEconItem)
		return false;

	int curDefinition = pEconItem->m_iItemDefinitionIndex();
	m_bDidWeaponChange = (m_nWeaponDefinitionIndex != curDefinition);
	m_nWeaponDefinitionIndex = curDefinition;
	if (m_bDidWeaponChange) {
		m_bRageNeedsConfigUpdate = true;
	}

	m_pWeapon->UpdateAccuracyPenalty();
	return true;
}

void LocalPlayerData::Reset()
{
	m_vecAbsOrigin = Vector(0, 0, 0);
	m_nTickBase = 0;
	m_nWeaponDefinitionIndex = 0U;
	m_pWeapon = nullptr;
	m_pWeaponBaseVData = nullptr;
}

bool Globals::DuplicatePawn(C_CSPlayerPawn* pPawn, int nIndex)
{

	return true;
}
