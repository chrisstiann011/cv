#pragma once

class CEconItemDefinition;

inline constexpr std::uint64_t Helper_GetAlternateIconKeyForWeaponPaintWearItem(std::uint16_t nDefIdx, std::uint32_t nPaintId, std::uint32_t nWear)
{
    return (nDefIdx << 16) + (nPaintId << 2) + nWear;
}

struct AlternateIconData_t
{
    const char* m_szSimpleName;
    const char* m_szLargeSimpleName;
    MEM_PAD(0x10);
};

#pragma pack(push, 1)
struct CPaintKit
{
    std::uint64_t m_nID;
    const char* m_szName;
    const char* m_szDescriptionString;
    const char* m_szDescriptionTag;
    MEM_PAD(0x8);
    const char* m_szPattern;
    const char* m_szNormal;
    const char* m_szLogoMaterial;
    MEM_PAD(0x4);
    std::uint32_t m_nRarity;
    std::uint32_t m_nStyle;
    Color m_rgbaColor[4];
    Color m_rgbaLogoColor[4];
    float m_flWearDefault;
    float m_flWearRemapMin;
    float m_flWearRemapMax;
    std::uint8_t m_nFixedSeed;
    std::uint8_t m_uchPhongExponent;
    std::uint8_t m_uchPhongAlbedoBoost;
    std::uint8_t m_uchPhongIntensity;
    float m_flPatternScale;
    float m_flPatternOffsetXStart;
    float m_flPatternOffsetXEnd;
    float m_flPatternOffsetYStart;
    float m_flPatternOffsetYEnd;
    float m_flPatternRotateStart;
    float m_flPatternRotateEnd;
    float m_flLogoScale;
    float m_flLogoOffsetX;
    float m_flLogoOffsetY;
    float m_flLogoRotation;
    bool m_bIgnoreWeaponSizeScale;
    MEM_PAD(0x3);
    std::uint32_t m_nViewModelExponentOverrideSize;
    bool m_bOnlyFirstMaterial;
    bool m_bUseNormalModel;
    bool m_bUseLegacyModel;
    MEM_PAD(0x1);
    float m_flPearlesent;
    const char* m_szVmtPath;
    MEM_PAD(0x8);
    const char* m_szCompositeMaterialPath;
    void* m_pkvVmtOverrides;
    MEM_PAD(0x8);
};
#pragma pack(pop)

class CStickerKit
{
public:
    int m_nID;
    int m_nRarity;
    const char* m_szName;
    const char* m_szDescription;
    const char* m_szItemName;
    const char* m_szMaterialPath;
    const char* m_szMaterialPathNoDrips;
    const char* m_szInventoryImage;
    int m_nTournamentID;
    int m_nTournamentTeamID;
    int m_nTournamentPlayerID;
    bool m_bMaterialPathIsAbsolute;
    bool m_bCannotTrade;
    MEM_PAD(0x2);
    float m_flRotateStart;
    float m_flRotateEnd;
    float m_flScaleMin;
    float m_flScaleMax;
    float m_flWearMin;
    float m_flWearMax;
    const char* m_szIconURLSmall;
    const char* m_szIconURLLarge;
    void* m_pKVItem;
};

class CEconMusicDefinition;
class CEconColorDefinition
{

};

struct ItemListEntry_t
{
    int m_nItemDefinition;
    int m_nPaintKit;
    MEM_PAD(20)

    EItemDefinitionIndexes GetWeaponDefinition() const noexcept
    {
        return static_cast<EItemDefinitionIndexes>(m_nItemDefinition);
    }
};

class EconLootListDefinition
{
public:
    const char* GetName()
    {
        return Memory::CallVFunc<const char*, 0U>(this);
    }

    const CUtlVector<ItemListEntry_t>& GetLootListContents()
    {
		return Memory::CallVFunc<const CUtlVector<ItemListEntry_t>&, 1U>(this);
	}
};

class EconItemSetDefinition
{
public:
    const char* GetLockKey()
    {
        return Memory::CallVFunc<const char*, 1U>(this);
    }

    int GetItemCount()
    {
        return Memory::CallVFunc<int, 4U>(this);
    }

    int GetItemDefinition(int nIndex)
    {
        return Memory::CallVFunc<int, 5U>(this, nIndex);
    }

    int GetPaintKit(int nIndex)
    {
        return Memory::CallVFunc<int, 6U>(this, nIndex);
    }
};

class CEconItemSchema
{
public:
    auto& GetSortedItemDefinitionMap()
    {
        return *reinterpret_cast<CUtlMap<int, CEconItemDefinition*>*>(
            (uintptr_t)(this) + 0x128);
    }

    auto& GetPaintKits()
    {
        return *reinterpret_cast<CUtlMap<int, CPaintKit*>*>((uintptr_t)(this) +
            0x2F0);
    }

    auto GetAttributeDefinitionInterface(int iAttribIndex) -> void*
    {
        return Functions::fnCEconItemSchema_GetAttributeDefinitionInterface(this, iAttribIndex);
    }

public:
    CEconItemDefinition* GetItemDefinitionInterface(int nID)
    {
        return Memory::CallVFunc<CEconItemDefinition*, 4U>(this, nID);
    }

    const char* GetRarityName(int nRarity)
    {
		return Memory::CallVFunc<const char*, 19U>(this, nRarity);
	}

    int GetItemSetCount()
    {
        return Memory::CallVFunc<int, 28U>(this);
    }

    EconItemSetDefinition* GetItemSet(int nIndex)
    {
        return Memory::CallVFunc<EconItemSetDefinition*, 29U>(this, nIndex);
    }

    EconLootListDefinition* GetLootList(int nIndex)
    {
        return Memory::CallVFunc<EconLootListDefinition*, 32U>(this, nIndex);
    }

    int GetLootListCount()
    {
        return Memory::CallVFunc<int, 34U>(this);
    }

    CEconItemDefinition* GetItemDefinitionByName(const char* szName)
    {
		return Memory::CallVFunc<CEconItemDefinition*, 42U>(this, szName);
	}
};
