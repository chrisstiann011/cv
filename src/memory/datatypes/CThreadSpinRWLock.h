#pragma once

class CThreadSpinMutex
{
public:
    CThreadSpinMutex([[maybe_unused]] const char* pDebugName = NULL) : m_ownerID(0), m_depth(0) { }

private:
    volatile ThreadId_t m_ownerID;
    int m_depth;
};

using CThreadFastMutex = CThreadSpinMutex;