#pragma once

#define EVENT_DEBUG_ID_INIT			42
#define EVENT_DEBUG_ID_SHUTDOWN		13

class IGameEventListener2
{
public:
	virtual			~IGameEventListener2() { }
	virtual void	FireGameEvent(CGameEvent* pEvent) = 0;
	virtual int		GetEventDebugID()
	{
		return nDebugID;
	}
public:
	int	nDebugID = 0;
};

class CEventListener : public IGameEventListener2
{
public:
	void Setup(const std::deque<const char*>& arrEvents);
	void Destroy();

	virtual void FireGameEvent(CGameEvent* pEvent) override;
	virtual int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT;
	}
};

class IGameEventManager2
{
public:
	enum
	{
		SERVERSIDE = 0,
		CLIENTSIDE,
		CLIENTSTUB,
		SERVERSIDE_OLD,
		CLIENTSIDE_OLD
	};

	virtual ~IGameEventManager2() { }

	int LoadEventsFromFile(const char* szFileName)
	{
		return Memory::CallVFunc<int, 1U>(this, szFileName);
	}

	void Reset()
	{
		Memory::CallVFunc<void, 2U>(this);
	}

	bool AddListener(IGameEventListener2* pListener, const char* szName, bool bServerSide = false)
	{
		return Memory::CallVFunc<bool, 3U>(this, pListener, szName, bServerSide);
	}

	bool FindListener(IGameEventListener2* pListener, const char* szName)
	{
		return Memory::CallVFunc<bool, 4U>(this, pListener, szName);
	}

	void RemoveListener(IGameEventListener2* pListener)
	{
		Memory::CallVFunc<void, 5U>(this, pListener);
	}
};