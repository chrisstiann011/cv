#pragma once

struct SOID_t
{
    std::uint64_t m_uID;
    std::uint32_t m_uType;
    std::uint32_t m_uPadding;
};

class CGCClientSharedObjectCache;
class CGCClient
{
public:
    CGCClientSharedObjectCache* FindSOCache(SOID_t ID, bool bCreateIfMissing = true)
    {
        void* shi = *reinterpret_cast<void**>((uintptr_t)this + 0x398);
        return *reinterpret_cast<CGCClientSharedObjectCache**>((uintptr_t)shi + 0x18);
    }
};
