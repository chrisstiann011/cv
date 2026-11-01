#pragma once

class CKeyValues3;
class CUtlBuffer;
struct KV3ID_t;

namespace Functions
{

    inline CKeyValues3*(__fastcall* fnSetTypeKV3)(CKeyValues3*, unsigned int, unsigned int) = nullptr;

    inline bool(__fastcall* fnLoadKeyValues)(CKeyValues3*, void*, CUtlBuffer*, KV3ID_t*, void*, void*, void*, void*, const char*);
    inline void(__stdcall* fnUtlBufferInit)(CUtlBuffer*, int, int, int) = NULL;
    inline void(__stdcall* fnUtlBufferPutString)(CUtlBuffer*, const char*) = NULL;
    inline void(__stdcall* fnUtlBufferEnsureCapacity)(CUtlBuffer*, int) = NULL;

    inline std::uint64_t(__fastcall* fnFindKeyVar)(const char*, unsigned int, int) = NULL;
}

enum EKV3Hashkeys : std::uint64_t
{
    KV3_UNKNOWN_KEY_HASH = 0x31415926,
    KV3_ID_UNKNOWN_HASH1 = 0x41B818518343427E,
    KV3_ID_UNKNOWN_HASH2 = 0xB5F447C23C0CDF8C
};

struct MaterialKeyVar_t
{
    std::uint64_t m_uKey;
    const char* m_szName;

    MaterialKeyVar_t(std::uint64_t uKey, const char* szName) :
        m_uKey(uKey), m_szName(szName) { }

    MaterialKeyVar_t(const char* szName, bool bShouldFindKey = false) :
        m_szName(szName)
    {
        m_uKey = bShouldFindKey ? FindKey(szName) : 0x0;
    }

    std::uint64_t FindKey(const char* szName)
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnFindKeyVar != nullptr);
        #endif

        return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pSceneSystemGadget, Functions::fnFindKeyVar, szName, 0x12U, 0x31415926);
    }
};

class CUtlBuffer
{
public:
    MEM_PAD(0x80);

    CUtlBuffer(int a1, int nSize, int a3)
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnUtlBufferInit != nullptr);
        #endif

        ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pTier0Gadget, Functions::fnUtlBufferInit, this, a1, nSize, a3);
    }

    void PutString(const char* szString)
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnUtlBufferPutString != nullptr);
        #endif

        ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pTier0Gadget, Functions::fnUtlBufferPutString, this, szString);
    }

    void EnsureCapacity(int nSize)
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnUtlBufferEnsureCapacity != nullptr);
        #endif

        ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pTier0Gadget, Functions::fnUtlBufferEnsureCapacity, this, nSize);
    }
};

struct KV3ID_t
{
    const char* m_szName;
    std::uint64_t m_uHash1;
    std::uint64_t m_uHash2;
};

class CKeyValues3
{
public:
    MEM_PAD(0x100);
    std::uint64_t m_uKey;
    void* m_pValue;
    MEM_PAD(0x8);

    void LoadFromBuffer(const char* szString)
    {
        CUtlBuffer buffer(0, static_cast<int>(CRT::StringLength(szString) + 10), 1);
        buffer.PutString(szString);
        LoadKV3(&buffer);
    }

    static CKeyValues3* CreateMaterialResource()
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnSetTypeKV3 != nullptr);
        #endif

        CKeyValues3* pKeyValue = new CKeyValues3[2 * sizeof(std::uintptr_t)];
        return Functions::fnSetTypeKV3(pKeyValue, 1U, 6U);

    }

    bool LoadKV3(CUtlBuffer* buffer)
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnLoadKeyValues != nullptr);
        #endif

        const char* szName = X("");
        KV3ID_t kv3ID = KV3ID_t(X("generic"), KV3_ID_UNKNOWN_HASH1, KV3_ID_UNKNOWN_HASH2);
        return Functions::fnLoadKeyValues(this, nullptr, buffer, &kv3ID, nullptr, nullptr, nullptr, nullptr, X(""));

    }
};