#pragma once

enum EPredictionReason : int
{
    PREDICTION_REASON_CREATEMOVE = 0,
    PREDICTION_REASON_DEMOPREENTITY = 1,
    PREDICTION_REASON_DEMOSIMULATION = 2,
    PREDICTION_REASON_POSTNETUPDATE = 3,
    PREDICTION_REASON_SERVERSTARVEDANDADDEDUSERCMDS = 4,
    PREDICTION_REASON_CLIENTFRAMESIMULATE = 5,
};

class CPrediction
{
public:

	void Update(int nPredictionReason, int iTickCount)
	{
		Memory::CallVFunc<void, 11U>(this, nPredictionReason, iTickCount);
	}
public:
    MEM_PAD(0x30);
    int m_nReason;
    bool m_bInPrediction;
    bool m_bEnginePause;
    MEM_PAD(0x6);
    int m_nIncomingAcknowledged;
    std::uint64_t m_nDeltaTick;
    std::uint64_t m_nSlot;
    CCSPlayerController* m_pControllerEntity;
    MEM_PAD(0x4);
    int int5C;
    MEM_PAD(0x20);
    bool m_bFirstPrediction;
    MEM_PAD(0xF);
    int m_nStorageEntitySize;
    MEM_PAD(0x4);
    void* m_pStorageEntities;
    MEM_PAD(0x38);
    void* m_pPosition;
    MEM_PAD(0x8);
    float m_flRealTime;
    MEM_PAD(0xC);
    int m_nMaxClients;
    MEM_PAD(0xC);
    std::uintptr_t* m_pUnknownClass;
    float m_flCurrentTime;
    MEM_PAD(0xC);
    int m_nTickCount;
    MEM_PAD(0xA)
    bool m_bHasUpdate;
};