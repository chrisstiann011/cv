
using UtlTsHashHandleT = std::uint64_t;

template <class T>
class ITSHashConstructor {
public:
    virtual void Construct(T* pElement) = 0;
};

template <class T>
class CDefaultTSHashConstructor : public ITSHashConstructor<T> {
public:
    virtual void Construct(T* pElement) {
        ::Construct(pElement);
    }
};

inline unsigned HashIntConventional(const int n)
{

    unsigned hash = 0xAAAAAAAA + (n & 0xFF);

    hash = (hash << 5) + hash + ((n >> 8) & 0xFF);

    hash = (hash << 5) + hash + ((n >> 16) & 0xFF);

    hash = (hash << 5) + hash + ((n >> 24) & 0xFF);

    return hash;

}
template <class KEYTYPE = std::uint64_t>
class CUtlTSHashGenericHash
{
public:
    static int Hash(const KEYTYPE& key, int nBucketMask)
    {
        int nHash = HashIntConventional((std::uint64_t)key);
        if (nBucketMask <= USHRT_MAX)
        {
            nHash ^= (nHash >> 16);
        }
        if (nBucketMask <= UCHAR_MAX)
        {
            nHash ^= (nHash >> 8);
        }
        return (nHash & nBucketMask);
    }

    static bool Compare(const KEYTYPE& lhs, const KEYTYPE& rhs)
    {
        return lhs == rhs;
    }
};

template <class KEYTYPE>
class CUtlTSHashUseKeyHashMethod
{
public:
    static int Hash(const KEYTYPE& key, int nBucketMask)
    {
        std::uint32_t nHash = key.HashValue();
        return (nHash & nBucketMask);
    }

    static bool Compare(const KEYTYPE& lhs, const KEYTYPE& rhs)
    {
        return lhs == rhs;
    }
};

template <class T, class Keytype = std::uint64_t, int BucketCount = 256, class HashFuncs = CUtlTSHashGenericHash<Keytype>>
class CUtlTSHash
{
public:

    static UtlTsHashHandleT InvalidHandle(void)
    {
        return static_cast<UtlTsHashHandleT>(0);
    }

    [[nodiscard]] int BlockSize() const
    {
        return m_EntryMemory.m_BlockSize;
    }

    [[nodiscard]] int PeakAlloc() const
    {
        return m_EntryMemory.m_PeakAlloc;
    }

    [[nodiscard]] int BlocksAllocated() const
    {
        return m_EntryMemory.m_BlocksAllocated;
    }

    [[nodiscard]] int Count() const
    {
        return BlocksAllocated() == 0 ? PeakAlloc() : BlocksAllocated();
    }

    std::vector<T> GetElements(int nFirstElement = 0);

public:
    class HashAllocatedBlob_t
    {
        public:
        HashAllocatedBlob_t* m_unAllocatedNext;
        char pad_0008[8];
        T m_unAllocatedData;
        char pad_0018[8];
    };

    template <typename Data_t>
    struct HashFixedDataInternal_t {
        Keytype m_uiKey;
        HashFixedDataInternal_t<Data_t>* m_pNext;
        Data_t m_Data;
    };

    typedef HashFixedDataInternal_t<T> HashFixedData_t;
    class HashBucket_t
    {
        public:
        CThreadSpinRWLock m_AddLock;
        HashFixedData_t* m_pFirst;
        HashFixedData_t* m_pFirstUncommitted;
    };
    static_assert(sizeof(HashBucket_t) == 0x28);

    CUtlMemoryPoolBase m_EntryMemory;
    std::array<HashBucket_t, BucketCount> m_aBuckets;
    bool m_bNeedsCommit;
    CInterlockedInt m_ContentionCheck;
};

template <class T, class Keytype, int BucketCount, class HashFuncs>
std::vector<T> CUtlTSHash<T, Keytype, BucketCount, HashFuncs>::GetElements(int nFirstElement)
{
    int n_count = BlocksAllocated();
    std::vector<T> AllocatedList;
    if (n_count > 0) {
        int nIndex = 0;
        for (int i = 0; i < BucketCount; i++) {
            const HashBucket_t& bucket = m_aBuckets[i];
            for (HashFixedData_t* pElement = bucket.m_pFirstUncommitted; pElement; pElement = pElement->m_pNext) {
                if (--nFirstElement >= 0)
                    continue;

                AllocatedList.emplace_back(pElement->m_Data);
                ++nIndex;

                if (nIndex >= n_count)
                    break;
            }
        }
    }

    n_count = PeakAlloc();
    std::vector<T> unAllocatedList;
    if (n_count > 0) {
        int nIndex = 0;
        auto m_unBuckets = *reinterpret_cast<HashAllocatedBlob_t**>(&m_EntryMemory.m_FreeBlocks.m_Head.value32);
        for (auto unallocated_element = m_unBuckets; unallocated_element; unallocated_element = unallocated_element->m_unAllocatedNext) {
            if (unallocated_element->m_unAllocatedData == nullptr)
                continue;

            unAllocatedList.emplace_back(unallocated_element->m_unAllocatedData);
            ++nIndex;

            if (nIndex >= n_count)
                break;
        }
    }

    return unAllocatedList.size() > AllocatedList.size() ? unAllocatedList : AllocatedList;
}