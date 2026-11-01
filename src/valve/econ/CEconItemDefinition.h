#pragma once

#pragma pack(push, 1)
class CEconItemDefinition
{
public:
	bool IsWeapon()
	{

		return GetStickersSupportedCount() >= 4;
	}

	bool IsKnife(bool bExcludeDefault)
	{
		if ( !m_pszItemTypeName )
			return false;
		static constexpr FNV1A_t CSGO_Type_Knife = FNV1A::HashConst("#CSGO_Type_Knife");
		if ( FNV1A::Hash( m_pszItemTypeName ) != CSGO_Type_Knife )
			return false;
		return bExcludeDefault ? m_uDefinitionIndex >= 500 : true;
	}

	bool IsGlove(bool bExcludeDefault)
	{
		if ( !m_pszItemTypeName )
			return false;
		static constexpr auto Type_Hands = FNV1A::HashConst("#Type_Hands");
		if ( FNV1A::Hash( m_pszItemTypeName ) != Type_Hands )
			return false;
		const bool bDefaultGlove = m_uDefinitionIndex == WEAPON_DEFAULT_T_GLOVES || m_uDefinitionIndex == WEAPON_DEFAULT_CT_GLOVES;
		return bExcludeDefault ? !bDefaultGlove : true;
	}

	bool IsWeaponCase()
	{
		if ( !m_pszItemTypeName )
			return false;
		static constexpr FNV1A_t Type_WeaponCase = FNV1A::HashConst("#CSGO_Type_WeaponCase");
		return FNV1A::Hash( m_pszItemTypeName ) == Type_WeaponCase;
	}

	bool IsKey()
	{
		if ( !m_pszItemTypeName )
			return false;
		static constexpr FNV1A_t Tool_WeaponCase_KeyTag = FNV1A::HashConst("#CSGO_Tool_WeaponCase_KeyTag");
		return FNV1A::Hash( m_pszItemTypeName ) == Tool_WeaponCase_KeyTag;
	}

	bool IsAgent(bool bExcludeDefault)
	{
		if ( !m_pszItemTypeName )
			return false;
		static constexpr FNV1A_t Type_CustomPlayer = FNV1A::HashConst("#Type_CustomPlayer");
		if ( FNV1A::Hash( m_pszItemTypeName ) != Type_CustomPlayer )
			return false;
		const bool bDefaultAgent = m_uDefinitionIndex == WEAPON_DEFAULT_CT_AGENT || m_uDefinitionIndex == WEAPON_DEFAULT_T_AGENT;
		return bExcludeDefault ? !bDefaultAgent : true;
	}

	const char* GetModelName()
	{
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x148);
	}

	int GetStickersSupportedCount()
	{
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x168);
	}

	const char* GetSimpleWeaponName()
	{
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x260);
	}

	int GetLoadoutSlot()
	{
		return *reinterpret_cast<int*>((uintptr_t)(this) + 0x338);
	}
public:
	MEM_PAD(0x8);
    void* m_pKVItem;
	std::uint16_t m_uDefinitionIndex;
	MEM_PAD(0x1e)
    bool m_bEnabled;
	MEM_PAD(0xf);
	std::uint8_t m_unMinItemLevel;
	std::uint8_t m_unMaxItemLevel;
	std::uint8_t m_nItemRarity;
	std::uint8_t m_nItemQuality;
	std::uint8_t m_nForcedItemQuality;
	std::uint8_t m_nDefaultDropItemQuality;
	std::uint8_t m_nDefaultDropQuantity;
	MEM_PAD(0x19);
	std::uint8_t m_nPopularitySeed;
	MEM_PAD(0x7);
    void* m_pPortraitsKV;
    char* m_pszItemBaseName;
    bool m_bProperName;
	MEM_PAD(0x7);
    char* m_pszItemTypeName;
	MEM_PAD(0x8);
    char* m_pszItemDesc;
	std::uint32_t m_iExpirationTimeStamp;
	std::uint32_t m_iCreationTimeStamp;
    char* m_pszInventoryModel;
    char* m_pszInventoryImage;
	MEM_PAD(0x18);
    int m_iInventoryImagePosition[2];
    int m_iInventoryImageSize[2];
    char* m_pszBaseDisplayModel;
    bool m_bLoadOnDemand;
	MEM_PAD(0x1);
    bool m_bHideBodyGroupsDeployedOnly;
	MEM_PAD(0x5);
    char* m_pszWorldDisplayModel;
    char* m_pszHolsteredModel;
    char* m_pszWorldExtraWearableModel;
	std::uint32_t m_iStickerSlots;
	MEM_PAD(0x4);
    char* m_pszIconDefaultImage;
    bool m_bAttachToHands;
    bool m_bAttachToHandsVMOnly;
    bool m_bFlipViewModel;
    bool m_bActAsWearable;
	MEM_PAD(0x24);
	std::uint32_t m_iItemType;
	MEM_PAD(0x4);
    char* m_pszBrassModelOverride;
    char* m_pszZoomInSoundPath;
    char* m_pszZoomOutSoundPath;
	MEM_PAD(0x18);
	std::uint32_t m_nSoundMaterialID;
    bool m_bDisableStyleSelection;
	MEM_PAD(0x13);
    char* m_pszParticleFile;
    char* m_pszParticleSnapshotFile;
	MEM_PAD(0x40);
    char* m_pszItemClassname;
    char* m_pszItemLogClassname;
    char* m_pszItemIconClassname;
    char* m_pszDefinitionName;
    bool m_bHidden;
    bool m_bShouldShowInArmory;
    bool m_bBaseItem;
    bool m_bFlexibleLoadoutDefault;
    bool m_bImported;
    bool m_bOnePerAccountCDKEY;
	MEM_PAD(0xa);
    char* m_pszArmoryDesc;
	MEM_PAD(0x8);
    char* m_pszArmoryRemap;
    char* m_pszStoreRemap;
    char* m_pszClassToken;
    char* m_pszSlotToken;
	std::uint32_t m_iDropType;
	MEM_PAD(0x4);
    char* m_pszHolidayRestriction;
	std::uint32_t m_iSubType;
	MEM_PAD(0xc)
	std::uint32_t m_unEquipRegionMask;
	std::uint32_t m_unEquipRegionConflictMask;
	MEM_PAD(0x50);
    bool m_bPublicItem;
    bool m_bIgnoreInCollectionView;
	MEM_PAD(0x36);
    int m_iLoadoutSlot;
	MEM_PAD(0x94);
};
#pragma pack(pop)