#pragma once

class CNetworkGameClient
{
public:
	MEM_PAD(0x90);
	float m_flRealTime;
	int m_iFrameCount;
	float m_flFrameTime;
	float m_flFrameTime2;
	int m_iMaxClients;
	MEM_PAD(0x8);
	float m_flPlayerInterp;
	double m_dbSomeTimer;
	MEM_PAD(0x8);
	float m_flIntervalPerSubTick;
	float m_flCurrentTime;
	float m_flCurrentTIme2;
	MEM_PAD(0x8);
	bool m_bInPrediction;
	MEM_PAD(0x3);
	int m_iTickCount;
	int m_iTickCount2;
	float m_flSomeTimer;
	MEM_PAD(0x4);
	CNetChannelInfo* m_pNetChannelInfo;
	MEM_PAD(0x8);
	bool m_bShouldPredict;
	MEM_PAD(0x14B);
	int m_iDeltaTick;
	MEM_PAD(0x124);
	int m_iServerTick;
	MEM_PAD(2642556);
	int m_iClientTick;

	void FullUpdate( )
	{

		Memory::CallVFunc<void, 28U>( this, X( "unk" ) );
	}

	void SendStringCommand(int nCount, const char* szCommand)
	{
		Memory::CallVFunc<void, 34U>( this, nCount, szCommand);
	}

	[[nodiscard]] void* GetNetChannel(int nSplitScreenSlot = 0)
	{
		return Memory::CallVFunc<void*, 37U>( this, nSplitScreenSlot);
	}

	[[nodiscard]] float GetClientInterpAmount()
	{
		return Memory::CallVFunc<float, 61U>( this );
	}

	void GetPredictionTick(int* pPredictionTick)
	{
		int unk = 0;
		return Memory::CallVFunc<void, 67U>(this, pPredictionTick, &unk);
	}

	void SetPlayerInterpFraction( )
	{
		Memory::CallVFunc<void, 70U>( this );
	}

	void SentTickSync( bool some_bool )
	{
		Memory::CallVFunc<void, 74U>( this, some_bool );
	}

	void RunPrediction(int nPredictionReason)
	{
		Memory::CallVFunc<void, 75U>(this, nPredictionReason);
	}

	void FullConnect(void* pAddress)
	{
		Memory::CallVFunc<void, 120U>( this, pAddress);
	}
};

class INetworkClientService
{
public:
	INetworkClientService() = delete;
	INetworkClientService(INetworkClientService&&) = delete;
	INetworkClientService(const INetworkClientService&) = delete;

	[[nodiscard]] CNetworkGameClient* GetNetworkGameClient()
	{
		return Memory::CallVFunc<CNetworkGameClient*, 23U>(this);
	}

	void DisconnectGameNow(int nUnk)
	{
		Memory::CallVFunc<void, 27U>(this, nUnk);
	}

	[[nodiscard]] bool IsMultiPlayer()
	{
		return Memory::CallVFunc<bool, 33U>(this);
	}

	[[nodiscard]] CNetChannelInfo* GetNetChannelInfo()
	{
		return Memory::CallVFunc<CNetChannelInfo*, 42U>(this);
	}
};