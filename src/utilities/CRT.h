#pragma once
#include <stb_sprintf.h>

#ifdef CS_COMPILER_CLANG
#define CS_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX) __attribute__((format(METHOD, STRING_INDEX, FIRST_INDEX)))
#elif defined(__RESHARPER__)
#define CS_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX) [[rscpp::format(METHOD, STRING_INDEX, FIRST_INDEX)]]
#else
#define CS_CRT_FORMAT_STRING_ATTRIBUTE(METHOD, STRING_INDEX, FIRST_INDEX)
#endif

#ifndef CS_COMPILER_MSC
#define CS_COMPILER_MSC
#endif

#ifndef CS_ARRAYSIZE
#define CS_ARRAYSIZE( a ) ( sizeof( a ) / sizeof( ( a )[ 0 ] ) )
#endif

namespace CRT
{

	template <typename T> requires ( std::is_floating_point_v<T> )
		struct FloatSpecification_t
	{
		static_assert( std::numeric_limits<T>::is_iec559 );
		static_assert( sizeof( long double ) == 8, "CRT rebuild doesn't support binary128 format" );

		using BitEquivalent_t = std::conditional_t<std::is_same_v<T, double> || std::is_same_v<T, long double>, std::uint64_t, std::uint32_t>;

		static constexpr std::uint32_t MANTISSA_WIDTH = ( ( sizeof( T ) == sizeof( double ) ) ? 52U : 23U );
		static constexpr std::uint32_t EXPONENT_WIDTH = ( ( sizeof( T ) == sizeof( double ) ) ? 11U : 8U );
		static constexpr std::uint32_t SIGN_WIDTH = 1U;
		static constexpr std::uint32_t TOTAL_WIDTH = MANTISSA_WIDTH + EXPONENT_WIDTH + SIGN_WIDTH;
		static constexpr std::uint32_t EXPONENT_BIAS = ( 1U << ( EXPONENT_WIDTH - 1U ) ) - 1U;

		static constexpr BitEquivalent_t MANTISSA_MASK = ( BitEquivalent_t( 1U ) << MANTISSA_WIDTH ) - 1U;
		static constexpr BitEquivalent_t SIGN_MASK = ( BitEquivalent_t( 1U ) << ( EXPONENT_WIDTH + MANTISSA_WIDTH ) );
		static constexpr BitEquivalent_t EXPONENT_MASK = ~( SIGN_MASK | MANTISSA_MASK );

		static constexpr BitEquivalent_t QUIET_NAN_MASK = ( BitEquivalent_t( 1U ) << ( MANTISSA_WIDTH - 1U ) );
	};

	constexpr int _NUMBER_MAX_BASE = 36;

	constexpr const char* _NUMBER_ALPHA = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	constexpr char _TWO_DIGITS_LUT[ ] =
		"0001020304050607080910111213141516171819"
		"2021222324252627282930313233343536373839"
		"4041424344454647484950515253545556575859"
		"6061626364656667686970717273747576777879"
		"8081828384858687888990919293949596979899";

	constexpr char _TWO_DIGITS_HEX_LUT[ ] =
		"000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F"
		"202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E3F"
		"404142434445464748494A4B4C4D4E4F505152535455565758595A5B5C5D5E5F"
		"606162636465666768696A6B6C6D6E6F707172737475767778797A7B7C7D7E7F"
		"808182838485868788898A8B8C8D8E8F909192939495969798999A9B9C9D9E9F"
		"A0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
		"C0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
		"E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

	template <typename T, std::size_t BASE> requires ( std::is_integral_v<T>&& BASE > 0U && BASE <= _NUMBER_MAX_BASE )
		struct IntegerToString_t
	{

		static consteval std::size_t MaxCount( )
		{
			std::size_t nDigitsCount = 0U;

			constexpr std::uint64_t ullNegativeMax = ( std::is_unsigned_v<T> ? ( std::numeric_limits<T>::max )( ) : ( static_cast< std::uint64_t >( ( std::numeric_limits<T>::max )( ) ) + 1ULL ) );

			for (std::uint64_t nShift = ullNegativeMax; nShift > 0ULL; nShift /= BASE)
				nDigitsCount++;

			return ( nDigitsCount + ( std::is_signed_v<T> && BASE == 10U ? 1U : 0U ) + 1U );
		}
	};

#pragma region stl_utility

	template <class T>
	[[nodiscard]] __forceinline constexpr std::remove_reference_t<T>&& Move( T&& argument ) noexcept
	{
		return static_cast< std::remove_reference_t<T>&& >( argument );
	}

	template <class T>
	[[nodiscard]] __forceinline constexpr T&& Forward( std::remove_reference_t<T>& argument ) noexcept
	{
		return static_cast< T&& >( argument );
	}

	template <class T>
	[[nodiscard]] __forceinline constexpr T&& Forward( std::remove_reference_t<T>&& argument ) noexcept
	{
		static_assert( !std::is_lvalue_reference_v<T>, "bad forward call" );
		return static_cast< T&& >( argument );
	}

	template <typename T>
		requires ( std::is_move_constructible_v<T>&& std::is_move_assignable_v<T> )
	__forceinline constexpr void Swap( T& left, T& right ) noexcept( std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_assignable_v<T> )
	{
		T temporary = Move( left );
		left = Move( right );
		right = Move( temporary );
	}

#pragma endregion

#pragma region crt_memory

	__forceinline int MemoryCompare( const void* pLeftBuffer, const void* pRightBuffer, std::size_t nCount )
	{
		auto pLeftByte = static_cast< const std::uint8_t* >( pLeftBuffer );
		auto pRightByte = static_cast< const std::uint8_t* >( pRightBuffer );

		while (nCount--)
		{
			if (*pLeftByte++ != *pRightByte++)
				return pLeftByte[ -1 ] - pRightByte[ -1 ];
		}

		return 0;
	}

	__forceinline int MemoryCompareW( const wchar_t* pwLeftBuffer, const wchar_t* pwRightBuffer, std::size_t nCount )
	{
		while (nCount--)
		{
			if (*pwLeftBuffer++ != *pwRightBuffer++)
				return pwLeftBuffer[ -1 ] - pwRightBuffer[ -1 ];
		}

		return 0;
	}

	__forceinline void* MemoryChar( const void* pBuffer, const std::uint8_t uSearch, std::size_t nCount )
	{
		auto pByte = static_cast< const std::uint8_t* >( pBuffer );

		while (nCount--)
		{
			if (*pByte == uSearch)
				return const_cast< std::uint8_t* >( pByte );

			++pByte;
		}

		return nullptr;
	}

	__forceinline wchar_t* MemoryCharW( wchar_t* pwBuffer, const wchar_t wSearch, std::size_t nCount )
	{
		while (nCount--)
		{
			if (*pwBuffer == wSearch)
				return pwBuffer;

			++pwBuffer;
		}

		return nullptr;
	}

	__forceinline void* MemorySet( void* pDestination, const std::uint8_t uByte, std::size_t nCount )
	{
#ifdef CS_COMPILER_MSC

		if (const std::size_t nCountAlign = ( nCount & 3U ); nCountAlign == 0U)
		{
			auto pDestinationLong = static_cast< unsigned long* >( pDestination );
			__stosd( pDestinationLong, static_cast< unsigned long >( uByte ) * 0x01010101, nCount >> 2U );
		}
		else if (nCountAlign == 2U)
		{
			auto pDestinationWord = static_cast< unsigned short* >( pDestination );
			__stosw( pDestinationWord, static_cast< unsigned short >( uByte | ( uByte << 8U ) ), nCount >> 1U );
		}
		else
		{
			auto pDestinationByte = static_cast< unsigned char* >( pDestination );
			__stosb( pDestinationByte, uByte, nCount );
		}
#else
		auto pDestinationByte = static_cast< std::uint8_t* >( pDestination );

		while (nCount--)
			*pDestinationByte++ = uByte;
#endif

		return pDestination;
	}

	__forceinline void* MemoryCopy( void* pDestination, const void* pSource, std::size_t nCount )
	{
#ifdef CS_COMPILER_MSC

		if (const std::size_t nCountAlign = ( nCount & 3U ); nCountAlign == 0U)
		{
			auto pDestinationLong = static_cast< unsigned long* >( pDestination );
			auto pSourceLong = static_cast< const unsigned long* >( pSource );
			__movsd( pDestinationLong, pSourceLong, nCount >> 2U );
		}
		else if (nCountAlign == 2U)
		{
			auto pDestinationWord = static_cast< unsigned short* >( pDestination );
			auto pSourceWord = static_cast< const unsigned short* >( pSource );
			__movsw( pDestinationWord, pSourceWord, nCount >> 1U );
		}
		else
		{
			auto pDestinationByte = static_cast< unsigned char* >( pDestination );
			auto pSourceByte = static_cast< const unsigned char* >( pSource );
			__movsb( pDestinationByte, pSourceByte, nCount );
		}
#else
		auto pDestinationByte = static_cast< std::uint8_t* >( pDestination );
		auto pSourceByte = static_cast< const std::uint8_t* >( pSource );

		while (nCount--)
			*pDestinationByte++ = *pSourceByte++;
#endif

		return pDestination;
	}

	__forceinline void* MemoryMove( void* pDestination, const void* pSource, std::size_t nCount )
	{
		auto pDestinationByte = static_cast< std::uint8_t* >( pDestination );
		auto pSourceByte = static_cast< const std::uint8_t* >( pSource );

		if (pDestinationByte < pSourceByte)
		{
#ifdef CS_COMPILER_MSC

			__movsb( pDestinationByte, pSourceByte, nCount );
#else
			while (nCount--)
				*pDestinationByte++ = *pSourceByte++;
#endif
		}

		else
		{
			std::uint8_t* pLastDestinationByte = pDestinationByte + ( nCount - 1U );
			const std::uint8_t* pLastSourceByte = pSourceByte + ( nCount - 1U );

			while (nCount--)
				*pLastDestinationByte-- = *pLastSourceByte--;
		}

		return pDestination;
	}

#pragma endregion

#pragma region crt_characters

	[[nodiscard]] constexpr bool IsControl( const std::uint8_t uChar )
	{
		return ( uChar <= 0x1F || uChar == 0x7F );
	}

	[[nodiscard]] constexpr bool IsDigit( const std::uint8_t uChar )
	{
		return ( uChar >= '0' && uChar <= '9' );
	}

	[[nodiscard]] constexpr bool IsHexDigit( const std::uint8_t uChar )
	{
		return ( ( uChar >= '0' && uChar <= '9' ) || ( uChar >= 'A' && uChar <= 'F' ) || ( uChar >= 'a' && uChar <= 'f' ) );
	}

	[[nodiscard]] constexpr bool IsBlank( const std::uint8_t uChar )
	{
		return ( uChar == '\t' || uChar == ' ' );
	}

	[[nodiscard]] constexpr bool IsSpace( const std::uint8_t uChar )
	{
		return ( ( uChar >= '\t' && uChar <= '\r' ) || uChar == ' ' );
	}

	[[nodiscard]] constexpr bool IsAlpha( const std::uint8_t uChar )
	{
		return ( ( uChar >= 'A' && uChar <= 'Z' ) || ( uChar >= 'a' && uChar <= 'z' ) );
	}

	[[nodiscard]] constexpr bool IsAlphaNum( const std::uint8_t uChar )
	{
		return ( ( uChar >= '0' && uChar <= '9' ) || ( uChar >= 'A' && uChar <= 'Z' ) || ( uChar >= 'a' && uChar <= 'z' ) );
	}

	[[nodiscard]] constexpr bool IsPrint( const std::uint8_t uChar )
	{
		return ( uChar >= ' ' && uChar <= '~' );
	}

	[[nodiscard]] constexpr bool IsGraph( const std::uint8_t uChar )
	{
		return ( uChar >= '!' && uChar <= '~' );
	}

	[[nodiscard]] constexpr bool IsPunct( const std::uint8_t uChar )
	{
		return ( ( uChar >= '!' && uChar <= '/' ) || ( uChar >= ':' && uChar <= '@' ) || ( uChar >= '[' && uChar <= '`' ) || ( uChar >= '{' && uChar <= '~' ) );
	}

	[[nodiscard]] constexpr bool IsUpper( const std::uint8_t uChar )
	{
		return ( uChar >= 'A' && uChar <= 'Z' );
	}

	[[nodiscard]] constexpr bool IsLower( const std::uint8_t uChar )
	{
		return ( uChar >= 'a' && uChar <= 'z' );
	}

#pragma endregion

#pragma region crt_character_conversion

	[[nodiscard]] constexpr std::int32_t CharToInt( const std::uint8_t uChar )
	{
		return IsDigit( uChar ) ? ( uChar - '0' ) : 0;
	}

	[[nodiscard]] constexpr std::uint32_t CharToHexInt( const std::uint8_t uChar )
	{
		const std::uint8_t uCharLower = ( uChar | ( 'a' ^ 'A' ) );
		return ( ( uCharLower >= 'a' && uCharLower <= 'f' ) ? ( uCharLower - 'a' + 0xA ) : ( IsDigit( uChar ) ? ( uChar - '0' ) : 0x0 ) );
	}

	[[nodiscard]] constexpr char CharToUpper( const std::uint8_t uChar )
	{
		return static_cast< char >( IsLower( uChar ) ? ( uChar & ~( 'a' ^ 'A' ) ) : uChar );
	}

	[[nodiscard]] constexpr char CharToLower( const std::uint8_t uChar )
	{
		return static_cast< char >( IsUpper( uChar ) ? ( uChar | ( 'a' ^ 'A' ) ) : uChar );
	}

#pragma endregion

#pragma region crt_string

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr std::size_t StringLength( const C* tszSource )
	{
		if ( !tszSource )
			return 0U;
		const C* tszSourceEnd = tszSource;
		while ( *tszSourceEnd != C( '\0' ) )
			++tszSourceEnd;
		return static_cast<std::size_t>( tszSourceEnd - tszSource );
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr std::size_t StringLengthN( const C* tszSource, const std::size_t nMaxLength )
	{
		if ( !tszSource )
			return 0U;
		std::size_t i = 0U;
		while ( tszSource[ i ] != C( '\0' ) && i < nMaxLength )
			++i;
		return i;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr int StringCompare( const C* tszLeft, const C* tszRight )
	{
		if (tszLeft == nullptr)
			return -1;

		if (tszRight == nullptr)
			return 1;

		using ComparisonType_t = std::conditional_t<std::is_same_v<C, char>, std::uint8_t, std::conditional_t<sizeof( wchar_t ) == 2U, std::int16_t, std::int32_t>>;

		ComparisonType_t nLeft, nRight;
		do
		{
			nLeft = static_cast< ComparisonType_t >( *tszLeft++ );
			nRight = static_cast< ComparisonType_t >( *tszRight++ );

			if (nLeft == C( '\0' ))
				break;
		} while (nLeft == nRight);

		return nLeft - nRight;
	}

	constexpr int StringCompareI( const char* szLeft, const char* szRight )
	{
		std::uint8_t uLeft, uRight;
		do
		{
			uLeft = static_cast< std::uint8_t >( CharToLower( static_cast< std::uint8_t >( *szLeft++ ) ) );
			uRight = static_cast< std::uint8_t >( CharToLower( static_cast< std::uint8_t >( *szRight++ ) ) );

			if (uLeft == '\0')
				break;
		} while (uLeft == uRight);

		return uLeft - uRight;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr int StringCompareN( const C* tszLeft, const C* tszRight, std::size_t nCount )
	{
		using ComparisonType_t = std::conditional_t<std::is_same_v<C, char>, std::uint8_t, std::conditional_t<sizeof( wchar_t ) == 2U, std::int16_t, std::int32_t>>;

		ComparisonType_t nLeft, nRight;
		while (nCount--)
		{
			nLeft = static_cast< ComparisonType_t >( *tszLeft++ );
			nRight = static_cast< ComparisonType_t >( *tszRight++ );

			if (nLeft != nRight)
				return nLeft - nRight;

			if (nLeft == C( '\0' ))
				break;
		}

		return 0;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringChar( const C* tszSource, const int iSearch )
	{
		while (*tszSource != C( '\0' ))
		{
			if (*tszSource == iSearch)
				return const_cast< C* >( tszSource );

			++tszSource;
		}

		return nullptr;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringCharR( const C* tszSource, const int iSearch )
	{
		C* tszLastOccurrence = nullptr;

		do
		{
			if (*tszSource == iSearch)
				tszLastOccurrence = const_cast< C* >( tszSource );
		} while (*tszSource++ != C( '\0' ));

		return tszLastOccurrence;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringString( const C* tszSource, const C* tszSearch )
	{
		while (*tszSource != C( '\0' ))
		{
			while (*tszSearch != C( '\0' ) && ( *tszSource - *tszSearch ) == C( '\0' ))
			{
				++tszSource;
				++tszSearch;
			}

			if (*tszSearch == C( '\0' ))
				return const_cast< C* >( tszSource );

			++tszSource;
		}

		return nullptr;
	}

	constexpr char* StringStringI( const char* szSource, const char* szSearch )
	{
		while (*szSource != '\0')
		{
			while (*szSearch != '\0' && CharToLower( static_cast< std::uint8_t >( *szSource ) ) == CharToLower( static_cast< std::uint8_t >( *szSearch ) ))
			{
				++szSource;
				++szSearch;
			}

			if (*szSearch == '\0')
				return const_cast< char* >( szSource );

			++szSource;
		}

		return nullptr;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringCopy( C* tszDestination, const C* tszSource )
	{
		while (*tszSource != C( '\0' ))
			*tszDestination++ = *tszSource++;

		*tszDestination = C( '\0' );
		return tszDestination;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringCopyN( C* tszDestination, const C* tszSource, std::size_t nCount )
	{
		while (nCount--)
			*tszDestination++ = ( *tszSource != C( '\0' ) ? *tszSource++ : C( '\0' ) );

		return tszDestination;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringCat( C* tszDestination, const C* tszSource )
	{
		while (*tszDestination != C( '\0' ))
			++tszDestination;

		while (*tszSource != C( '\0' ))
			*tszDestination++ = *tszSource++;

		*tszDestination = C( '\0' );
		return tszDestination;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		constexpr C* StringCatN( C* tszDestination, const C* tszSource, std::size_t nCount )
	{
		while (*tszDestination != C( '\0' ))
			++tszDestination;

		while (*tszSource != C( '\0' ) && nCount--)
			*tszDestination++ = *tszSource++;

		*tszDestination = C( '\0' );
		return tszDestination;
	}

#pragma endregion

#pragma region crt_string_conversion

	constexpr char* StringToUpper( char* szDestination )
	{
		char* szDestinationOut = szDestination;

		while (*szDestinationOut++ != '\0')
			*szDestinationOut = CharToUpper( static_cast< std::uint8_t >( *szDestinationOut ) );

		return szDestination;
	}

	constexpr char* StringToLower( char* szDestination )
	{
		char* szDestinationOut = szDestination;

		while (*szDestinationOut++ != '\0')
			*szDestinationOut = CharToLower( static_cast< std::uint8_t >( *szDestinationOut ) );

		return szDestination;
	}

	template <typename T> requires std::is_integral_v<T>
	char* IntegerToString( const T value, char* szDestination, const std::size_t nDestinationLength, int iBase = 10 )
	{
		if (iBase < 0 || iBase == 1 || iBase > _NUMBER_MAX_BASE)
		{
			CS_ASSERT( false );
			return szDestination;
		}

		const bool bIsPositive = ( value >= 0 );
		std::make_unsigned_t<T> uValue = ( bIsPositive ? static_cast< std::make_unsigned_t<T> >( value ) : static_cast< std::make_unsigned_t<T> >( 0 - value ) );

		char* szDestinationEnd = szDestination + nDestinationLength;
		*--szDestinationEnd = '\0';

		if (uValue == 0U)
			*--szDestinationEnd = '0';

		else if (iBase == 10)
		{
			while (uValue >= 100U)
			{
				const char* szTwoDigits = &_TWO_DIGITS_LUT[ ( uValue % 100U ) * 2U ];
				*--szDestinationEnd = szTwoDigits[ 1 ];
				*--szDestinationEnd = szTwoDigits[ 0 ];
				uValue /= 100U;
			}

			if (uValue < 10U)
				*--szDestinationEnd = _NUMBER_ALPHA[ uValue ];
			else
			{
				const char* szTwoDigits = &_TWO_DIGITS_LUT[ uValue * 2U ];
				*--szDestinationEnd = szTwoDigits[ 1 ];
				*--szDestinationEnd = szTwoDigits[ 0 ];
			}

			if (!bIsPositive)
				*--szDestinationEnd = '-';
		}

		else if (iBase == 16)
		{
			while (uValue >= 0x100)
			{
				const char* szTwoDigits = &_TWO_DIGITS_HEX_LUT[ ( uValue % 0x100 ) * 2U ];
				*--szDestinationEnd = szTwoDigits[ 1 ];
				*--szDestinationEnd = szTwoDigits[ 0 ];
				uValue /= 0x100;
			}

			if (uValue < 0x10)
				*--szDestinationEnd = _NUMBER_ALPHA[ uValue ];
			else
			{
				const char* szTwoDigits = &_TWO_DIGITS_HEX_LUT[ uValue * 2U ];
				*--szDestinationEnd = szTwoDigits[ 1 ];
				*--szDestinationEnd = szTwoDigits[ 0 ];
			}
		}

		else
		{
			while (uValue > 0U)
			{
				*--szDestinationEnd = _NUMBER_ALPHA[ uValue % iBase ];
				uValue /= iBase;
			}
		}

		return szDestinationEnd;
	}

	template <typename T> requires std::is_floating_point_v<T>
	char* FloatToString( const T value, char* szDestination, const std::size_t nDestinationSize, int iPrecision = std::numeric_limits<T>::digits10 )
	{

		StringPrintN( szDestination, nDestinationSize, "%.*f", iPrecision, value );
		return szDestination;
	}

	template <typename C> requires ( std::is_same_v<C, char> || std::is_same_v<C, wchar_t> )
		std::size_t TimeToString( C* tszDestination, const std::size_t nDestinationSize, const C* tszFormat, const std::tm* pTime )
	{

		constexpr const char* arrWeekdayNames[ 7 ] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

		constexpr const char* arrMonthNames[ 12 ] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

		C* tszDestinationOut = tszDestination;
		C* tszFormatCurrent = nullptr;

		while (*tszFormat != C( '\0' ) && nDestinationSize > 0U)
		{
			if (*tszFormat == C( '%' ))
			{
				tszFormatCurrent = const_cast< C* >( tszFormat );
				break;
			}

			*tszDestinationOut++ = *tszFormat++;
		}

		std::size_t nWroteCount = tszDestinationOut - tszDestination;

		if (tszFormatCurrent == nullptr)
			return tszDestinationOut - tszDestination;

		while (*tszFormatCurrent != C( '\0' ) && nWroteCount < nDestinationSize)
		{

			if (*tszFormatCurrent == C( '%' ))
			{
				++tszFormatCurrent;

				if (*tszFormatCurrent == C( '#' ) || *tszFormatCurrent == C( '0' ) || *tszFormatCurrent == C( 'E' ))
					++tszFormatCurrent;

				const C* tszDestinationBefore = tszDestinationOut;

				switch (*tszFormatCurrent)
				{
				case C( 'a' ):
				{
					const char* szWeekdayName = arrWeekdayNames[ pTime->tm_wday ];

					std::size_t i = 3U;
					while (i-- > 0U)
						*tszDestinationOut++ = static_cast< C >( szWeekdayName[ i ] );

					break;
				}
				case C( 'A' ):
				{
					const char* szWeekdayName = arrWeekdayNames[ pTime->tm_wday ];

					while (*szWeekdayName != C( '\0' ))
						*tszDestinationOut++ = static_cast< C >( *szWeekdayName++ );

					break;
				}
				case C( 'b' ):
				case C( 'h' ):
				{
					const char* szMonthName = arrMonthNames[ pTime->tm_mon ];

					std::size_t i = 3U;
					while (i-- > 0U)
						*tszDestinationOut++ = static_cast< C >( szMonthName[ i ] );

					break;
				}
				case C( 'B' ):
				{
					const char* szMonthName = arrMonthNames[ pTime->tm_mon ];

					while (*szMonthName != C( '\0' ))
						*tszDestinationOut++ = static_cast< C >( *szMonthName++ );

					break;
				}
				case C( 'c' ):
				{

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), L"%a %b %e %H:%M:%S %Y", pTime );
					else
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), "%a %b %e %H:%M:%S %Y", pTime );

					break;
				}
				case C( 'C' ):
				{
					const int iYear = pTime->tm_year + 1900;
					const int iCentury = iYear / 100;

					if (iYear >= -99 && iYear < 0)
					{
						*tszDestinationOut++ = C( '-' );
						*tszDestinationOut++ = C( '0' );
					}
					else if (iCentury >= 0 && iCentury < 100)
					{
						const char* szCenturyNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( iCentury ) * 2U ];
						*tszDestinationOut++ = static_cast< C >( *szCenturyNumber++ );
						*tszDestinationOut++ = static_cast< C >( *szCenturyNumber );
					}
					else
						*tszDestinationOut++ = static_cast< C >( iCentury % 10U + '0' );

					break;
				}
				case C( 'd' ):
				{
					const char* szMonthDayNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_mday ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szMonthDayNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szMonthDayNumber );
					break;
				}
				case C( 'D' ):
					[[fallthrough]];
				case C( 'x' ):
				{

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), L"%m/%d/%y", pTime );
					else
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), "%m/%d/%y", pTime );

					break;
				}
				case C( 'e' ):
				{
					unsigned int uMonthDay = pTime->tm_mday;

					const char* szMonthDayNumber = &_TWO_DIGITS_LUT[ uMonthDay * 2U ];
					*tszDestinationOut++ = static_cast< C >( uMonthDay < 10U ? ' ' : *szMonthDayNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szMonthDayNumber );
					break;
				}
				case C( 'F' ):
				{

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), L"%Y-%m-%d", pTime );
					else
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), "%Y-%m-%d", pTime );

					break;
				}
				case C( 'g' ):
					[[fallthrough]];
				case C( 'G' ):
					[[fallthrough]];
				case C( 'V' ):
				{
					unsigned int uYear = pTime->tm_year + 1900U;
					int iDays = ( pTime->tm_yday - ( pTime->tm_yday - pTime->tm_wday + 382 ) % 7 + 3 );

					if (iDays < 0)
					{
						uYear -= 1U;

						const int iPreviousYearEndDay = pTime->tm_yday + ( 365 + ( ( uYear & 3U ) == 0U && ( uYear % 100U != 0U || uYear % 400U == 0U ) ) );
						iDays = ( iPreviousYearEndDay - ( iPreviousYearEndDay - pTime->tm_wday + 382 ) % 7 + 3 );
					}
					else
					{
						const int iCurrentYearEndDay = pTime->tm_yday - ( 365 + ( ( uYear & 3U ) == 0U && ( uYear % 100U != 0U || uYear % 400U == 0U ) ) );
						const int iNextDays = ( iCurrentYearEndDay - ( iCurrentYearEndDay - pTime->tm_wday + 382 ) % 7 + 3 );

						if (iNextDays >= 0)
						{
							uYear += 1U;
							iDays = iNextDays;
						}
					}

					switch (*tszFormatCurrent)
					{
					case C( 'G' ):
					{
						const char* szCenturyNumber = &_TWO_DIGITS_LUT[ ( uYear / 100U ) * 2U ];
						*tszDestinationOut++ = static_cast< C >( *szCenturyNumber++ );
						*tszDestinationOut++ = static_cast< C >( *szCenturyNumber );
						[[fallthrough]];
					}
					case C( 'g' ):
					{
						const char* szYearNumber = &_TWO_DIGITS_LUT[ ( uYear % 100U ) * 2U ];
						*tszDestinationOut++ = static_cast< C >( *szYearNumber++ );
						*tszDestinationOut++ = static_cast< C >( *szYearNumber );
						break;
					}
					default:
					{
						const char* szWeekNumber = &_TWO_DIGITS_LUT[ ( iDays / 7U + 1U ) * 2U ];
						*tszDestinationOut++ = static_cast< C >( *szWeekNumber++ );
						*tszDestinationOut++ = static_cast< C >( *szWeekNumber );
						break;
					}
					}

					break;
				}
				case C( 'H' ):
				{
					const char* szHourNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_hour ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szHourNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szHourNumber );
					break;
				}
				case C( 'I' ):
				{
					unsigned int uHourFormat = pTime->tm_hour % 12U;
					uHourFormat = ( ( uHourFormat == 0U ) ? 12U : uHourFormat );

					const char* szHourNumber = &_TWO_DIGITS_LUT[ uHourFormat * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szHourNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szHourNumber );
					break;
				}
				case C( 'j' ):
				{
					unsigned int uYearDay = pTime->tm_yday + 1U;

					const char* szYearDayNumber = &_TWO_DIGITS_LUT[ uYearDay * 2U ];
					*tszDestinationOut++ = static_cast< C >( ( uYearDay / 100U ) % 10U + '0' );
					*tszDestinationOut++ = static_cast< C >( *szYearDayNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szYearDayNumber );
					break;
				}
				case C( 'm' ):
				{
					const char* szMonthNumber = &_TWO_DIGITS_LUT[ ( pTime->tm_mon + 1U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szMonthNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szMonthNumber );
					break;
				}
				case C( 'M' ):
				{
					const char* szMinuteNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_min ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szMinuteNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szMinuteNumber );
					break;
				}
				case C( 'n' ):
				{
					*tszDestinationOut++ = C( '\n' );
					break;
				}
				case C( 'p' ):
				{
					*tszDestinationOut++ = ( ( pTime->tm_hour < 12 ) ? C( 'A' ) : C( 'P' ) );
					*tszDestinationOut++ = C( 'M' );
					break;
				}
				case C( 'r' ):
				{

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), L"%I:%M:%S %p", pTime );
					else
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), "%I:%M:%S %p", pTime );

					break;
				}
				case C( 'R' ):
				{
					const char* szHourNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_hour ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szHourNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szHourNumber );

					const char* szMinuteNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_min ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szMinuteNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szMinuteNumber );
					break;
				}
				case C( 'S' ):
				{
					const char* szSecondNumber = &_TWO_DIGITS_LUT[ static_cast< unsigned int >( pTime->tm_sec ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szSecondNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szSecondNumber );
					break;
				}
				case C( 't' ):
				{
					*tszDestinationOut++ = C( '\t' );
					break;
				}
				case C( 'T' ):
					[[fallthrough]];
				case C( 'X' ):
				{

					if constexpr (std::is_same_v<C, wchar_t>)
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), L"%H:%M:%S", pTime );
					else
						tszDestinationOut += TimeToString( tszDestinationOut, nDestinationSize - ( tszDestinationOut - tszDestination ), "%H:%M:%S", pTime );

					break;
				}
				case C( 'u' ):
				{
					*tszDestinationOut++ = static_cast< C >( ( ( pTime->tm_wday + 6U ) % 7U + 1U ) + '0' );
					break;
				}
				case C( 'U' ):
				{
					const char* szWeekNumber = &_TWO_DIGITS_LUT[ ( ( pTime->tm_yday - pTime->tm_wday + 7U ) / 7U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szWeekNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szWeekNumber );
					break;
				}
				case C( 'w' ):
				{
					*tszDestinationOut++ = static_cast< C >( pTime->tm_wday + '0' );
					break;
				}
				case C( 'W' ):
				{
					const char* szWeekNumber = &_TWO_DIGITS_LUT[ ( ( ( pTime->tm_yday - pTime->tm_wday - 6U ) % 7U + 7U ) / 7U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szWeekNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szWeekNumber );
					break;
				}
				case C( 'y' ):
				{
					const char* szYearNumber = &_TWO_DIGITS_LUT[ ( ( pTime->tm_year + 1900U ) % 100U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szYearNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szYearNumber );
					break;
				}
				case C( 'Y' ):
				{
					unsigned int uYear = pTime->tm_year + 1900U;

					const char* szCenturyNumber = &_TWO_DIGITS_LUT[ ( uYear / 100U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szCenturyNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szCenturyNumber );

					const char* szYearNumber = &_TWO_DIGITS_LUT[ ( uYear % 100U ) * 2U ];
					*tszDestinationOut++ = static_cast< C >( *szYearNumber++ );
					*tszDestinationOut++ = static_cast< C >( *szYearNumber );
					break;
				}
				case C( 'z' ):

					break;
				case C( 'Z' ):
				{

					break;
				}
				case C( '%' ):
				{
					*tszDestinationOut++ = *tszFormatCurrent;
					break;
				}
				default:
					CS_ASSERT( false );
					break;
				}

				nWroteCount += tszDestinationOut - tszDestinationBefore;

				++tszFormatCurrent;
				continue;
			}

			*tszDestinationOut++ = *tszFormatCurrent++;
			++nWroteCount;
		}

		if (nWroteCount > nDestinationSize)
			return 0U;

		*tszDestinationOut = C( '\0' );
		return nWroteCount;
	}

	template <typename T> requires ( std::is_integral_v<T> )
		constexpr T StringToInteger( const char* szSourceBegin, char** pszSourceEnd = nullptr, int iBase = 0 )
	{
		if (iBase < 0 || iBase == 1 || iBase > _NUMBER_MAX_BASE)
		{
			CS_ASSERT( false );
			return 0;
		}

		const char* szSourceCurrent = szSourceBegin;

		while (IsSpace( *szSourceCurrent ))
			++szSourceCurrent;

		const bool bIsPositive = ( ( *szSourceCurrent == '+' || *szSourceCurrent == '-' ) ? ( *szSourceCurrent++ == '+' ) : true );
		constexpr bool bIsUnsigned = ( ( std::numeric_limits<T>::min )( ) == 0U );

		if (iBase > 0)
		{

			if (iBase == 16 && *szSourceCurrent == '0' && ( szSourceCurrent[ 1 ] | ( 'a' ^ 'A' ) ) == 'x')
				szSourceCurrent += 2;
		}

		else if (*szSourceCurrent == '0')
		{
			if (( ( *szSourceCurrent++ ) | ( 'a' ^ 'A' ) ) == 'x')
			{

				iBase = 16;
				++szSourceCurrent;
			}
			else

				iBase = 8;
		}
		else

			iBase = 10;

		constexpr std::uint64_t ullNegativeMax = ( bIsUnsigned ? ( std::numeric_limits<T>::max )( ) : ( static_cast< std::uint64_t >( ( std::numeric_limits<T>::max )( ) ) + 1ULL ) );
		const std::uint64_t ullAbsoluteMax = ( bIsPositive ? ( std::numeric_limits<T>::max )( ) : ullNegativeMax );
		const std::uint64_t ullAbsoluteMaxOfBase = ullAbsoluteMax / iBase;

		bool bIsNumber = false;
		std::uint64_t ullResult = 0ULL;

		for (bool bIsDigit = false, bIsAlpha = false; ( ( bIsDigit = IsDigit( *szSourceCurrent ) ) ) || ( ( bIsAlpha = IsAlpha( *szSourceCurrent ) ) );)
		{
			int iCurrentDigit = 0;

			if (bIsDigit)
				iCurrentDigit = *szSourceCurrent - '0';
			else if (bIsAlpha)
				iCurrentDigit = ( *szSourceCurrent | ( 'a' ^ 'A' ) ) - 'a' + 0xA;

			if (iCurrentDigit >= iBase)
				break;

			bIsNumber = true;
			++szSourceCurrent;

			if (ullResult == ullAbsoluteMax)
			{
				CS_ASSERT( false );
				continue;
			}

			if (ullResult <= ullAbsoluteMaxOfBase)
				ullResult *= iBase;
			else
			{
				CS_ASSERT( false );
				ullResult = ullAbsoluteMax;
			}

			if (ullResult <= ullAbsoluteMax - iCurrentDigit)
				ullResult += iCurrentDigit;
			else
			{
				CS_ASSERT( false );
				ullResult = ullAbsoluteMax;
			}
		}

		if (pszSourceEnd != nullptr)
			*pszSourceEnd = const_cast< char* >( bIsNumber ? szSourceCurrent : szSourceBegin );

		if (ullResult == ullAbsoluteMax)
			return ( ( bIsPositive || bIsUnsigned ) ? ( std::numeric_limits<T>::max )( ) : ( std::numeric_limits<T>::min )( ) );

		return ( bIsPositive ? static_cast< T >( ullResult ) : -static_cast< T >( ullResult ) );
	}

#pragma endregion

#pragma region crt_string_encode_decode

	template <typename C>
	std::ptrdiff_t CharMultiByteToUTF32( const C* tszBegin, const C* tszEnd, std::uint32_t* puOutChar )
	{

		constexpr std::uint8_t arrSequenceLength[ 32 ] = {
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			0, 0, 0, 0, 0, 0, 0, 0,
			2, 2, 2, 2,
			3, 3,
			4, 0
		};

		constexpr std::uint8_t arrMask[ ] = { 0x00, 0x7F, 0x1F, 0x0F, 0x07 };
		constexpr std::uint32_t arrStartCodePoint[ ] = { 0x400000, 0, 0x80, 0x800, 0x10000 };
		constexpr std::uint8_t arrCharShift[ ] = { 0, 18, 12, 6, 0 };
		constexpr std::uint8_t arrErrorShift[ ] = { 0, 6, 4, 2, 0 };

		const std::uint8_t nNextLength = arrSequenceLength[ *reinterpret_cast< const std::uint8_t* >( tszBegin ) >> 3U ];
		std::ptrdiff_t nLength = nNextLength + !nNextLength;

		unsigned char uString[ 4 ] = {};
		const std::ptrdiff_t nSourceLength = tszEnd - tszBegin;

		uString[ 0 ] = nSourceLength > 0 ? tszBegin[ 0 ] : 0;
		uString[ 1 ] = nSourceLength > 1 ? tszBegin[ 1 ] : 0;
		uString[ 2 ] = nSourceLength > 2 ? tszBegin[ 2 ] : 0;
		uString[ 3 ] = nSourceLength > 3 ? tszBegin[ 3 ] : 0;

		*puOutChar = static_cast< std::uint32_t >( uString[ 0 ] & arrMask[ nNextLength ] ) << 18U;
		*puOutChar |= static_cast< std::uint32_t >( uString[ 1 ] & 0x3F ) << 12U;
		*puOutChar |= static_cast< std::uint32_t >( uString[ 2 ] & 0x3F ) << 6U;
		*puOutChar |= static_cast< std::uint32_t >( uString[ 3 ] & 0x3F );
		*puOutChar >>= arrCharShift[ nNextLength ];

		unsigned int uError = ( *puOutChar < arrStartCodePoint[ nNextLength ] ) << 6U;
		uError |= ( ( *puOutChar >> 11U ) == 0x1B ) << 7U;
		uError |= ( *puOutChar > 0x10FFFF ) << 8U;
		uError |= ( uString[ 1 ] & 0xC0 ) >> 2U;
		uError |= ( uString[ 2 ] & 0xC0 ) >> 4U;
		uError |= ( uString[ 3 ] ) >> 6U;
		uError ^= 0x2A;
		uError >>= arrErrorShift[ nNextLength ];

		if (uError > 0U)
		{

			nLength = std::min( nLength, static_cast< std::ptrdiff_t >( !!uString[ 0 ] + !!uString[ 1 ] + !!uString[ 2 ] + !!uString[ 3 ] ) );
			*puOutChar = 0xFFFD;
		}

		return nLength;
	}

	inline std::ptrdiff_t CharMultiByteFromUTF32( char* szOutBuffer, const std::size_t nOutBufferSize, const std::uint32_t uChar )
	{

		if (uChar < 0x80)
		{
			szOutBuffer[ 0 ] = static_cast< char >( uChar );
			return 1;
		}

		if (uChar < 0x800 && nOutBufferSize >= 2U)
		{
			szOutBuffer[ 0 ] = static_cast< char >( 0xC0 + ( uChar >> 6U ) );
			szOutBuffer[ 1 ] = static_cast< char >( 0x80 + ( uChar & 0x3F ) );
			return 2;
		}

		if (uChar < 0x10000 && nOutBufferSize >= 3U)
		{
			szOutBuffer[ 0 ] = static_cast< char >( 0xE0 + ( uChar >> 12U ) );
			szOutBuffer[ 1 ] = static_cast< char >( 0x80 + ( ( uChar >> 6U ) & 0x3F ) );
			szOutBuffer[ 2 ] = static_cast< char >( 0x80 + ( uChar & 0x3F ) );
			return 3;
		}

		if (uChar <= 0x10FFFF && nOutBufferSize >= 4U)
		{
			szOutBuffer[ 0 ] = static_cast< char >( 0xF0 + ( uChar >> 18U ) );
			szOutBuffer[ 1 ] = static_cast< char >( 0x80 + ( ( uChar >> 12U ) & 0x3F ) );
			szOutBuffer[ 2 ] = static_cast< char >( 0x80 + ( ( uChar >> 6U ) & 0x3F ) );
			szOutBuffer[ 3 ] = static_cast< char >( 0x80 + ( uChar & 0x3F ) );
			return 4;
		}

		return 0;
	}

	template <typename C>
	int StringLengthMultiByte( const C* tszBegin, const C* tszEnd = nullptr )
	{
		int nOctetCount = 0;

		while (*tszBegin != C( '\0' ) && ( tszEnd == nullptr || tszBegin < tszEnd ))
		{
			if (const std::uint32_t uChar = static_cast< std::uint32_t >( *tszBegin++ ); uChar < 0x80)
				nOctetCount += 1;
			else if (uChar < 0x800)
				nOctetCount += 2;
			else if (uChar < 0x10000)
				nOctetCount += 3;
			else if (uChar <= 0x7FFFFFFF)
				nOctetCount += 4;
		}

		return nOctetCount;
	}

	template <typename C>
	int StringLengthUnicode( const C* tszBegin, const C* tszEnd )
	{
		int nCharCount = 0;

		std::uint32_t uChar = 0U;
		while (*tszBegin != C( '\0' ) && tszBegin < tszEnd)
		{
			tszBegin += CharMultiByteToUTF32( tszBegin, tszEnd, &uChar );

			if (uChar == 0U)
				break;

			++nCharCount;
		}

		return nCharCount;
	}

	__forceinline std::ptrdiff_t StringMultiByteToUnicode( wchar_t* szOutBuffer, const std::size_t nOutBufferLength, const char* szBegin, const char* szEnd )
	{
		wchar_t* pBufferBegin = szOutBuffer;
		const wchar_t* pBufferEnd = szOutBuffer + nOutBufferLength;

		std::uint32_t uChar = 0U;
		while (pBufferBegin < pBufferEnd - 1 && szBegin < szEnd && *szBegin != '\0')
		{
			szBegin += CharMultiByteToUTF32( szBegin, szEnd, &uChar );

			if (uChar == 0U)
				break;

			*pBufferBegin++ = static_cast< wchar_t >( uChar );
		}

		*pBufferBegin = 0U;
		return pBufferBegin - szOutBuffer;
	}

	__forceinline std::ptrdiff_t StringUnicodeToMultiByte( char* szOutBuffer, const std::size_t nOutBufferLength, const wchar_t* wszBegin, const wchar_t* wszEnd = nullptr )
	{
		char* pBufferBegin = szOutBuffer;
		const char* pBufferEnd = szOutBuffer + nOutBufferLength;

		while (pBufferBegin < pBufferEnd - 1 && ( wszEnd == nullptr || wszBegin < wszEnd ) && *wszBegin != L'\0')
			pBufferBegin += CharMultiByteFromUTF32( pBufferBegin, pBufferEnd - pBufferBegin - 1, *wszBegin++ );

		*pBufferBegin = '\0';
		return pBufferBegin - szOutBuffer;
	}

	__forceinline std::wstring StringMultiByteToUnicode( const std::string_view strMultiByte )
	{
		const std::size_t nLength = StringLengthUnicode( strMultiByte.data( ), strMultiByte.data( ) + strMultiByte.size( ) ) + 1U;
		std::wstring wstrUnicodeOut( nLength, L'\0' );

		StringMultiByteToUnicode( wstrUnicodeOut.data( ), nLength, strMultiByte.data( ), strMultiByte.data( ) + strMultiByte.size( ) );
		return wstrUnicodeOut;
	}

	__forceinline std::string StringUnicodeToMultiByte( const std::wstring_view wstrUnicode )
	{
		const std::size_t nLength = StringLengthMultiByte( wstrUnicode.data( ), wstrUnicode.data( ) + wstrUnicode.size( ) ) + 1U;
		std::string strMultiByteOut( nLength, '\0' );

		StringUnicodeToMultiByte( strMultiByteOut.data( ), nLength, wstrUnicode.data( ), wstrUnicode.data( ) + wstrUnicode.size( ) );
		return strMultiByteOut;
	}

#pragma endregion

#pragma region string_format

	inline CS_CRT_FORMAT_STRING_ATTRIBUTE( printf, 2, 3 ) int StringPrint( char* szBuffer, const char* const szFormat, ... )
	{
		va_list args;
		va_start( args, szFormat );

		const int iReturn = stbsp_vsprintf( szBuffer, szFormat, args );

		va_end( args );

		return iReturn;
	}

	__forceinline CS_CRT_FORMAT_STRING_ATTRIBUTE( printf, 3, 4 ) int StringPrintN( char* szBuffer, std::size_t nCount, const char* const szFormat, ... )
	{
		va_list args;
		va_start( args, szFormat );

		const int iReturn = stbsp_vsnprintf( szBuffer, static_cast< int >( nCount ), szFormat, args );

		va_end( args );

		if (iReturn >= static_cast< int >( nCount ))
			return iReturn;

		return iReturn;
	}

#pragma endregion

	template <size_t SIZE = _MAX_PATH>
	struct String_t
	{
		String_t( )
		{

			szBuffer[ 0 ] = '\0';
		}

		String_t( const char* szStr )
		{
			StringCopy( this->szBuffer, szStr );
		}

		String_t( const char* szFormat, ... )
		{
			va_list args;
			va_start( args, szFormat );

			[[maybe_unused]] const int iReturn = stbsp_vsnprintf( this->szBuffer, static_cast< int >( sizeof( this->szBuffer ) ), szFormat, args );

			va_end( args );
		}

		String_t( const char* tszFormat, const std::tm* pTime )
		{
			TimeToString( this->szBuffer, sizeof( this->szBuffer ), tszFormat, pTime );
		}

		String_t( const wchar_t* wszStr )
		{
			FromMultiByte( wszStr );
		}

		~String_t( ) = default;

		const char* Format( const char* szFormat, ... )
		{
			va_list args;
			va_start( args, szFormat );

			[[maybe_unused]] const int iReturn = stbsp_vsnprintf( this->szBuffer, static_cast< int >( sizeof( this->szBuffer ) ), szFormat, args );

			va_end( args );

			return this->Data( );
		}

		[[nodiscard]] const std::size_t Length( ) const
		{
			return StringLength( this->szBuffer );
		}

		[[nodiscard]] const std::size_t Length( )
		{
			return StringLength( this->szBuffer );
		}

		[[nodiscard]] const std::size_t Size( ) const
		{
			return CS_ARRAYSIZE( this->szBuffer );
		}

		[[nodiscard]] const std::size_t Size( )
		{
			return CS_ARRAYSIZE( this->szBuffer );
		}

		[[nodiscard]] const bool IsEmpty( ) const
		{
			return szBuffer[ 0 ] == '\0';
		}

		[[nodiscard]] const char* Data( ) const
		{
			return this->szBuffer;
		}

		[[nodiscard]] const char* Data( )
		{
			return this->szBuffer;
		}

		[[nodiscard]] const int Compare( const char* szData, bool bCaseSensitive = false ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return bCaseSensitive ? StringCompareI( this->Data( ), szData ) : StringCompare( this->Data( ), szData );
		}

		[[nodiscard]] const int CompareN( const char* szSearch ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return StringCompareN( this->Data( ), szSearch, this->Size( ) );
		}

		[[nodiscard]] const char* Find( const char* szSearch, bool bCaseSensitive = false ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return bCaseSensitive ? StringStringI( this->Data( ), szSearch ) : StringString( this->Data( ), szSearch );
		}

		[[nodiscard]] const char* Append( const char* szData ) const
		{
			return StringCat( this->szBuffer, szData );
		}

		[[nodiscard]] const char* AppendN( const char* szData ) const
		{
			return StringCatN( this->szBuffer, szData, this->Size( ) );
		}

		[[nodiscard]] const char* ToUpper( ) const
		{
			CS_ASSERT( IsEmpty( ) );
			return StringToUpper( this->szBuffer );
		}

		[[nodiscard]] const char* ToLower( ) const
		{
			CS_ASSERT( IsEmpty( ) );
			return StringToLower( this->szBuffer );
		}

		[[nodiscard]] const wchar_t* ToMultiByte( ) const
		{
			wchar_t* szOutBuffer = new wchar_t[ this->Length( ) + 1U ];
			StringMultiByteToUnicode( szOutBuffer, this->Length( ) + 1U, this->szBuffer, this->szBuffer + this->Length( ) + 1U );

			return szOutBuffer;
		}

		void FromMultiByte( const wchar_t* wszData )
		{
			StringUnicodeToMultiByte( this->szBuffer, this->Length( ), wszData, wszData + this->Length( ) );
		}

		void Clear( ) noexcept
		{
			MemorySet( szBuffer, 0U, sizeof( szBuffer ) );
		}

		char szBuffer[ SIZE ];
	};

	template <size_t SIZE = _MAX_PATH>
	struct WString_t
	{
		WString_t( )
		{

			wszBuffer[ 0 ] = L'\0';
		}

		WString_t( const wchar_t* wszStr )
		{
			StringCopy( this->wszBuffer, wszStr );
		}

		WString_t( const char* szStr )
		{
			FromUnicode( szStr );
		}

		[[nodiscard]] const std::size_t Length( ) const
		{
			return StringLengthMultiByte( this->wszBuffer );
		}

		[[nodiscard]] const std::size_t Length( )
		{
			return StringLengthMultiByte( this->wszBuffer );
		}

		[[nodiscard]] const std::size_t Size( ) const
		{
			return CS_ARRAYSIZE( this->wszBuffer );
		}

		[[nodiscard]] const std::size_t Size( )
		{
			return CS_ARRAYSIZE( this->wszBuffer );
		}

		[[nodiscard]] const bool IsEmpty( ) const
		{
			return wszBuffer[ 0 ] == '\0';
		}

		[[nodiscard]] const wchar_t* Data( ) const
		{
			return this->wszBuffer;
		}

		[[nodiscard]] const wchar_t* Data( )
		{
			return this->wszBuffer;
		}

		[[nodiscard]] const int Compare( const wchar_t* szData, bool bCaseSensitive = false ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return bCaseSensitive ? StringCompareI( this->Data( ), szData ) : StringCompare( this->Data( ), szData );
		}

		[[nodiscard]] const int CompareN( const wchar_t* szSearch ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return StringCompareN( this->Data( ), szSearch, this->Size( ) );
		}

		[[nodiscard]] const char* Find( const wchar_t* szSearch, bool bCaseSensitive = false ) const
		{
			CS_ASSERT( IsEmpty( ) );

			return bCaseSensitive ? StringStringI( this->Data( ), szSearch ) : StringString( this->Data( ), szSearch );
		}

		[[nodiscard]] const char* Append( const wchar_t* szData ) const
		{
			return StringCat( this->wszBuffer, szData );
		}

		[[nodiscard]] const char* AppendN( const wchar_t* szData ) const
		{
			return StringCatN( this->wszBuffer, szData, this->Size( ) );
		}

		[[nodiscard]] const char* ToUpper( ) const
		{
			CS_ASSERT( IsEmpty( ) );
			return StringToUpper( this->wszBuffer );
		}

		[[nodiscard]] const char* ToLower( ) const
		{
			CS_ASSERT( IsEmpty( ) );
			return StringToLower( this->wszBuffer );
		}

		const char* ToUnicode( )
		{
			char* szOutBuffer = new char[ this->Length( ) + 1U ];
			StringUnicodeToMultiByte( szOutBuffer, this->Length( ) + 1U, this->wszBuffer, this->wszBuffer + this->Length( ) + 1U );

			return szOutBuffer;
		}

		void FromUnicode( const char* szStr )
		{
			StringMultiByteToUnicode( this->wszBuffer, this->Length( ), szStr, szStr + this->Length( ) );
		}

		wchar_t wszBuffer[ SIZE ];
	};
}