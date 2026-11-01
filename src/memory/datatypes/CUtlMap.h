#pragma once

template <typename K, typename V>
class CUtlMap
{
public:
    struct Node_t
    {
        int m_nLeft;
        int m_nRight;
        int m_nParent;
        int m_nTag;
        K m_Key;
        V m_Value;
    };

    auto begin() const { return m_pData; }
    auto end() const { return m_pData + m_nSize; }

    std::optional<V> FindByKey(K key) const
    {
        int iCurrent = m_nRoot;
        while (iCurrent != -1)
        {
            const Node_t& element = m_pData[iCurrent];
            if (element.m_Key < key)
                iCurrent = element.m_nRight;
            else if (element.m_Key > key)
                iCurrent = element.m_nLeft;
            else
                return element.m_Value;
        }
        return {};
    }

    MEM_PAD(0x8);
    Node_t* m_pData;
    MEM_PAD(0x8);
    int m_nRoot;
    int m_nSize;
    int m_nFirstFree;
    int m_nLastAlloc;
};