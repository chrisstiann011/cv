#pragma once

class CParticleSnapshot
{
public:
    void Draw(int nCount, void* pData)
    {
        Memory::CallVFunc<void, 1U>(this, nCount, pData);
    }
};

class CParticleInformation
{
public:
    float m_flTime;
    float m_flWidth;
    float m_flAlpha;
};

class CParticleData
{
public:
    Vector* m_vecPosition;
    MEM_PAD(0x74);
    float* m_flTimes;
    MEM_PAD(0x28);
    float* m_flTimes2;
    MEM_PAD(0x98);
};

class CParticleName
{
    MEM_PAD(0x8);
    char** m_szParticleName;
};

class CParticleCollection
{
    MEM_PAD(0x18);
    CParticleName* m_pName;
};

struct CParticleColor
{
    float m_flRed;
    float m_flGreen;
    float m_flBlue;
};

class CBeamInfo
{
public:
    unsigned int m_nEffectIndex = 0;
    Vector* m_vecPositions = nullptr;
    float* m_flTimes = nullptr;
    CStrongHandle<CParticleSnapshot> m_hSnapsotParticle;
    CParticleData m_ParticleData;
};

class IParticleSystemMgr
{
public:
    void CreateSnapshot(CStrongHandle<CParticleSnapshot>* pOutParticleSnapshot)
    {
        __int64 unknown = 0;
        Memory::CallVFunc<void, 41>(this, pOutParticleSnapshot, &unknown);
    }

    void Draw(CStrongHandle<CParticleSnapshot>* pParticleSnapshot, int nCount, void* pData)
    {
        Memory::CallVFunc<void, 42>(this, pParticleSnapshot, nCount, pData);
    }
};

class CGameParticleManager
{
public:
    int* CreateParticleEffect(unsigned int* pEffectIndex, const char* szName)
    {
#ifdef CS_PARANOID
        CS_ASSERT(Functions::fnCacheParticleEffect != nullptr);
#endif

        return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCacheParticleEffect, this, pEffectIndex, szName, 2, 0ll, 0ll, 0ll, 0);
    }

    bool SetParticleSetting(unsigned int nEffectIndex, int nUnknown, void* pData)
    {
#ifdef CS_PARANOID
        CS_ASSERT(Functions::fnCreateParticleEffect != nullptr);
#endif

        return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCreateParticleEffect, this, nEffectIndex, nUnknown, pData, 0);
    }

    bool UnknownFunction(int nEffectIndex, unsigned int nUnknown, const CStrongHandle<CParticleSnapshot>* pParticleSnapshot)
    {
#ifdef CS_PARANOID
        CS_ASSERT(Functions::fnUnknownParticleFunction != nullptr);
#endif

        return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnUnknownParticleFunction, this, nEffectIndex, nUnknown, pParticleSnapshot);
    }

    void UnkFunc2(CStrongHandle<CParticleSnapshot>* pParticleSnapshot, int nCount, void* pData)
    {
        Memory::CallVFunc<void, 42>(this, pParticleSnapshot, nCount, pData);
    }

    void DestroyParticle(int iIndex, bool a1, bool a2)
    {
        using func_t = void(__fastcall*)(void*, int, char, char);
        static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("83 FA ? 0F 84 ? ? ? ? 41 54"));
        return fn(this, iIndex, a1, a2);
    }

    void ReleaseParticleIndex(int iIndex)
    {
        return Memory::CallVFunc<void, 3U>(this, iIndex);
    }
};