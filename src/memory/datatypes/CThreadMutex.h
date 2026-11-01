#pragma once

#ifdef _WIN32
typedef std::uint32_t ThreadId_t;
#else
typedef std::uint64_t ThreadId_t;
#endif

constexpr auto kTtSizeofCriticalsection = 40;

class CThreadMutex
{
public:
    std::byte m_CriticalSection[kTtSizeofCriticalsection];

    ThreadId_t m_currentOwnerID;
    std::uint16_t m_lockCount;
    bool m_bTrace;
    const char* m_pDebugName;
};
static_assert(sizeof(CThreadMutex) == 0x38);