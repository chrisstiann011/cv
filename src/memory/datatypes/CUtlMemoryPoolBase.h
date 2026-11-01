#pragma once

enum class MemoryPoolGrowType_t : std::int32_t
{
    UTLMEMORYPOOL_GROW_NONE = 0,
    UTLMEMORYPOOL_GROW_FAST = 1,
    UTLMEMORYPOOL_GROW_SLOW = 2,
    UTLMEMORYPOOL_GROW_RBTREE = 3
};

enum MemAllocAttribute_t {
    MemAllocAttribute_Unk0 = 0,
    MemAllocAttribute_Unk1 = 1,
    MemAllocAttribute_Unk2 = 2
};

class CUtlMemoryPoolBase
{
public:
    struct FreeList_t
    {
        FreeList_t* m_pNext;
    };

    class CBlob
    {
    public:
        CBlob* m_pNext;
        int m_NumBytes;
        char m_Data[1];
        char m_Padding[3];
    };

    int m_BlockSize;
    int m_BlocksPerBlob;

    MemoryPoolGrowType_t m_GrowMode;

    CInterlockedInt m_BlocksAllocated;
    CInterlockedInt m_PeakAlloc;
    std::uint16_t m_nAlignment;
    std::uint16_t m_NumBlobs;

    CTSListBase m_FreeBlocks;

    MemAllocAttribute_t m_AllocAttribute;

    CThreadMutex m_Mutex;

    CBlob* m_pBlobHead;

    int m_TotalSize;
};
static_assert(sizeof(CUtlMemoryPoolBase) == 0x80);