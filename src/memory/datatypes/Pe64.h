#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#pragma region winapi_nt_types
using NTSTATUS = LONG;
#pragma endregion

#pragma region winapi_nt_definitions
#define NtCurrentProcess() (reinterpret_cast<HANDLE>(-1))
#define NtCurrentThread() (reinterpret_cast<HANDLE>(-2))

#define NT_SUCCESS(STATUS) (static_cast<NTSTATUS>(STATUS) >= 0)
#define NT_INFORMATION(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 1UL)
#define NT_WARNING(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 2UL)
#define NT_ERROR(STATUS) ((static_cast<ULONG>(STATUS) >> 30UL) == 3UL)
#pragma endregion

#pragma region winapi_nt

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	WCHAR* Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

static_assert(sizeof(_UNICODE_STRING) == 0x10);

struct _RTL_BALANCED_NODE
{
	union
	{
		struct _RTL_BALANCED_NODE* Children[2];

		struct
		{
			struct _RTL_BALANCED_NODE* Left;
			struct _RTL_BALANCED_NODE* Right;
		};
	};

	union
	{
		struct
		{
			UCHAR Red : 1;
			UCHAR Balance : 2;
		};

		ULONGLONG ParentValue;
	};
};

static_assert(sizeof(_RTL_BALANCED_NODE) == 0x18);

struct _LDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;
	struct _LIST_ENTRY InMemoryOrderLinks;
	struct _LIST_ENTRY InInitializationOrderLinks;
	VOID* DllBase;
	VOID* EntryPoint;
	ULONG SizeOfImage;
	struct _UNICODE_STRING FullDllName;
	struct _UNICODE_STRING BaseDllName;

	union
	{
		UCHAR FlagGroup[4];
		ULONG Flags;

		struct
		{
			ULONG PackagedBinary : 1;
			ULONG MarkedForRemoval : 1;
			ULONG ImageDll : 1;
			ULONG LoadNotificationsSent : 1;
			ULONG TelemetryEntryProcessed : 1;
			ULONG ProcessStaticImport : 1;
			ULONG InLegacyLists : 1;
			ULONG InIndexes : 1;
			ULONG ShimDll : 1;
			ULONG InExceptionTable : 1;
			ULONG ReservedFlags1 : 2;
			ULONG LoadInProgress : 1;
			ULONG LoadConfigProcessed : 1;
			ULONG EntryProcessed : 1;
			ULONG ProtectDelayLoad : 1;
			ULONG ReservedFlags3 : 2;
			ULONG DontCallForThreads : 1;
			ULONG ProcessAttachCalled : 1;
			ULONG ProcessAttachFailed : 1;
			ULONG CorDeferredValidate : 1;
			ULONG CorImage : 1;
			ULONG DontRelocate : 1;
			ULONG CorILOnly : 1;
			ULONG ChpeImage : 1;
			ULONG ChpeEmulatorImage : 1;
			ULONG ReservedFlags5 : 1;
			ULONG Redirected : 1;
			ULONG ReservedFlags6 : 2;
			ULONG CompatDatabaseProcessed : 1;
		};
	};

	USHORT ObsoleteLoadCount;
	USHORT TlsIndex;
	struct _LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
	VOID* Lock;
	struct _LDR_DDAG_NODE* DdagNode;
	struct _LIST_ENTRY NodeModuleLink;
	struct _LDRP_LOAD_CONTEXT* LoadContext;
	VOID* ParentDllBase;
	VOID* SwitchBackContext;
	_RTL_BALANCED_NODE BaseAddressIndexNode;
	_RTL_BALANCED_NODE MappingInfoIndexNode;
	ULONGLONG OriginalBase;
	union _LARGE_INTEGER LoadTime;
	ULONG BaseNameHashValue;
	enum _LDR_DLL_LOAD_REASON LoadReason;
	ULONG ImplicitPathOptions;
	ULONG ReferenceCount;
	ULONG DependentLoadFlags;
	UCHAR SigningLevel;
	ULONG CheckSum;
	VOID* ActivePatchImageBase;
	enum _LDR_HOT_PATCH_STATE HotPatchState;
};

static_assert(sizeof(_LDR_DATA_TABLE_ENTRY) == 0x138);

struct _PEB_LDR_DATA
{
	ULONG Length;
	UCHAR Initialized;
	VOID* SsHandle;
	struct _LIST_ENTRY InLoadOrderModuleList;
	struct _LIST_ENTRY InMemoryOrderModuleList;
	struct _LIST_ENTRY InInitializationOrderModuleList;
	VOID* EntryInProgress;
	UCHAR ShutdownInProgress;
	VOID* ShutdownThreadId;
};

static_assert(sizeof(_PEB_LDR_DATA) == 0x58);

struct _CURDIR
{
	struct _UNICODE_STRING DosPath;
	VOID* Handle;
};

static_assert(sizeof(_CURDIR) == 0x18);

struct _STRING
{
	USHORT Length;
	USHORT MaximumLength;
	CHAR* Buffer;
};

static_assert(sizeof(_STRING) == 0x10);

struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	struct _STRING DosPath;
};

static_assert(sizeof(_RTL_DRIVE_LETTER_CURDIR) == 0x18);

struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	VOID* ConsoleHandle;
	ULONG ConsoleFlags;
	VOID* StandardInput;
	VOID* StandardOutput;
	VOID* StandardError;
	struct _CURDIR CurrentDirectory;
	struct _UNICODE_STRING DllPath;
	struct _UNICODE_STRING ImagePathName;
	struct _UNICODE_STRING CommandLine;
	VOID* Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	struct _UNICODE_STRING WindowTitle;
	struct _UNICODE_STRING DesktopInfo;
	struct _UNICODE_STRING ShellInfo;
	struct _UNICODE_STRING RuntimeData;
	struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
	ULONGLONG EnvironmentSize;
	ULONGLONG EnvironmentVersion;
	VOID* PackageDependencyData;
	ULONG ProcessGroupId;
	ULONG LoaderThreads;
	struct _UNICODE_STRING RedirectionDllName;
	struct _UNICODE_STRING HeapPartitionName;
	ULONGLONG* DefaultThreadpoolCpuSetMasks;
	ULONG DefaultThreadpoolCpuSetMaskCount;
	ULONG DefaultThreadpoolThreadMaximum;
	ULONG HeapMemoryTypeMask;
};

static_assert(sizeof(_RTL_USER_PROCESS_PARAMETERS) == 0x448);

struct _PEB
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;

	union
	{
		UCHAR BitField;

		struct
		{
			UCHAR ImageUsesLargePages : 1;
			UCHAR IsProtectedProcess : 1;
			UCHAR IsImageDynamicallyRelocated : 1;
			UCHAR SkipPatchingUser32Forwarders : 1;
			UCHAR IsPackagedProcess : 1;
			UCHAR IsAppContainer : 1;
			UCHAR IsProtectedProcessLight : 1;
			UCHAR IsLongPathAwareProcess : 1;
		};
	};

	UCHAR Padding0[4];
	VOID* Mutant;
	VOID* ImageBaseAddress;
	struct _PEB_LDR_DATA* Ldr;
	struct _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
	VOID* SubSystemData;
	VOID* ProcessHeap;
	struct _RTL_CRITICAL_SECTION* FastPebLock;
	union _SLIST_HEADER* volatile AtlThunkSListPtr;
	VOID* IFEOKey;

	union
	{
		ULONG CrossProcessFlags;

		struct
		{
			ULONG ProcessInJob : 1;
			ULONG ProcessInitializing : 1;
			ULONG ProcessUsingVEH : 1;
			ULONG ProcessUsingVCH : 1;
			ULONG ProcessUsingFTH : 1;
			ULONG ProcessPreviouslyThrottled : 1;
			ULONG ProcessCurrentlyThrottled : 1;
			ULONG ProcessImagesHotPatched : 1;
			ULONG ReservedBits0 : 24;
		};
	};

	UCHAR Padding1[4];

	union
	{
		VOID* KernelCallbackTable;
		VOID* UserSharedInfoPtr;
	};

	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	VOID* ApiSetMap;
	ULONG TlsExpansionCounter;
	UCHAR Padding2[4];
	struct _RTL_BITMAP* TlsBitmap;
	ULONG TlsBitmapBits[2];
	VOID* ReadOnlySharedMemoryBase;
	VOID* SharedData;
	VOID** ReadOnlyStaticServerData;
	VOID* AnsiCodePageData;
	VOID* OemCodePageData;
	VOID* UnicodeCaseTableData;
	ULONG NumberOfProcessors;
	ULONG NtGlobalFlag;
	union _LARGE_INTEGER CriticalSectionTimeout;
	ULONGLONG HeapSegmentReserve;
	ULONGLONG HeapSegmentCommit;
	ULONGLONG HeapDeCommitTotalFreeThreshold;
	ULONGLONG HeapDeCommitFreeBlockThreshold;
	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	VOID** ProcessHeaps;
	VOID* GdiSharedHandleTable;
	VOID* ProcessStarterHelper;
	ULONG GdiDCAttributeList;
	UCHAR Padding3[4];
	struct _RTL_CRITICAL_SECTION* LoaderLock;
	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	USHORT OSBuildNumber;
	USHORT OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	UCHAR Padding4[4];
	ULONGLONG ActiveProcessAffinityMask;
	ULONG GdiHandleBuffer[60];
	VOID(*PostProcessInitRoutine)
		();
	struct _RTL_BITMAP* TlsExpansionBitmap;
	ULONG TlsExpansionBitmapBits[32];
	ULONG SessionId;
	UCHAR Padding5[4];
	union _ULARGE_INTEGER AppCompatFlags;
	union _ULARGE_INTEGER AppCompatFlagsUser;
	VOID* pShimData;
	VOID* AppCompatInfo;
	struct _UNICODE_STRING CSDVersion;
	struct _ACTIVATION_CONTEXT_DATA* ActivationContextData;
	struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;
	struct _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;
	struct _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;
	ULONGLONG MinimumStackCommit;
	VOID* SparePointers[2];
	VOID* PatchLoaderData;
	struct _CHPEV2_PROCESS_INFO* ChpeV2ProcessInfo;
	ULONG AppModelFeatureState;
	ULONG SpareUlongs[2];
	USHORT ActiveCodePage;
	USHORT OemCodePage;
	USHORT UseCaseMapping;
	USHORT UnusedNlsField;
	VOID* WerRegistrationData;
	VOID* WerShipAssertPtr;
	VOID* EcCodeBitMap;
	VOID* pImageHeaderHash;

	union
	{
		ULONG TracingFlags;

		struct
		{
			ULONG HeapTracingEnabled : 1;
			ULONG CritSecTracingEnabled : 1;
			ULONG LibLoaderTracingEnabled : 1;
			ULONG SpareTracingBits : 29;
		};
	};

	UCHAR Padding6[4];
	ULONGLONG CsrServerReadOnlySharedMemoryBase;
	ULONGLONG TppWorkerpListLock;
	struct _LIST_ENTRY TppWorkerpList;
	VOID* WaitOnAddressHashTable[128];
	VOID* TelemetryCoverageHeader;
	ULONG CloudFileFlags;
	ULONG CloudFileDiagFlags;
	CHAR PlaceholderCompatibilityMode;
	CHAR PlaceholderCompatibilityModeReserved[7];
	struct _LEAP_SECOND_DATA* LeapSecondData;

	union
	{
		ULONG LeapSecondFlags;

		struct
		{
			ULONG SixtySecondEnabled : 1;
			ULONG Reserved : 31;
		};
	};

	ULONG NtGlobalFlag2;
	ULONGLONG ExtendedFeatureDisableMask;
};

static_assert(sizeof(_PEB) == 0x7d0);

struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
};

static_assert(sizeof(_CLIENT_ID) == 0x10);

struct _GDI_TEB_BATCH
{
	ULONG Offset : 31;
	ULONG HasRenderingCommand : 1;
	ULONGLONG HDC;
	ULONG Buffer[310];
};

static_assert(sizeof(_GDI_TEB_BATCH) == 0x4E8);

struct _ACTIVATION_CONTEXT_STACK
{
	struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* ActiveFrame;
	struct _LIST_ENTRY FrameListCache;
	ULONG Flags;
	ULONG NextCookieSequenceNumber;
	ULONG StackId;
};

static_assert(sizeof(_ACTIVATION_CONTEXT_STACK) == 0x28);

struct _TEB
{
	struct _NT_TIB NtTib;
	VOID* EnvironmentPointer;
	struct _CLIENT_ID ClientId;
	VOID* ActiveRpcHandle;
	VOID* ThreadLocalStoragePointer;
	struct _PEB* ProcessEnvironmentBlock;
	ULONG LastErrorValue;
	ULONG CountOfOwnedCriticalSections;
	VOID* CsrClientThread;
	VOID* Win32ThreadInfo;
	ULONG User32Reserved[26];
	ULONG UserReserved[5];
	VOID* WOW32Reserved;
	ULONG CurrentLocale;
	ULONG FpSoftwareStatusRegister;
	VOID* ReservedForDebuggerInstrumentation[16];
	VOID* SystemReserved1[30];
	CHAR PlaceholderCompatibilityMode;
	UCHAR PlaceholderHydrationAlwaysExplicit;
	CHAR PlaceholderReserved[10];
	ULONG ProxiedProcessId;
	struct _ACTIVATION_CONTEXT_STACK _ActivationStack;
	UCHAR WorkingOnBehalfTicket[8];
	LONG ExceptionCode;
	UCHAR Padding0[4];
	struct _ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer;
	ULONGLONG InstrumentationCallbackSp;
	ULONGLONG InstrumentationCallbackPreviousPc;
	ULONGLONG InstrumentationCallbackPreviousSp;
	ULONG TxFsContext;
	UCHAR InstrumentationCallbackDisabled;
	UCHAR UnalignedLoadStoreExceptions;
	UCHAR Padding1[2];
	struct _GDI_TEB_BATCH GdiTebBatch;
	struct _CLIENT_ID RealClientId;
	VOID* GdiCachedProcessHandle;
	ULONG GdiClientPID;
	ULONG GdiClientTID;
	VOID* GdiThreadLocalInfo;
	ULONGLONG Win32ClientInfo[62];
	VOID* glDispatchTable[233];
	ULONGLONG glReserved1[29];
	VOID* glReserved2;
	VOID* glSectionInfo;
	VOID* glSection;
	VOID* glTable;
	VOID* glCurrentRC;
	VOID* glContext;
	ULONG LastStatusValue;
	UCHAR Padding2[4];
	struct _UNICODE_STRING StaticUnicodeString;
	WCHAR StaticUnicodeBuffer[261];
	UCHAR Padding3[6];
	VOID* DeallocationStack;
	VOID* TlsSlots[64];
	struct _LIST_ENTRY TlsLinks;
	VOID* Vdm;
	VOID* ReservedForNtRpc;
	VOID* DbgSsReserved[2];
	ULONG HardErrorMode;
	UCHAR Padding4[4];
	VOID* Instrumentation[11];
	struct _GUID ActivityId;
	VOID* SubProcessTag;
	VOID* PerflibData;
	VOID* EtwTraceData;
	VOID* WinSockData;
	ULONG GdiBatchCount;

	union
	{
		struct _PROCESSOR_NUMBER CurrentIdealProcessor;
		ULONG IdealProcessorValue;

		struct
		{
			UCHAR ReservedPad0;
			UCHAR ReservedPad1;
			UCHAR ReservedPad2;
			UCHAR IdealProcessor;
		};
	};

	ULONG GuaranteedStackBytes;
	UCHAR Padding5[4];
	VOID* ReservedForPerf;
	VOID* ReservedForOle;
	ULONG WaitingOnLoaderLock;
	UCHAR Padding6[4];
	VOID* SavedPriorityState;
	ULONGLONG ReservedForCodeCoverage;
	VOID* ThreadPoolData;
	VOID** TlsExpansionSlots;
	struct _CHPEV2_CPUAREA_INFO* ChpeV2CpuAreaInfo;
	VOID* Unused;
	ULONG MuiGeneration;
	ULONG IsImpersonating;
	VOID* NlsCache;
	VOID* pShimData;
	ULONG HeapData;
	UCHAR Padding7[4];
	VOID* CurrentTransactionHandle;
	struct _TEB_ACTIVE_FRAME* ActiveFrame;
	VOID* FlsData;
	VOID* PreferredLanguages;
	VOID* UserPrefLanguages;
	VOID* MergedPrefLanguages;
	ULONG MuiImpersonation;

	union
	{
		volatile USHORT CrossTebFlags;
		USHORT SpareCrossTebBits : 16;
	};

	union
	{
		USHORT SameTebFlags;

		struct
		{
			USHORT SafeThunkCall : 1;
			USHORT InDebugPrint : 1;
			USHORT HasFiberData : 1;
			USHORT SkipThreadAttach : 1;
			USHORT WerInShipAssertCode : 1;
			USHORT RanProcessInit : 1;
			USHORT ClonedThread : 1;
			USHORT SuppressDebugMsg : 1;
			USHORT DisableUserStackWalk : 1;
			USHORT RtlExceptionAttached : 1;
			USHORT InitialThread : 1;
			USHORT SessionAware : 1;
			USHORT LoadOwner : 1;
			USHORT LoaderWorker : 1;
			USHORT SkipLoaderInit : 1;
			USHORT SkipFileAPIBrokering : 1;
		};
	};

	VOID* TxnScopeEnterCallback;
	VOID* TxnScopeExitCallback;
	VOID* TxnScopeContext;
	ULONG LockCount;
	LONG WowTebOffset;
	VOID* ResourceRetValue;
	VOID* ReservedForWdf;
	ULONGLONG ReservedForCrt;
	struct _GUID EffectiveContainerId;
	ULONGLONG LastSleepCounter;
	ULONG SpinCallCount;
	UCHAR Padding8[4];
	ULONGLONG ExtendedFeatureDisableMask;
};

static_assert(sizeof(_TEB) == 0x1850);
#pragma endregion