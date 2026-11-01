#include "../Precompiled.h"

bool Functions::Setup()
{

	fnSetViewAngle = reinterpret_cast<decltype(fnSetViewAngle)>(Memory::FindPattern(CLIENT_DLL, X("85 D2 75 ? 48 63 81")));
	fnGetViewAngles = reinterpret_cast<decltype(fnGetViewAngles)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B C1 85 D2 74 ? 48 8D 05")));
	fnCGCClientSharedObjectCache_FindTypeCache = reinterpret_cast<decltype(fnCGCClientSharedObjectCache_FindTypeCache)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B 49 18 44 8B D2 4C 63 41 10 4F 8D 1C C1 49 8B C3")));

	fnGetBaseEntity = reinterpret_cast<decltype(fnGetBaseEntity)>(Memory::FindPattern(CLIENT_DLL, X("4C 8D 49 ? 81 FA")));

	fnCalculateWorldSpaceBones = reinterpret_cast<decltype(fnCalculateWorldSpaceBones)>(Memory::FindPattern(CLIENT_DLL, X("48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8D 6C 24 ? 48 8B 81")));

	fnTraceShape = reinterpret_cast<decltype(fnTraceShape)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 4C 24 ? 55 57")));

	fnClipRayToEntity = reinterpret_cast<decltype(fnClipRayToEntity)>(Memory::FindPattern(CLIENT_DLL, X("48 8B C4 48 89 58 ? 55 56 57 41 54 41 56 48 8D 68 ? 48 81 EC ? ? ? ? 48 8B 5D")));
	fnGetSurfaceData = reinterpret_cast<decltype(fnGetSurfaceData)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 80 78 18 00")), 0x1, 0x0));

	fnFindKeyVar = reinterpret_cast<decltype(fnFindKeyVar)>(Memory::FindPattern(PARTICLES_DLL, X("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 33 C0 8B DA")));

	fnLoadKeyValues = reinterpret_cast<decltype(fnLoadKeyValues)>(Memory::GetExportAddress(Memory::GetModuleBaseHandle(TIER0_DLL), X("?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEBDAEBUKV3ID_t@@2I@Z")));

	fnSetTypeKV3 = reinterpret_cast<decltype(fnSetTypeKV3)>(Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("40 53 48 83 EC ? 80 FA")));

	fnCacheParticleEffect = reinterpret_cast<decltype(fnCacheParticleEffect)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B DC 53 48 81 EC ? ? ? ? F2 0F 10 05")));
	fnCreateParticleEffect = reinterpret_cast<decltype(fnCreateParticleEffect)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? F3 0F 10 1D ? ? ? ? 41 8B F8 8B DA 4C 8D 05")));
	fnUnknownParticleFunction = reinterpret_cast<decltype(fnUnknownParticleFunction)>(Memory::FindPattern(CLIENT_DLL, X("48 89 74 24 ? 57 48 83 EC ? 4C 8B D9 49 8B F9 33 C9 41 8B F0 83 FA ? 0F 84")));

	fnSetPlayerReady = reinterpret_cast<decltype(fnSetPlayerReady)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC 20 48 8B DA 48 8D 15 ? ? ? ? 48 8B CB FF 15 ? ? ? ? 85 C0 75 14 BA")));

	fnCreateMaterial = reinterpret_cast<decltype(fnCreateMaterial)>(Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 8B F2")));

	fnTraceSmokeDensity = reinterpret_cast<decltype(fnTraceSmokeDensity)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 0F 28 F8 44 0F 28 54 24 ?")), 0x1, 0x0));

	fnGetEntityHandle = reinterpret_cast<decltype(fnGetEntityHandle)>(Memory::FindPattern(CLIENT_DLL, X("48 85 C9 74 32 48 8B 49 10 48 85 C9 74 29 44 8B 41 10 BA")));

	{
		auto p = Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B D7 48 8B C8 4C 8B 00 41 FF 90 ? ? ? ? 48 8B CF"));
		if ( p )
			fnGetInventoryManager = reinterpret_cast<decltype(fnGetInventoryManager)>(Memory::GetAbsoluteAddress(p, 0x1, 0x0));
	}

	fnLookupBone = reinterpret_cast<decltype(fnLookupBone)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 48 8B 89 ? ? ? ? 48 8B DA 48 8B 01 FF 50 ? 48 8B C8")));

	fnUpdateCompositeMaterial = reinterpret_cast<decltype(fnUpdateCompositeMaterial)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 44 0F B6 F2")));

	fnSetModel = reinterpret_cast<decltype(fnSetModel)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24 40")));

	fnSetMeshGroupMask = reinterpret_cast<decltype(fnSetMeshGroupMask)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 99 ? ? ? ? 48 8B 71")));

	fnAddNametagEntity = reinterpret_cast<decltype(fnAddNametagEntity)>(Memory::FindPattern(CLIENT_DLL, X("40 55 53 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B DA")));

	fnCreateSharedObjectSubclassEconItem = reinterpret_cast<decltype(fnCreateSharedObjectSubclassEconItem)>(Memory::FindPattern(CLIENT_DLL, X("48 83 EC 28 B9 48 00 00 00 E8 ? ? ? ? 48 85")));

	fnCreateBaseTypeCache = reinterpret_cast<decltype(fnCreateBaseTypeCache)>(Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 4C 8B 49 ? 44 8B D2")));

	fnGetClientSystem = reinterpret_cast<decltype(fnGetClientSystem)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 8B D8 85 C0 74 33")), 0x1, 0x0));

	fnSetDynamicAttributeValue = reinterpret_cast<decltype(fnSetDynamicAttributeValue)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 49 8B C0 48 8B CA 48 8B D0")), 0x1, 0x0));

	fnCAttributeStringInit = reinterpret_cast<decltype(fnCAttributeStringInit)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 7D ? 48 89 45 ? 49 8D 4F")), 0x1, 0x0));

	fnCAttributeStringFill = reinterpret_cast<decltype(fnCAttributeStringFill)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 41 83 CF 08")), 0x1, 0x0));

	fnSetDynamicAttributeValueString = reinterpret_cast<decltype(fnSetDynamicAttributeValueString)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8B 5C 24 ? 48 8B 6C 24 ? 48 8B 74 24 ? 48 83 C4 40")), 0x1, 0x0));
	fnCEconItemSchema_GetAttributeDefinitionInterface = reinterpret_cast<decltype(fnCEconItemSchema_GetAttributeDefinitionInterface)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 85 C0 74 ? E8 ? ? ? ? 0F B7 14 3B 48 8B C8 E8 ? ? ? ? 0F B6 48")), 0x1, 0x0));

	fnCBufferStringInit = reinterpret_cast<decltype(fnCBufferStringInit)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 8D 79")));

	fnDispatchEffect = reinterpret_cast<decltype(fnDispatchEffect)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 48 8B DA 48 8D 4C 24")));

	fnLoadFileForMe = reinterpret_cast<decltype(fnLoadFileForMe)>(Memory::FindPattern(CLIENT_DLL, X("40 55 57 41 56 48 83 EC 20 4C")));

	fnUpdateSubClass = reinterpret_cast<decltype(fnUpdateSubClass)>(Memory::FindPattern(CLIENT_DLL, X("4C 8B DC 53 48 81 EC ? ? ? ? 48 8B 41")));

	fnCreateNewSubtickMoveStep = reinterpret_cast<decltype(fnCreateNewSubtickMoveStep)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8B D0 48 8B CE E8 ? ? ? ? 48 8B C8")), 0x1, 0x0));

	fnGetBoneFlags = reinterpret_cast<decltype(fnGetBoneFlags)>(Memory::FindPattern(CLIENT_DLL, X("85 D2 78 ? 3B 91 ? ? ? ? 7D ? 48 8B 81 ? ? ? ? 48 63 D2 8B 04 90")));

	fnGetBoneParent = reinterpret_cast<decltype(fnGetBoneParent)>(Memory::FindPattern(CLIENT_DLL, X("85 D2 78 ? 3B 91 ? ? ? ? 7D ? 48 8B 81 ? ? ? ? 48 63 D2 0F BF 04 50")));

	fnGetPunchAngle = reinterpret_cast<decltype(fnGetPunchAngle)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 4C 8B C0 48 8D 55 ? 48 8B CB E8 ? ? ? ? 48 8D 0D")), 0x1, 0x0));

	fnRegenerateWeaponSkin = reinterpret_cast<decltype(fnRegenerateWeaponSkin)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8B D8 48 85 C0 75 ? 48 8B 5C 24 ? 48 83 C4 ? 5F")), 0x1, 0x0));

	return true;
}

bool ReturnAddressSpoofGadgets::FindGadgets()
{
	m_pClientGadet = Memory::FindPattern(CLIENT_DLL, X("FF 27"));
	if (!m_pClientGadet)
		return false;

	m_pServerGadet = Memory::FindPattern(SERVER_DLL, X("FF 27"));
	if (!m_pServerGadet)
		return false;

	m_pEngineGadet = Memory::FindPattern(ENGINE2_DLL, X("FF 27"));
	if (!m_pEngineGadet)
		return false;

	m_pSchemaSystemGadget = Memory::FindPattern(SCHEMASYSTEM_DLL, X("FF 27"));
	if (!m_pSchemaSystemGadget)
		return false;

	m_pAnimationSystemGadget = Memory::FindPattern(ANIMATIONSYSTEM_DLL, X("FF 27"));
	if (!m_pAnimationSystemGadget)
		return false;

	m_pSDL3Gadget = Memory::FindPattern(SDL3_DLL, X("FF 27"));
	if (!m_pSDL3Gadget)
		return false;

	m_pTier0Gadget = Memory::FindPattern(TIER0_DLL, X("FF 27"));
	if (!m_pTier0Gadget)
		return false;

	m_pFileSystemGadget = Memory::FindPattern(FILESYSTEM_STDIO_DLL, X("FF 27"));
	if (!m_pFileSystemGadget)
		return false;

	m_pDbgHelpGadget = Memory::FindPattern(DBGHELP_DLL, X("FF 27"));
	if (!m_pDbgHelpGadget)
		return false;

	m_pParticlesGadget = Memory::FindPattern(PARTICLES_DLL, X("FF 27"));
	if (!m_pParticlesGadget)
		return false;

	m_pSceneSystemGadget = Memory::FindPattern(SCENESYSTEM_DLL, X("FF 27"));
	if (!m_pSceneSystemGadget)
		return false;

	m_pMaterialSystemGadget = Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("FF 27"));
	if (!m_pMaterialSystemGadget)
		return false;

	m_pSoundSystemGadget = Memory::FindPattern(SOUNDSYSTEM_DLL, X("FF 27"));
	if (!m_pSoundSystemGadget)
		return false;

	const void* hTier0Lib = Memory::GetModuleBaseHandle(TIER0_DLL);
	if (hTier0Lib == nullptr)
		return false;

	Functions::fnConColorMsg = reinterpret_cast<void(__cdecl*)(const Color&, const char*, ...)>(Memory::GetExportAddress(hTier0Lib, X("?ConColorMsg@@YAXAEBVColor@@PEBDZZ")));
	if (!Functions::fnConColorMsg)
		return false;

	return true;
}