#pragma once

class CThreadSpinRWLock
{
public:
    struct LockInfo_t
    {
        std::uint32_t m_writerId;
        std::int32_t m_nReaders;
    };

public:
    void* m_pThreadSpin;
    LockInfo_t m_lockInfo;
    const char* m_pszDebugName;
};
static_assert(sizeof(CThreadSpinRWLock) == 0x18);