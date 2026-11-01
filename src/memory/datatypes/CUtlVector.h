template <typename T>
class CUtlVector
{
public:
	auto begin() const { return m_pData; }
	auto end() const { return m_pData + m_nSize; }

	auto At(int i) { return m_pData[i]; }
	auto AtPtr(int i) { return m_pData + i; }

	bool Exists(T val) const
	{
		for (const auto& it : *this)
			if (it == val) return true;
		return false;
	}
	bool Empty() const { return m_nSize == 0; }

	std::span<T> it() { return std::span(m_pData, m_nSize); }

	int m_nSize;
	MEM_PAD(0x4);
	T* m_pData;
	MEM_PAD(0x8);
};

template <typename T>
class C_NetworkUtlVectorBase
{
public:
	std::uint32_t m_nSize;
	T* m_pElements;
};