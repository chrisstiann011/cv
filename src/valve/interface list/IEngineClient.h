#pragma once

enum EClientFrameStage : int
{
	FRAME_UNDEFINED = -1,

	FRAME_NET_START = 0,

	FRAME_SIMULATE_START = 1,

	FRAME_NET_UPDATE_START = 2,

	FRAME_NET_UPDATE_PREPROCESS = 3,

	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE = 4,

	FRAME_NET_UPDATE_POSTDATAUPDATE_START = 5,

	FRAME_NET_UPDATE_POSTDATAUPDATE_END = 6,

	FRAME_NET_UPDATE_END = 7,

	FRAME_SIMULATE_END = 9
};

enum EFlow : int
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING = 1,
};

class CNetChannelInfo
{
public:
	[[nodiscard]] const char* GetName()
	{
		return Memory::CallVFunc<const char*, 0U>(this);
	}

	[[nodiscard]] const char* GetAddress()
	{
		return Memory::CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] float GetTime()
	{
		return Memory::CallVFunc<float, 2U>(this);
	}

	[[nodiscard]] float GetTimeConnected()
	{
		return Memory::CallVFunc<float, 3U>(this);
	}

	[[nodiscard]] float GetBufferSize()
	{
		return Memory::CallVFunc<float, 4U>(this);
	}

	[[nodiscard]] float GetDataRate()
	{
		return Memory::CallVFunc<float, 5U>(this);
	}

	[[nodiscard]] float GetLatency(int nFlow)
	{
		return Memory::CallVFunc<float, 9U>(this, nFlow);
	}

	[[nodiscard]] int GetAverageLatency(int nFlow)
	{
		return Memory::CallVFunc<int, 10U>(this, nFlow);
	}

	[[nodiscard]] float GetNetLatency()
	{
		return Memory::CallVFunc<float, 10>(this);
	}
	[[nodiscard]] float GetEngineLatency()
	{
		return Memory::CallVFunc<float, 11>(this);
	}

};

class IEngineClient
{
public:
	[[nodiscard]] int GetMaxClients()
	{
		return Memory::CallVFunc<int, 34U>(this);
	}

	[[nodiscard]] bool IsInGame()
	{
		return Memory::CallVFunc<bool, 38U>(this);
	}

	[[nodiscard]] bool IsConnected()
	{
		return Memory::CallVFunc<bool, 39U>(this);
	}

	[[nodiscard]] CNetChannelInfo* GetNetChannelInfo(int nSplitScreenSlot = 0)
	{
		return Memory::CallVFunc<CNetChannelInfo*, 40U>(this, nSplitScreenSlot);
	}

	void ExecuteClientCmdUnrestricted(const char* szCommand)
	{
		Memory::CallVFunc<void, 49U>(this, 0, szCommand, 0x7FFEF001);
	}

	[[nodiscard]] int GetLocalPlayer()
	{
		int nIndex = -1;

		Memory::CallVFunc<void, 53U>(this, std::ref(nIndex), 0);

		return nIndex + 1;
	}

	void GetScreenSize(int& iWidth, int& iHeight)
	{
		Memory::CallVFunc<void, 60U>(this, std::ref(iWidth), std::ref(iHeight));
	}

	[[nodiscard]] const char* GetLevelName()
	{
		return Memory::CallVFunc<const char*, 62>(this);
	}

	[[nodiscard]] const char* GetLevelNameShort()
	{
		return Memory::CallVFunc<const char*, 63>(this);
	}

	[[nodiscard]] const char* GetProductVersionString()
	{
		return Memory::CallVFunc<const char*, 84U>(this);
	}

	[[nodiscard]] void GetClientNetworkData(CClientNetworkData* pClientNetworkData)
	{
		Memory::CallVFunc<void, 178U>(this, pClientNetworkData);
	}
};