struct EventHash_t
{
	EventHash_t(const char* szString)
	{
		m_szString = szString;
		m_uHash = MurmurHash2(szString, static_cast<int>(CRT::StringLength(szString)), STRINGTOKEN_MURMURHASH_SEED);
	}

	std::uint32_t m_uHash;
	const char* m_szString;
};

class CBasePlayerController;
class C_BasePlayerPawn;
class CEntityInstace;
class CGameEvent
{
public:
	virtual ~CGameEvent() { }

	[[nodiscard]] const char* GetName() const
	{

		return Memory::CallVFunc<const char*, 1U>(this);
	}

	[[nodiscard]] bool GetBool(const char* szEvent, bool bDefault = false) const
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<bool, 6U>(this, uKeyHash, bDefault);
	}

	[[nodiscard]] int GetInt(const char* szEvent, int iDefault = 0) const
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<int, 7U>(this, uKeyHash, iDefault);
	}

	[[nodiscard]] std::uint64_t GetUint64(const char* szEvent, std::uint64_t ullDefault = 0ULL) const
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<std::uint64_t, 8U>(this, uKeyHash, ullDefault);
	}

	[[nodiscard]] float GetFloat(const char* szEvent, const float flDefault = 0.0f) const
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<float, 9U>(this, uKeyHash, flDefault);
	}

	[[nodiscard]] const char* GetString(const char* szEvent, const char* szDefault = "") const
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<const char*, 10U>(this, uKeyHash, szDefault);
	}

	[[nodiscard]] const void* GetPtr(const char* szEvent) const
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<const void*, 11U>(this, uKeyHash);
	}

	int GetPlayerIndex(const char* szEvent)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		int nOutIndex;
		Memory::CallVFunc<void, 14U>(this, uKeyHash , &nOutIndex);
		return nOutIndex;
	}

	CBasePlayerController* GetPlayerController(const char* szEvent)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<CBasePlayerController*, 16U>(this, uKeyHash);
	}

	C_BasePlayerPawn* GetPlayerPawn(const char* szEvent)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		return Memory::CallVFunc<C_BasePlayerPawn*, 17U>(this, uKeyHash);
	}

	void SetBool(const char* szEvent, const bool bValue)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 20U>(this, uKeyHash, bValue);
	}

	void SetInt(const char* szEvent, const int nValue)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 21U>(this, uKeyHash, nValue);
	}

	void SetUint64(const char* szEvent, const std::uint64_t ullValue)
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 22U>(this, uKeyHash, ullValue);
	}

	void SetFloat(const char* szEvent, const float flValue)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 23U>(this, uKeyHash, flValue);
	}

	void SetString(const char* szEvent, const char* szValue)
	{

		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 24U>(this, uKeyHash, szValue);
	}

	void SetPtr(const char* szEvent, const void* pValue)
	{
		const EventHash_t uKeyHash = EventHash_t(szEvent);
		Memory::CallVFunc<void, 25U>(this, uKeyHash, pValue);
	}
public:
	std::byte pad0[0x60];
};
static_assert(sizeof(CGameEvent) == 0x68);