#include "../Precompiled.h"

SurfaceData_t* GameTrace_t::GetSurfaceData()
{
#ifdef CS_PARANOID
    CS_ASSERT(Functions::fnGetSurfaceData != nullptr);
#endif

    return reinterpret_cast<SurfaceData_t*>(ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetSurfaceData, m_pSurface));
}

int GameTrace_t::GetHitboxId()
{
    if (m_pHitboxData)
        return m_pHitboxData->m_nHitboxId;
    return 0;
}

int GameTrace_t::GetHitgroup()
{
    if (m_pHitboxData)
        return m_pHitboxData->m_nHitGroup;
    return 0;
}

bool GameTrace_t::DidHitWorld() const
{
    C_BaseEntity* pWorldEntity = Interfaces::m_pGameResourceService->pGameEntitySystem->Get(0);
    if (!pWorldEntity)
        return false;

    return reinterpret_cast<C_BaseEntity*>(m_pHitEntity) == pWorldEntity;
}

bool GameTrace_t::IsVisible() const
{
    return (m_flFraction > 0.97f);
}

TraceFilter_t::TraceFilter_t(uint64_t mask, C_CSPlayerPawn* entity, C_CSPlayerPawn* player, int layer)
{
    this->m_uTraceMask = mask;
    this->m_v1[0] = m_v1[1] = 0;
    this->m_v2 = 15;
    this->m_v3 = layer;
    this->m_v4 = 0x49;

    this->collision = 0;

    if (entity)
    {
        m_arrSkipHandles[0] = entity->GetEntityHandle();
        m_arrSkipHandles[2] = entity->GetEntityHandle();
        m_arrCollisions[0] = entity->GetCollisionMask();
    }

    if (player)
    {
        m_arrSkipHandles[0] = entity->GetEntityHandle();
        m_arrSkipHandles[2] = entity->GetEntityHandle();
        m_arrCollisions[0] = player->GetCollisionMask();
    }
}