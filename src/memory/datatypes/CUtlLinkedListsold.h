#pragma once
#include <cstdint>

template <typename ElementType, typename IndexType = std::uint16_t>
struct CUtlLinkedLists {
    struct UtlLinkedListElem_t {
        ElementType m_Element;
        IndexType m_Previous;
        IndexType m_Next;
    };

    static constexpr auto kInvalidIndex{ static_cast<IndexType>(-1) };

    IndexType size;
    IndexType allocationCount;
    UtlLinkedListElem_t* memory;
    IndexType m_Head;
};