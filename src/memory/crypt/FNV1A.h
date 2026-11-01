#pragma once

#include <Windows.h>

using FNV1A_t = std::uint64_t;

namespace FNV1A
{
	
	constexpr FNV1A_t ullBasis = 0xCBF29CE484222325ULL;
	constexpr FNV1A_t ullPrime = 0x100000001B3ULL;

	consteval FNV1A_t HashConst( const char* szString, const FNV1A_t uKey = ullBasis ) noexcept
	{
		return ( szString[ 0 ] == '\0' ) ? uKey : HashConst( &szString[ 1 ], ( uKey ^ static_cast< FNV1A_t >( szString[ 0 ] ) ) * ullPrime );
	}

	inline FNV1A_t Hash( const char* szString, FNV1A_t uKey = ullBasis ) noexcept
	{
		if ( !szString )
			return uKey;
#pragma warning( push )
#pragma warning( disable : 28159 )
		if ( IsBadReadPtr( szString, 1 ) )
			return uKey;
#pragma warning( pop )
		const std::size_t nLength = CRT::StringLengthN( szString, 0x10000U );
		for ( std::size_t i = 0U; i < nLength; ++i )
		{
			uKey ^= static_cast<FNV1A_t>( szString[ i ] );
			uKey *= ullPrime;
		}
		return uKey;
	}
}