#pragma once

#pragma warning(push)
#pragma warning(disable: 6255)
#define MEM_STACKALLOC(SIZE) _malloca(SIZE)
#pragma warning(pop)
#define MEM_STACKFREE(MEMORY) static_cast<void>(0)

#define _CS_INTERNAL_CONCATENATE(LEFT, RIGHT) LEFT##RIGHT
#define CS_CONCATENATE(LEFT, RIGHT) _CS_INTERNAL_CONCATENATE(LEFT, RIGHT)

#define MEM_PAD(SIZE)										\
private:												\
	char CS_CONCATENATE(pad_0, __COUNTER__)[SIZE]; \
public:

#define MEM_PAD_VTABLE (COUNT)\
MEM_PAD(COUNT * sizeof(void*))

#define OFFSET( type, name, offset )                                                                                                                 \
    type& name ( ) {                                                                                                                                 \
        return *( type* )( std::uintptr_t ( this ) + offset );                                                                                       \
    }

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetInformation,
	MemoryMappedFilenameInformation,
	MemoryRegionInformation,
	MemoryWorkingSetExInformation,
	MemorySharedCommitInformation,
	MemoryImageInformation,
	MemoryRegionInformationEx,
	MemoryPrivilegedBasicInformation,
	MemoryEnclaveImageInformation,
	MemoryBasicInformationCapped,
	MemoryPhysicalContiguityInformation,
	MaxMemoryInfoClass
} MEMORY_INFORMATION_CLASS;

using tNtProtectVirtualMemory = long( NTAPI* )( HANDLE ProcessHandle, PVOID* BaseAddress, PULONG NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection );
using tNtQueryVirtualMemory = long( NTAPI* )( HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength );

class CKeyValues3;
class Color;
struct KV3ID_t;
namespace Memory
{
	bool Setup( );

	inline tNtProtectVirtualMemory NtProtectVirtualMemory;
	inline tNtQueryVirtualMemory NtQueryVirtualMemory;

	[[nodiscard]] void* GetModuleBaseHandle( const wchar_t* wszModuleName );

	[[nodiscard]] const wchar_t* GetModuleBaseFileName( const void* hModuleBase );

	[[nodiscard]] void* GetExportAddress( const void* hModuleBase, const char* szProcedureName );

	[[nodiscard]] bool GetSectionInfo( const void* hModuleBase, const char* szSectionName, std::uint8_t** ppSectionStart, std::size_t* pnSectionSize );

	template <typename T = std::uint8_t>
	[[nodiscard]] T* GetAbsoluteAddress( T* pRelativeAddress, int nPreOffset = 0x0, int nPostOffset = 0x0 )
	{
		if ( !pRelativeAddress )
			return nullptr;
		pRelativeAddress += nPreOffset;
		pRelativeAddress += sizeof( std::int32_t ) + *reinterpret_cast< std::int32_t* >( pRelativeAddress );
		pRelativeAddress += nPostOffset;
		return pRelativeAddress;
	}

	[[nodiscard]] __forceinline std::uint8_t* ResolveRelativeAddress( std::uint8_t* nAddressBytes, std::uint32_t nRVAOffset, std::uint32_t nRIPOffset )
	{
		if ( !nAddressBytes )
			return nullptr;
		std::uint32_t nRVA = *reinterpret_cast< std::uint32_t* >( nAddressBytes + nRVAOffset );
		std::uint64_t nRIP = reinterpret_cast< std::uint64_t >( nAddressBytes ) + nRIPOffset;

		return reinterpret_cast< std::uint8_t* >( nRVA + nRIP );
	}

	template <typename T = void*>
	[[nodiscard]] __forceinline T GetVFunc( const void* thisptr, std::size_t nIndex )
	{
		return ( *static_cast< T* const* >( thisptr ) )[ nIndex ];
	}

	template <typename T, std::size_t nIndex, class CBaseClass, typename... Args_t>
	static __forceinline T CallVFunc(CBaseClass* thisptr, Args_t... argList)
	{
		using VirtualFn_t = T( __thiscall* )( const void*, decltype( argList )... );
		return ( *reinterpret_cast< VirtualFn_t* const* >( reinterpret_cast< std::uintptr_t >( thisptr ) ) )[ nIndex ]( thisptr, argList... );
	}

	[[nodiscard]] std::uint8_t* FindPattern( const wchar_t* wszModuleName, const char* szPattern );

	[[nodiscard]] std::uint8_t* FindPattern( const wchar_t* wszModuleName, const char* szBytePattern, const char* szByteMask );

	[[nodiscard]] std::uint8_t* FindPatternEx( const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr );

	[[nodiscard]] std::vector<std::uint8_t*> FindPatternAllOccurrencesEx( const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask = nullptr );

	std::size_t PatternToBytes( const char* szPattern, std::uint8_t* pOutByteBuffer, char* szOutMaskBuffer );

	std::size_t BytesToPattern( const std::uint8_t* pByteBuffer, const std::size_t nByteCount, char* szOutBuffer );

	inline unsigned long( __stdcall* fnUnDecorateSymbolName )( const char* szName, char* pszOutput, unsigned long nMaxStringLength, unsigned long dwFlags ) = nullptr;

	inline int(__fastcall* fnLoggingSystemRegisterLoggingChannel)(const char*, std::uintptr_t, int, int, Color);
	inline bool(__fastcall* fnLoggingSystemIsChannelEnabled)(int, int);
	inline void(__fastcall* fnLoggingSystemLog)(int, int, const char*);
	inline void(__fastcall* fnLoggingSystemSetChannelColor)(int, Color);
}

namespace CS2ChannelManager
{
	void* RegisterChannel(const char* szName);
	void* GetChannel(const char* szName);
	void Msg(const char* szChannelName, const char* szMsg, Color nColor);
}