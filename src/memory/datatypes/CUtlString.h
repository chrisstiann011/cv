#pragma once

class CUtlString;
namespace Functions
{
	inline void(__fastcall* fnUtlStringSet)(CUtlString*, const char*);
}

class CUtlBinaryBlock
{
public:
	CUtlBinaryBlock( const int nInitialGrowSize = 0, const int nInitialSize = 0 ) :
		memory( nInitialGrowSize, nInitialSize ), nLength( 0 ) { }

	CUtlMemory<unsigned char> memory;
	int nLength;
};

class CUtlString
{
public:
	CUtlString( ) { }

	CUtlString(const char* szString)
	{
		Set( szString );
	}

	const char* Get() const
	{
		return m_szString;
	}

	void Set( const char* szValue )
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnUtlStringSet != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pTier0Gadget, Functions::fnUtlStringSet, this, szValue );
	}

private:
	const char* m_szString;

};

template <typename T = char>
class CUtlConstStringBase
{
public:
	CUtlConstStringBase( ) :
		pString( nullptr ) { }

	[[nodiscard]] const T* Get( ) const
	{
		return ( pString != nullptr ? pString : static_cast< T* >( "" ) );
	}

	[[nodiscard]] operator const T* ( ) const
	{
		return ( pString != nullptr ? pString : static_cast< T* >( "" ) );
	}

	[[nodiscard]] bool Empty( ) const
	{
		return ( pString == nullptr );
	}

protected:
	const T* pString;
};

using CUtlConstString = CUtlConstStringBase<char>;
using CUtlConstWideString = CUtlConstStringBase<wchar_t>;