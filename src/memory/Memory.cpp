#include "../Precompiled.h"

bool Memory::Setup( )
{
	bool bSuccess = true;

	void* ntHandle = GetModuleBaseHandle(X(L"ntdll.dll"));
	NtProtectVirtualMemory = (tNtProtectVirtualMemory)GetExportAddress(ntHandle, X("NtProtectVirtualMemory"));
	NtQueryVirtualMemory = (tNtQueryVirtualMemory)GetExportAddress(ntHandle, X("NtQueryVirtualMemory"));
	bSuccess &= (NtProtectVirtualMemory != nullptr && NtQueryVirtualMemory != nullptr);

	const void* hDbgHelp = GetModuleBaseHandle(DBGHELP_DLL);
	const void* hTier0 = GetModuleBaseHandle(TIER0_DLL);
	if (!hDbgHelp || !hTier0)
		return false;

	fnUnDecorateSymbolName = reinterpret_cast<decltype(fnUnDecorateSymbolName)>(GetExportAddress(hDbgHelp, X("UnDecorateSymbolName")));
	bSuccess &= (fnUnDecorateSymbolName != nullptr);

	Functions::fnUtlBufferInit = reinterpret_cast<decltype(Functions::fnUtlBufferInit)>(GetExportAddress(hTier0, X("??0CUtlBuffer@@QEAA@HHW4BufferFlags_t@0@@Z")));
	bSuccess &= (Functions::fnUtlBufferInit != nullptr);

	Functions::fnUtlBufferPutString = reinterpret_cast<decltype(Functions::fnUtlBufferPutString)>(GetExportAddress(hTier0, X("?PutString@CUtlBuffer@@QEAAXPEBD@Z")));
	bSuccess &= (Functions::fnUtlBufferPutString != nullptr);

	Functions::fnUtlBufferEnsureCapacity = reinterpret_cast<decltype(Functions::fnUtlBufferEnsureCapacity)>(GetExportAddress(hTier0, X("?EnsureCapacity@CUtlBuffer@@QEAAXH@Z")));
	bSuccess &= (Functions::fnUtlBufferEnsureCapacity != nullptr);

	Functions::fnUtlStringSet = reinterpret_cast<decltype(Functions::fnUtlStringSet)>(GetExportAddress(hTier0, X("?Set@CUtlString@@QEAAXPEBD@Z")));
	bSuccess &= (Functions::fnUtlStringSet != nullptr);

	Functions::fnPurgeCBufferString = reinterpret_cast<decltype(Functions::fnPurgeCBufferString)>(GetExportAddress(hTier0, X("?Purge@CBufferString@@QEAAXH@Z")));
	bSuccess &= (Functions::fnPurgeCBufferString != nullptr);

	return bSuccess;
}

void* __cdecl operator new( const std::size_t nSize )
{
	if (!Interfaces::m_pMemAlloc)
	{
		const auto pTier0Handle = Memory::GetModuleBaseHandle(TIER0_DLL);
		Interfaces::m_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(pTier0Handle, X("g_pMemAlloc")));
	}

	return Interfaces::m_pMemAlloc->Alloc( nSize );
}

void* __cdecl operator new[]( const std::size_t nSize )
{
	if (!Interfaces::m_pMemAlloc)
	{
		const auto pTier0Handle = Memory::GetModuleBaseHandle(TIER0_DLL);
		Interfaces::m_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(pTier0Handle, X("g_pMemAlloc")));
	}

	return Interfaces::m_pMemAlloc->Alloc( nSize );
}

void __cdecl operator delete( void* pMemory ) noexcept
{
	if (!Interfaces::m_pMemAlloc)
	{
		const auto pTier0Handle = Memory::GetModuleBaseHandle(TIER0_DLL);
		Interfaces::m_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(pTier0Handle, X("g_pMemAlloc")));
	}

	Interfaces::m_pMemAlloc->Free( pMemory );
}

void __cdecl operator delete[]( void* pMemory ) noexcept
{
	if (!Interfaces::m_pMemAlloc)
	{
		const auto pTier0Handle = Memory::GetModuleBaseHandle(TIER0_DLL);
		Interfaces::m_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(pTier0Handle, X("g_pMemAlloc")));
	}

	Interfaces::m_pMemAlloc->Free( pMemory );
}
#pragma endregion

#pragma region memory_get

void* Memory::GetModuleBaseHandle( const wchar_t* wszModuleName )
{
	const _PEB* pPEB = reinterpret_cast< _PEB* >( __readgsqword( 0x60 ) );

	if (wszModuleName == nullptr)
		return pPEB->ImageBaseAddress;

	void* pModuleBase = nullptr;
	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InMemoryOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InMemoryOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD( pListEntry, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks );

		if (pEntry->FullDllName.Buffer != nullptr && CRT::StringCompare( wszModuleName, pEntry->BaseDllName.Buffer ) == 0)
		{
			pModuleBase = pEntry->DllBase;
			break;
		}
	}

	if (pModuleBase == nullptr)
	{

	}

	return pModuleBase;
}

const wchar_t* Memory::GetModuleBaseFileName( const void* hModuleBase )
{
	const _PEB* pPEB = reinterpret_cast< _PEB* >(LI_FN(__readgsqword)( 0x60 ) );

	if (hModuleBase == nullptr)
		hModuleBase = pPEB->ImageBaseAddress;

	LI_FN(::EnterCriticalSection)(pPEB->LoaderLock);

	const wchar_t* wszModuleName = nullptr;
	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InMemoryOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InMemoryOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD( pListEntry, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks );

		if (pEntry->DllBase == hModuleBase)
		{
			wszModuleName = pEntry->BaseDllName.Buffer;
			break;
		}
	}

	LI_FN(::LeaveCriticalSection)( pPEB->LoaderLock );

	return wszModuleName;
}

void* Memory::GetExportAddress( const void* hModuleBase, const char* szProcedureName )
{
	const auto pBaseAddress = static_cast< const std::uint8_t* >( hModuleBase );

	const auto pIDH = static_cast< const IMAGE_DOS_HEADER* >( hModuleBase );
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	const auto pINH = reinterpret_cast< const IMAGE_NT_HEADERS64* >( pBaseAddress + pIDH->e_lfanew );
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	const IMAGE_OPTIONAL_HEADER64* pIOH = &pINH->OptionalHeader;
	const std::uintptr_t nExportDirectorySize = pIOH->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;
	const std::uintptr_t uExportDirectoryAddress = pIOH->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress;

	if (nExportDirectorySize == 0U || uExportDirectoryAddress == 0U)
	{

		return nullptr;
	}

	const auto pIED = reinterpret_cast< const IMAGE_EXPORT_DIRECTORY* >( pBaseAddress + uExportDirectoryAddress );
	const auto pNamesRVA = reinterpret_cast< const std::uint32_t* >( pBaseAddress + pIED->AddressOfNames );
	const auto pNameOrdinalsRVA = reinterpret_cast< const std::uint16_t* >( pBaseAddress + pIED->AddressOfNameOrdinals );
	const auto pFunctionsRVA = reinterpret_cast< const std::uint32_t* >( pBaseAddress + pIED->AddressOfFunctions );

	std::size_t nRight = pIED->NumberOfNames, nLeft = 0U;
	while (nRight != nLeft)
	{

		const std::size_t uMiddle = nLeft + ( ( nRight - nLeft ) >> 1U );
		const int iResult = CRT::StringCompare( szProcedureName, reinterpret_cast< const char* >( pBaseAddress + pNamesRVA[ uMiddle ] ) );

		if (iResult == 0)
		{
			const std::uint32_t uFunctionRVA = pFunctionsRVA[ pNameOrdinalsRVA[ uMiddle ] ];

			if (uFunctionRVA >= uExportDirectoryAddress && uFunctionRVA - uExportDirectoryAddress < nExportDirectorySize)
			{

				break;
			}

			return const_cast< std::uint8_t* >( pBaseAddress ) + uFunctionRVA;
		}

		if (iResult > 0)
			nLeft = uMiddle + 1;
		else
			nRight = uMiddle;
	}

	Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
	Logging::Print( X( "[ error ] export not found: {}" ), szProcedureName );
	Logging::PopConsoleColor( );

	return nullptr;
}

bool Memory::GetSectionInfo( const void* hModuleBase, const char* szSectionName, std::uint8_t** ppSectionStart, std::size_t* pnSectionSize )
{
	const auto pBaseAddress = static_cast< const std::uint8_t* >( hModuleBase );

	const auto pIDH = static_cast< const IMAGE_DOS_HEADER* >( hModuleBase );
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	const auto pINH = reinterpret_cast< const IMAGE_NT_HEADERS* >( pBaseAddress + pIDH->e_lfanew );
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	const IMAGE_SECTION_HEADER* pISH = IMAGE_FIRST_SECTION( pINH );

	for (WORD i = 0U; i < pINH->FileHeader.NumberOfSections; i++, pISH++)
	{

		if (CRT::StringCompareN( szSectionName, reinterpret_cast< const char* >( pISH->Name ), IMAGE_SIZEOF_SHORT_NAME ) == 0)
		{
			if (ppSectionStart != nullptr)
				*ppSectionStart = const_cast< std::uint8_t* >( pBaseAddress ) + pISH->VirtualAddress;

			if (pnSectionSize != nullptr)
				*pnSectionSize = pISH->SizeOfRawData;

			return true;
		}
	}

	Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
	Logging::Print( X( "[ error ] code section not found: {}" ), szSectionName );
	Logging::PopConsoleColor( );

	return false;
}

std::uint8_t* Memory::FindPattern( const wchar_t* wszModuleName, const char* szPattern )
{

	const std::size_t nApproximateBufferSize = ( CRT::StringLength( szPattern ) >> 1U ) + 1U;
	std::uint8_t* arrByteBuffer = static_cast< std::uint8_t* >( MEM_STACKALLOC( nApproximateBufferSize ) );
	char* szMaskBuffer = static_cast< char* >( MEM_STACKALLOC( nApproximateBufferSize ) );
	PatternToBytes( szPattern, arrByteBuffer, szMaskBuffer );

	return FindPattern( wszModuleName, reinterpret_cast< const char* >( arrByteBuffer ), szMaskBuffer );
}

std::uint8_t* Memory::FindPattern( const wchar_t* wszModuleName, const char* szBytePattern, const char* szByteMask )
{
	const void* hModuleBase = GetModuleBaseHandle( wszModuleName );

	if (hModuleBase == nullptr)
	{

		return nullptr;
	}

	const auto pBaseAddress = static_cast< const std::uint8_t* >( hModuleBase );
	const auto pIDH = static_cast< const IMAGE_DOS_HEADER* >( hModuleBase );
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
		Logging::Print( X( "[ error ] failed to get module size, image is invalid" ) );
		Logging::PopConsoleColor( );
		return nullptr;
	}

	const auto pINH = reinterpret_cast< const IMAGE_NT_HEADERS* >( pBaseAddress + pIDH->e_lfanew );
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
	{
		Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
		Logging::Print( X( "[ error ] failed to get module size, image is invalid" ) );
		Logging::PopConsoleColor( );
		return nullptr;
	}

	const std::uint8_t* arrByteBuffer = reinterpret_cast< const std::uint8_t* >( szBytePattern );
	const std::size_t nByteCount = CRT::StringLength( szByteMask );

	std::uint8_t* pFoundAddress = nullptr;

	pFoundAddress = FindPatternEx( pBaseAddress, pINH->OptionalHeader.SizeOfImage, arrByteBuffer, nByteCount, szByteMask );

	if (pFoundAddress == nullptr)
	{
		char* szPattern = static_cast< char* >( MEM_STACKALLOC( ( nByteCount << 1U ) + nByteCount ) );
		[[maybe_unused]] const std::size_t nConvertedPatternLength = BytesToPattern( arrByteBuffer, nByteCount, szPattern );

		static std::unordered_set<std::string> s_loggedPatterns;
		if ( s_loggedPatterns.insert( szPattern ).second )
		{
			Logging::PushConsoleColor( FOREGROUND_INTENSE_RED );
			Logging::Print( X( "[ error ] pattern not found: {}" ), szPattern );
			Logging::PopConsoleColor( );
		}
		MEM_STACKFREE( szPattern );
	}

	return pFoundAddress;
}

std::uint8_t* Memory::FindPatternEx( const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask )
{
	std::uint8_t* pCurrentAddress = const_cast< std::uint8_t* >( pRegionStart );
	const std::uint8_t* pRegionEnd = pRegionStart + nRegionSize - nByteCount;
	const bool bIsMaskUsed = ( szByteMask != nullptr );

	while (pCurrentAddress < pRegionEnd)
	{

		if (( bIsMaskUsed && *szByteMask == '?' ) || *pCurrentAddress == *arrByteBuffer)
		{
			if (nByteCount == 1)
				return pCurrentAddress;

			std::size_t nComparedBytes = 0U;
			while (( bIsMaskUsed && szByteMask[ nComparedBytes + 1U ] == '?' ) || pCurrentAddress[ nComparedBytes + 1U ] == arrByteBuffer[ nComparedBytes + 1U ])
			{

				if (++nComparedBytes == nByteCount - 1U)
					return pCurrentAddress;
			}

			pCurrentAddress += nComparedBytes;
		}

		++pCurrentAddress;
	}

	return nullptr;
}

std::vector<std::uint8_t*> Memory::FindPatternAllOccurrencesEx( const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask )
{
	const std::uint8_t* pRegionEnd = pRegionStart + nRegionSize - nByteCount;
	const bool bIsMaskUsed = ( szByteMask != nullptr );

	std::vector<std::uint8_t*> vecOccurrences = {};

	for (std::uint8_t* pCurrentByte = const_cast< std::uint8_t* >( pRegionStart ); pCurrentByte < pRegionEnd; ++pCurrentByte)
	{

		if (( !bIsMaskUsed || *szByteMask != '?' ) && *pCurrentByte != *arrByteBuffer)
			continue;

		bool bSequenceMatch = true;
		for (std::size_t i = 1U; i < nByteCount; i++)
		{

			if (( !bIsMaskUsed || szByteMask[ i ] != '?' ) && pCurrentByte[ i ] != arrByteBuffer[ i ])
			{

				pCurrentByte += i - 1U;

				bSequenceMatch = false;
				break;
			}
		}

		if (bSequenceMatch)
			vecOccurrences.emplace_back( pCurrentByte );
	}

	return vecOccurrences;
}

std::size_t Memory::PatternToBytes( const char* szPattern, std::uint8_t* pOutByteBuffer, char* szOutMaskBuffer )
{
	std::uint8_t* pCurrentByte = pOutByteBuffer;

	while (*szPattern != '\0')
	{

		if (*szPattern == '?')
		{
			++szPattern;

			* pCurrentByte++ = 0U;
			*szOutMaskBuffer++ = '?';
		}

		else if (*szPattern != ' ')
		{

			std::uint8_t uByte = static_cast< std::uint8_t >( CRT::CharToHexInt( *szPattern ) << 4 );

			++szPattern;

			uByte |= static_cast< std::uint8_t >( CRT::CharToHexInt( *szPattern ) );

			*pCurrentByte++ = uByte;
			*szOutMaskBuffer++ = 'x';
		}

		++szPattern;
	}

	*pCurrentByte = 0U;
	*szOutMaskBuffer = '\0';

	return pCurrentByte - pOutByteBuffer;
}

std::size_t Memory::BytesToPattern( const std::uint8_t* pByteBuffer, const std::size_t nByteCount, char* szOutBuffer )
{
	char* szCurrentPattern = szOutBuffer;

	for (std::size_t i = 0U; i < nByteCount; i++)
	{

		const char* szHexByte = &CRT::_TWO_DIGITS_HEX_LUT[ pByteBuffer[ i ] * 2U ];
		*szCurrentPattern++ = szHexByte[ 0 ];
		*szCurrentPattern++ = szHexByte[ 1 ];
		*szCurrentPattern++ = ' ';
	}
	*--szCurrentPattern = '\0';

	return szCurrentPattern - szOutBuffer;
}

void* CS2ChannelManager::RegisterChannel(const char* szName)
{
	using fn_t = void* (*)(const char*, uintptr_t, int, int, uint32_t);
	static fn_t fn = (fn_t)Memory::GetExportAddress(Memory::GetModuleBaseHandle(L"tier0.dll"), "LoggingSystem_RegisterLoggingChannel");
	return fn(szName, 0, 0, 2, 0xFFFFFFFF);
}

void* CS2ChannelManager::GetChannel(const char* szName)
{
	static std::unordered_map<FNV1A_t, void*> mapChannels{};

	if (!mapChannels.contains(FNV1A::Hash(szName)))
		mapChannels.insert({ FNV1A::Hash(szName), CS2ChannelManager::RegisterChannel(szName) });

	return mapChannels[FNV1A::Hash(szName)];
}

void CS2ChannelManager::Msg(const char* szChannelName, const char* szMsg, Color nColor)
{
	void* pChannel = CS2ChannelManager::GetChannel(szChannelName);
	using fn_t = void* (*)(void*, int, Color, const char*);
	static fn_t fn = (fn_t)Memory::GetExportAddress(Memory::GetModuleBaseHandle(L"tier0.dll"), "?LoggingSystem_Log@@YA?AW4LoggingResponse_t@@HW4LoggingSeverity_t@@VColor@@PEBDZZ");
	fn(pChannel, 2, nColor, (std::string(szMsg) + "\n").c_str());
}