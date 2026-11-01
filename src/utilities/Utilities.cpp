#include "../Precompiled.h"

void Utilities::CopyToClipboard(std::string strData)
{
	OpenClipboard(NULL);
	EmptyClipboard();
	HGLOBAL pMemory = GlobalAlloc(GMEM_MOVEABLE, strData.size());
	if (!pMemory)
	{
		CloseClipboard();
	}
	else
	{
		CRT::MemoryCopy(GlobalLock(pMemory), strData.c_str(), strData.size());
		GlobalUnlock(pMemory);
		SetClipboardData(CF_TEXT, pMemory);
		CloseClipboard();
		GlobalFree(pMemory);
	}
}

std::vector<Vector> Utilities::GiftWrappingAlgorithm(std::vector<Vector> vecVector)
{
	std::vector<Vector> vecHull;

	if (vecVector.size() < 3)
		return vecHull;

	CRT::Swap(vecVector[0], *min_element(vecVector.begin(), vecVector.end(), IsLeftOfPoint));

	do
	{
		vecHull.emplace_back(vecVector[0]);
		CRT::Swap(vecVector[0], *min_element(vecVector.begin() + 1, vecVector.end(), ccwSorter(vecVector[0])));
	} while (vecVector[0].x != vecHull[0].x && vecVector[0].y != vecHull[0].y);

	return vecHull;
}

bool Utilities::LineGoesThroughSmoke(const Vector& vecStart, const Vector& vecEnd, const float& flMaxDensity)
{
#ifdef CS_PARANOID
	CS_ASSERT(Functions::fnTraceSmokeDensity != nullptr);
#endif

	const float flDensity = ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnTraceSmokeDensity, vecStart, vecEnd, nullptr);
	return flMaxDensity >= flDensity;
}

void Utilities::SetLocalPlayerReady()
{
#ifdef CS_PARANOID
	CS_ASSERT(Functions::fnSetPlayerReady != nullptr);
#endif
	const char* nigga = "";
	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetPlayerReady, nullptr, nigga);
}

void Utilities::FlashWindow(HWND pWindow)
{
	FLASHWINFO fwInfo;
	fwInfo.cbSize = sizeof(FLASHWINFO);
	fwInfo.hwnd = pWindow;
	fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fwInfo.uCount = 0;
	fwInfo.dwTimeout = 0;
	FlashWindowEx(&fwInfo);
}

void* Utilities::LoadFileForMe(const std::string_view strPath, int& nFileSize)
{
#ifdef CS_PARANOID
	CS_ASSERT(Functions::fnLoadFileForMe != nullptr);
#endif

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnLoadFileForMe, strPath.data(), &nFileSize);
}

std::uintptr_t* Utilities::FindHudElement(const char* szName)
{

	using fnFindHudElement = std::uintptr_t* (__thiscall*)(const char*);
	static const fnFindHudElement oFindHudElement = reinterpret_cast<fnFindHudElement>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 8B D9 48 85 C0 74 ? 48 89 5C 24")));
	if (!oFindHudElement)
		return nullptr;

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oFindHudElement, szName);
}

void Utilities::HudChatPrint(const char* szString)
{
	std::uintptr_t* pHudChatDelegate = FindHudElement(X("HudChatDelegate"));
	if (!pHudChatDelegate)
		return;

	using fnHudChatPrintf = void* (__fastcall*)(std::uintptr_t*, int, const char*);
	static const fnHudChatPrintf oHudChatPrintf = reinterpret_cast<fnHudChatPrintf>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 49 8B 4E 20 BA ? ? ? ?")), 0x1, 0x0));

	ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oHudChatPrintf, pHudChatDelegate, -1, szString);
}

bool Utilities::ForceHoldKnife()
{
	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return false;

	CPlayer_WeaponServices* pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return false;

	C_BasePlayerWeapon* pActiveWeapon = Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	if (!pActiveWeapon)
		return false;

	CCSWeaponBaseVData* pWeaponData = pActiveWeapon->GetWeaponBaseVData();
	if (!pWeaponData)
		return false;

	if (pWeaponData->m_WeaponType() != WEAPONTYPE_KNIFE)
	{
		for (size_t uSize = 0; uSize < pWeaponServices->m_hMyWeapons().m_nSize; uSize++)
		{

			C_BasePlayerWeapon* pCurrentWeapon = pWeaponServices->m_hMyWeapons().m_pElements[uSize].Get();
			if (!pCurrentWeapon)
				continue;

			CCSWeaponBaseVData* pCurrentWeaponData = pCurrentWeapon->GetWeaponBaseVData();
			if (!pCurrentWeaponData)
				continue;

			if (pCurrentWeaponData->m_WeaponType() == WEAPONTYPE_KNIFE)
			{
				Globals::m_pBaseCmd->set_weaponselect(pCurrentWeapon->GetRefEHandle().GetEntryIndex());
				return false;
			}
		}
	}

	Globals::m_pBaseCmd->set_weaponselect(0);
	return true;
}

int Utilities::HitboxToHitgroup(EHitBoxes index)
{
	switch (index)
	{
	case EHitBoxes::HITBOX_HEAD:
		return (int)EHitgroups::HITGROUP_HEAD;

	case EHitBoxes::HITBOX_NECK:

		return (int)EHitgroups::HITGROUP_NECK;

	case EHitBoxes::HITBOX_PELVIS:

		return (int)EHitgroups::HITGROUP_STOMACH;

	case EHitBoxes::HITBOX_THORAX:
	case EHitBoxes::HITBOX_CHEST:
	case EHitBoxes::HITBOX_UPPER_CHEST:
		return (int)EHitgroups::HITGROUP_CHEST;

	case EHitBoxes::HITBOX_LEFT_HAND:
	case EHitBoxes::HITBOX_LEFT_UPPER_ARM:
	case EHitBoxes::HITBOX_LEFT_FOREARM:
		return (int)EHitgroups::HITGROUP_LEFTARM;

	case EHitBoxes::HITBOX_RIGHT_HAND:
	case EHitBoxes::HITBOX_RIGHT_UPPER_ARM:
	case EHitBoxes::HITBOX_RIGHT_FOREARM:
		return (int)EHitgroups::HITGROUP_RIGHTARM;

	case EHitBoxes::HITBOX_LEFT_FOOT:
	case EHitBoxes::HITBOX_LEFT_THIGH:
	case EHitBoxes::HITBOX_LEFT_CALF:
		return (int)EHitgroups::HITGROUP_LEFTLEG;

	case EHitBoxes::HITBOX_RIGHT_FOOT:
	case EHitBoxes::HITBOX_RIGHT_THIGH:
	case EHitBoxes::HITBOX_RIGHT_CALF:
		return (int)EHitgroups::HITGROUP_RIGHTLEG;

	default:
		return (int)EHitgroups::HITGROUP_GENERIC;
	}
}

std::string Utilities::GetHitgroupName(int nHitgroupIndex)
{
	switch (nHitgroupIndex)
	{
	case HITGROUP_INVALID:
		return "invalid hitgroup";
	case HITGROUP_GENERIC:
		return "generic";
	case HITGROUP_HEAD:
		return "head";
	case HITGROUP_CHEST:
		return "chest";
	case HITGROUP_STOMACH:
		return "stomach";
	case HITGROUP_LEFTARM:
		return "left arm";
	case HITGROUP_RIGHTARM:
		return "right arm";
	case HITGROUP_LEFTLEG:
		return "left leg";
	case HITGROUP_RIGHTLEG:
		return "right leg";
	case HITGROUP_NECK:
		return "neck";
	}
}

void Utilities::PlayVSound(const char* szSoundPath, float flVolume)
{
	char szVolumeOut[20];
	stbsp_sprintf(szVolumeOut, X("snd_toolvolume %.2f"), flVolume);

	Interfaces::m_pEngine->ExecuteClientCmdUnrestricted(szVolumeOut);

	using fnPlayVSound = void(__fastcall*)(const char*);
	static const fnPlayVSound oPlayVSound = reinterpret_cast<fnPlayVSound>(Memory::FindPattern(SOUNDSYSTEM_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 FF")));
	if ( oPlayVSound )
		oPlayVSound( reinterpret_cast<const char*>( szSoundPath ) );
}