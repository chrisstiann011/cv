#pragma once

#define DEBUG_STRINGTOKENS 0

#define STRINGTOKEN_MURMURHASH_SEED 0x31415926
class CUtlStringToken
{
public:
	std::uint32_t m_nHashCode;
#if DEBUG_STRINGTOKENS
	char const* m_pDebugName;
#endif

	CUtlStringToken(char const* szString)
	{
		this->SetHashCode(this->MakeStringToken(szString));
	}

	bool operator==(CUtlStringToken const& other) const
	{
		return (other.m_nHashCode == m_nHashCode);
	}

	bool operator!=(CUtlStringToken const& other) const
	{
		return (other.m_nHashCode != m_nHashCode);
	}

	bool operator<(CUtlStringToken const& other) const
	{
		return (m_nHashCode < other.m_nHashCode);
	}

	uint32_t GetHashCode(void) const
	{
		return m_nHashCode;
	}

	void SetHashCode(uint32_t nCode)
	{
		m_nHashCode = nCode;
	}

	__forceinline std::uint32_t MakeStringToken(char const* szString, int nLen)
	{
		std::uint32_t nHashCode = MurmurHash2LowerCaseA(szString, nLen, STRINGTOKEN_MURMURHASH_SEED);
		return nHashCode;
	}

	__forceinline std::uint32_t MakeStringToken(char const* szString)
	{
		return MakeStringToken(szString, (int)CRT::StringLength(szString));
	}

	CUtlStringToken() { m_nHashCode = 0; }
};