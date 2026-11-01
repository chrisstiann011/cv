#pragma once

enum EEntityType : unsigned int
{
    ENTITY_INVALID = 0,
    ENTITY_PLAYER,
    ENTITY_GRENADE,
    ENTITY_INFERNO,
    ENTITY_WEAPON,
    ENTITY_PLANTED_C4,
    ENTITY_CHICKEN,
    ENTITY_FISH,
    ENTITY_POSTPROCESSING,
    ENTITY_SKY,
    ENTITY_FOG,
    ENTITY_GRADIENT_FOG
};

struct EntityObject_t
{
    EntityObject_t(C_BaseEntity* pEntity, CBaseHandle uHandle, int nIndex, EEntityType eEntityType, FNV1A_t uHashedName, float flSpawnTime = 0.0f) :
        m_pEntity(pEntity), m_nIndex(nIndex), m_eType(eEntityType), m_uHashedName(uHashedName), m_flSpawnTime(flSpawnTime) {
    }

    C_BaseEntity* m_pEntity = nullptr;
    CBaseHandle   m_uHandle = CBaseHandle();
    int			  m_nIndex = 0;
    EEntityType	  m_eType = EEntityType::ENTITY_INVALID;
    FNV1A_t       m_uHashedName = FNV1A::Hash("");
    float         m_flSpawnTime = 0.0f;
    std::unique_ptr<CRecordTrack> m_pRecordTrack = std::make_unique<CRecordTrack>();
    bool m_bPredictedGrenade = false;

    CRecordTrack* GetRecordTrack() {
        return this->m_pRecordTrack.get();
    }
};

class CEntities
{
public:

    std::vector<EntityObject_t> m_vecEntities;
    std::vector<EntityObject_t> m_vecPlayersOnly;

    void OnAddEntity(CEntityInstance* pInstance, CBaseHandle uHandle)
    {
        if (!pInstance || !uHandle.IsValid())
            return;

        const int nIndex = uHandle.GetEntryIndex();
        if (nIndex < 0)
            return;

        C_BaseEntity* pEntity = reinterpret_cast<C_BaseEntity*>(pInstance);
        if (!pEntity || uHandle.GetEntryIndex() > 0x3FFF)
            return;

        CBaseHandle entityHandle;
        try {
            entityHandle = pEntity->GetRefEHandle();
            if (entityHandle != uHandle)
                return;
        } catch (...) {

            return;
        }

        SchemaClassInfoData_t* pClassInfo = nullptr;
        pEntity->GetSchemaClassInfo(&pClassInfo);
        if (!pClassInfo || !pClassInfo->szName)
            return;

        const FNV1A_t uHashedName = FNV1A::Hash(pClassInfo->szName);

        if (pEntity->IsWeapon())
        {
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_WEAPON, uHashedName));
            return;
        }

        switch (uHashedName)
        {
        case FNV1A::HashConst("CCSPlayerController"):
            this->m_vecPlayersOnly.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_PLAYER, uHashedName));
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_PLAYER, uHashedName));
            break;

        case FNV1A::HashConst("C_HEGrenadeProjectile"):
        case FNV1A::HashConst("C_FlashbangProjectile"):
        case FNV1A::HashConst("C_SmokeGrenadeProjectile"):
        case FNV1A::HashConst("C_DecoyProjectile"):
        case FNV1A::HashConst("C_MolotovProjectile"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_GRENADE, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
            break;

        case FNV1A::HashConst("C_Inferno"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_INFERNO, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
            break;

        case FNV1A::HashConst("C_PlantedC4"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_PLANTED_C4, uHashedName, Interfaces::m_pGlobalVariables->m_flCurrentTime));
            break;

        case FNV1A::HashConst("C_Chicken"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_CHICKEN, uHashedName));
            break;

        case FNV1A::HashConst("C_Fish"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_FISH, uHashedName));
            break;

        case FNV1A::HashConst("C_PostProcessingVolume"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_POSTPROCESSING, uHashedName));
            break;

        case FNV1A::HashConst("C_EnvSky"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_SKY, uHashedName));
            break;

        case FNV1A::HashConst("C_EnvCubemapFog"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_FOG, uHashedName));
            break;

        case FNV1A::HashConst("C_GradientFog"):
            this->m_vecEntities.emplace_back(EntityObject_t(pEntity, uHandle, nIndex, EEntityType::ENTITY_GRADIENT_FOG, uHashedName));
            break;

        default:

            break;
        }
    }

    void OnRemoveEntity(CEntityInstance* pInstance, CBaseHandle uHandle)
    {
        const int nIndex = uHandle.GetEntryIndex();
        if (nIndex < 0)
            return;

        C_BaseEntity* pEntity = reinterpret_cast<C_BaseEntity*>(pInstance);
        if (pEntity == nullptr || uHandle.GetEntryIndex() > 0x3FFF)
            return;

        auto entityIt = std::ranges::find_if(this->m_vecEntities, [&](const EntityObject_t& entry) { return entry.m_nIndex == nIndex; });
        if (entityIt != this->m_vecEntities.end())
            this->m_vecEntities.erase(entityIt);

        SchemaClassInfoData_t* pClassInfo = nullptr;
        pEntity->GetSchemaClassInfo(&pClassInfo);
        if (pClassInfo && FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("CCSPlayerController"))
        {
            auto playerIt = std::ranges::find_if(this->m_vecPlayersOnly, [&](const EntityObject_t& entry) { return entry.m_nIndex == nIndex; });
            if (playerIt != this->m_vecPlayersOnly.end())
                this->m_vecPlayersOnly.erase(playerIt);
        }
    }

    void ForceUpdateEntityList()
    {

        this->m_vecEntities.clear();
        this->m_vecPlayersOnly.clear();

        for (int i = 0; i < Interfaces::m_pGameResourceService->pGameEntitySystem->GetHighestEntityIndex(); i++)
        {
            C_BaseEntity* pEntity = Interfaces::m_pGameResourceService->pGameEntitySystem->Get(i);
            if (!pEntity || !pEntity->m_pEntity() || !pEntity->m_pEntity()->m_pInstance || !pEntity->GetRefEHandle().IsValid())
                continue;

            OnAddEntity(pEntity->m_pEntity()->m_pInstance, pEntity->GetRefEHandle());
        }
    }
};

inline auto g_Entities = std::make_unique<CEntities>();