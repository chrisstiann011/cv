#pragma once
using CUtlSymLargeId = std::int32_t*;
class CUtlSymbolLarge
{
    inline static const CUtlSymLargeId UTL_INVALID_SYMBOL_LARGE = reinterpret_cast<CUtlSymLargeId>(-1);

public:
    CUtlSymbolLarge() { m_nID = UTL_INVALID_SYMBOL_LARGE; }
    CUtlSymbolLarge(CUtlSymbolLarge const& sym) { m_nID = sym.m_nID; }
    explicit CUtlSymbolLarge(CUtlSymLargeId id) { m_nID = id; }

    CUtlSymbolLarge& operator=(CUtlSymbolLarge const& src)
    {
        m_nID = src.m_nID;

        return *this;
    }

    bool operator==(CUtlSymbolLarge const& src) const { return m_nID == src.m_nID; }
    bool operator==(CUtlSymLargeId const& src) const { return m_nID == src; }
    bool operator!=(CUtlSymbolLarge const& src) const { return m_nID != src.m_nID; }
    bool operator!=(CUtlSymLargeId const& src) const { return m_nID != src; }
    explicit operator CUtlSymLargeId() const { return m_nID; }

    [[nodiscard]] const char* GetString() const
    {
        if (m_nID == UTL_INVALID_SYMBOL_LARGE)
            return "";

        return m_szString;
    }

    [[nodiscard]] bool IsValid() const
    {
        return m_nID != UTL_INVALID_SYMBOL_LARGE;
    }

private:
    union
    {
        CUtlSymLargeId m_nID;
        const char* m_szString;
    };
};

