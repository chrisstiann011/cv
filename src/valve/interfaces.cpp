#include "../Precompiled.h"

using InstantiateInterfaceFn_t = void* (*)();
class CInterfaceRegister
{
public:
	InstantiateInterfaceFn_t fnCreate;
	const char* szName;
	CInterfaceRegister* pNext;
};

static const CInterfaceRegister* GetRegisterList(const wchar_t* wszModuleName)
{
	void* hModule = Memory::GetModuleBaseHandle(wszModuleName);
	if (hModule == nullptr)
		return nullptr;

	std::uint8_t* pCreateInterface = reinterpret_cast<std::uint8_t*>(Memory::GetExportAddress(hModule, X("CreateInterface")));

	if (pCreateInterface == nullptr)
	{
		Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
		Logging::Print(X("[ error ] failed to get CreateInterface address"));
		Logging::PopConsoleColor();
		return nullptr;
	}

	return *reinterpret_cast<CInterfaceRegister**>(Memory::ResolveRelativeAddress(pCreateInterface, 0x3, 0x7));
}

template <typename T = void*>
T* Capture(const CInterfaceRegister* pModuleRegister, const char* szInterfaceName)
{
	for (const CInterfaceRegister* pRegister = pModuleRegister; pRegister != nullptr; pRegister = pRegister->pNext)
	{
		if (const std::size_t nInterfaceNameLength = CRT::StringLength(szInterfaceName);

			CRT::StringCompareN(szInterfaceName, pRegister->szName, nInterfaceNameLength) == 0 &&

			(CRT::StringLength(pRegister->szName) == nInterfaceNameLength ||

				CRT::StringToInteger<int>(pRegister->szName + nInterfaceNameLength, nullptr, 10) > 0))
		{

			void* pInterface = pRegister->fnCreate();

#ifdef _DEBUG

			Logging::PushConsoleColor(FOREGROUND_WHITE);
			Logging::Print(X("Captured interface: {} -> {:#08X}"), pRegister->szName, reinterpret_cast<std::uintptr_t>(pInterface));
			Logging::PopConsoleColor();
#endif

			return static_cast<T*>(pInterface);
		}
	}

	Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
	Logging::Print(X("[ error ] failed to find interface: {}"), szInterfaceName);
	Logging::PopConsoleColor();
	return nullptr;
}

CGameParticleManager* Interfaces::GetGameParticleManager()
{
	return *reinterpret_cast<CGameParticleManager**>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("48 8B 0D ? ? ? ? 41 B8 ? ? ? ? F3 0F 11 74 24 ? 48 C7 44 24 ? ? ? ? ?")), 0x3, 0x7));
}

static CParticleCollection* GetParticleCollection()
{
	return *reinterpret_cast<CParticleCollection**>(Memory::FindPattern(PARTICLES_DLL, X(" 48 89 5C 24 ? 57 48 83 EC ? 0F 28 05")));
}
IDXGISwapChain* CreateDummySwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = 1;
	sd.BufferDesc.Height = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = GetForegroundWindow();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context);

	if (FAILED(hr))
	{
		return nullptr;
	}

	if (device) device->Release();
	if (context) context->Release();

	return swapChain;
}
bool Interfaces::Setup()
{
	bool bSuccess = true;
	const auto pTier0Handle = Memory::GetModuleBaseHandle(TIER0_DLL);
	if (pTier0Handle == nullptr)
		return false;
	m_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(pTier0Handle, X("g_pMemAlloc")));
	bSuccess &= (m_pMemAlloc != nullptr);

	const auto pLocalizeHandle = GetRegisterList(LOCALIZE_DLL);
	if (pLocalizeHandle == nullptr)
		return false;

	m_pLocalize = Capture<CLocalize>(pLocalizeHandle, LOCALIZE);
	bSuccess &= (m_pLocalize != nullptr);

	const auto pFileSystemHandle = GetRegisterList(FILESYSTEM_STDIO_DLL);
	if (pFileSystemHandle == nullptr)
		return false;

	m_pFileSystem = Capture<CBaseFileSystem>(pFileSystemHandle, FILE_SYSTEM);
	bSuccess &= (m_pFileSystem != nullptr);

	const auto pSchemaSystemRegisterList = GetRegisterList(SCHEMASYSTEM_DLL);
	if (pSchemaSystemRegisterList == nullptr)
		return false;

	m_pSchemaSystem = Capture<ISchemaSystem>(pSchemaSystemRegisterList, SCHEMA_SYSTEM);
	bSuccess &= (m_pSchemaSystem != nullptr);

	const auto pInputSystemRegisterList = GetRegisterList(INPUTSYSTEM_DLL);
	if (pInputSystemRegisterList == nullptr)
		return false;

	m_pInputSystem = Capture<IInputSystem>(pInputSystemRegisterList, INPUT_SYSTEM_VERSION);
	bSuccess &= (m_pInputSystem != nullptr);

	const auto pEngineRegisterList = GetRegisterList(ENGINE2_DLL);
	if (pEngineRegisterList == nullptr)
		return false;

	m_pGameResourceService = Capture<IGameResourceService>(pEngineRegisterList, GAME_RESOURCE_SERVICE_CLIENT);
	bSuccess &= (m_pGameResourceService != nullptr);

	m_pEngine = Capture<IEngineClient>(pEngineRegisterList, SOURCE2_ENGINE_TO_CLIENT);
	bSuccess &= (m_pEngine != nullptr);

	m_pNetworkClientService = Capture<INetworkClientService>(pEngineRegisterList, NETWORK_CLIENT_SERVICE);
	bSuccess &= (m_pNetworkClientService != nullptr);

	m_pGameUIServices = Capture<IGameUIServices>(pEngineRegisterList, GAME_UI_SERVICE);
	bSuccess &= (m_pGameUIServices != nullptr);

	m_pInputService = Capture<IInputService>(pEngineRegisterList, INPUT_SERVICE);
	bSuccess &= (m_pInputService != nullptr);

	const auto pTier0RegisterList = GetRegisterList(TIER0_DLL);
	if (pTier0RegisterList == nullptr)
		return false;

	m_pCvar = Capture<IEngineCVar>(pTier0RegisterList, ENGINE_CVAR);
	bSuccess &= (m_pCvar != nullptr);

	const auto pClientRegister = GetRegisterList(CLIENT_DLL);
	if (pClientRegister == nullptr)
		return false;

	m_pClient = Capture<ISource2Client>(pClientRegister, SOURCE2_CLIENT);
	bSuccess &= (m_pClient != nullptr);

	m_pPrediction = Capture<CPrediction>(pClientRegister, SOURCE2_CLIENT_PREDICTION);
	bSuccess &= (m_pPrediction != nullptr);

	const auto pMaterialSystem2Register = GetRegisterList(MATERIAL_SYSTEM2_DLL);
	if (pMaterialSystem2Register == nullptr)
		return false;

	m_pMaterialSystem2 = Capture<IMaterialSystem2>(pMaterialSystem2Register, MATERIAL_SYSTEM2);
	bSuccess &= (m_pMaterialSystem2 != nullptr);

	const auto pSceneSystemRegister = GetRegisterList(SCENESYSTEM_DLL);
	if (pSceneSystemRegister == nullptr)
		return false;

	m_pSceneSystem = Capture<ISceneSystem>(pSceneSystemRegister, SCENE_SYSTEM);
	bSuccess &= (m_pSceneSystem != nullptr);

	const auto pMeshSystemRegister = GetRegisterList(L"meshsystem.dll");
	if (pMeshSystemRegister == nullptr)
		return false;

	m_pMeshSystem = Capture<CMeshSystem>(pMeshSystemRegister, "MeshSystem001");
	bSuccess &= (m_pMeshSystem != nullptr);

	const auto pResourceSystemRegisterList = GetRegisterList(RESOURCESYSTEM_DLL);
	if (pResourceSystemRegisterList == nullptr)
		return false;

	m_pResourceSystem = Capture<IResourceSystem>(pResourceSystemRegisterList, RESOURCE_SYSTEM);
	bSuccess &= (m_pResourceSystem != nullptr);

	if (m_pResourceSystem != nullptr)
	{
		m_pResourceHandleUtils = reinterpret_cast<CResourceHandleUtils*>(m_pResourceSystem->QueryInterface(RESOURCE_HANDLE_UTILS));
		bSuccess &= (m_pResourceHandleUtils != nullptr);
	}

	const auto pParticlesRegisterList = GetRegisterList(PARTICLES_DLL);
	if (pParticlesRegisterList == nullptr)
		return false;

	m_pParticleSystemMgr = Capture<IParticleSystemMgr>(pParticlesRegisterList, PARTICLE_SYSTEM_MANAGER);
	bSuccess &= (m_pParticleSystemMgr != nullptr);
	m_IDXGISwapChain = reinterpret_cast<IDXGISwapChain*>(Memory::FindPattern(GAMEOVERLAYRENDERER64_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8")));
	bSuccess &= (m_IDXGISwapChain != nullptr);
	auto pseudom_swap_chain = CreateDummySwapChain();
	if (!pseudom_swap_chain)
		return false;
	Dummy_Swapchain = pseudom_swap_chain;

	m_pSwapChain = **reinterpret_cast<ISwapChainDx11***>(pseudom_swap_chain);
	bSuccess &= (m_pSwapChain != nullptr);

	m_pInput = *reinterpret_cast<CCSGOInput**>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("48 8B 0D ? ? ? ? 4C 8B C6 8B 10 E8")), 0x3, 0x7));
	bSuccess &= (m_pInput != nullptr);
	std::cout << "m_pInput: " << m_pInput;

	m_pGlobalVariables = *reinterpret_cast<IGlobalVariables**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 89 15 ? ? ? ? 48 89 42")), 0x3));
	bSuccess &= (m_pGlobalVariables != nullptr);
	std::cout << "m_pGlobalVariables: " << m_pGlobalVariables;

	m_pEntityList = reinterpret_cast<std::uintptr_t*>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("4C 8B 0D ? ? ? ? 4D 85 C9 74 ? 41 83 F8 ? 74 ? 41 8B C8 81 E1 ? ? ? ? 8B C1 C1 E8 ? 4D 8B 14 C1")), 0x3, 0x7));

	m_pClientMode = reinterpret_cast<IClientMode*>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 8D 0D ? ? ? ? 48 69 C0 ? ? ? ? 48 03 C1 C3 CC CC")), 0x3, 0x0));
	bSuccess &= (m_pClientMode != nullptr);
	std::cout << "m_pClientMode: " << m_pClientMode;

	m_pViewRender = *reinterpret_cast<IViewRender**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 89 05 ? ? ? ? 48 8B C8 48 85 C0")), 0x3, 0x0));
	bSuccess &= (m_pViewRender != nullptr);

	m_pVPhys2World = *reinterpret_cast<CVPhys2World**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("4C 8B 25 ? ? ? ? 24")), 0x3, 0x0));
	bSuccess &= (m_pVPhys2World != nullptr);

	m_pPVSManager = reinterpret_cast<CEnginePVSManager*>(Memory::ResolveRelativeAddress(Memory::FindPattern(ENGINE2_DLL, X("48 8D 0D ? ? ? ? 33 D2 FF 50")), 0x3, 0x7));
	bSuccess &= (m_pPVSManager != nullptr);

	m_pGameEventManager = *reinterpret_cast<IGameEventManager2**>(Memory::ResolveRelativeAddress(Memory::GetVFunc<std::uint8_t*>(m_pClient, 14U) + 0x3E, 0x3, 0x7));
	bSuccess &= (m_pGameEventManager != nullptr);

	m_pGameRules = *reinterpret_cast<C_CSGameRules**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 8B 1D ? ? ? ? 48 8D 54 24 ? 0F 28 D0 48 8D 4C 24 ?")), 0x3));
	bSuccess &= (m_pGameRules != nullptr);

	const void* hSteamAPI = Memory::GetModuleBaseHandle(STEAM_API64_DLL);
	bSuccess &= (hSteamAPI != nullptr);

	m_pSteamClient = reinterpret_cast<std::add_pointer_t<ISteamClient * ()>>(Memory::GetExportAddress(hSteamAPI, X("SteamClient")))();
	bSuccess &= (m_pSteamClient != nullptr);

	const HSteamUser hSteamUser = reinterpret_cast<std::add_pointer_t<HSteamUser()>>(Memory::GetExportAddress(hSteamAPI, X("SteamAPI_GetHSteamUser")))();
	bSuccess &= (hSteamUser != NULL);

	const HSteamPipe hSteamPipe = reinterpret_cast<std::add_pointer_t<HSteamPipe()>>(Memory::GetExportAddress(hSteamAPI, X("SteamAPI_GetHSteamPipe")))();
	bSuccess &= (hSteamUser != NULL);

	m_pSteamGameCoordinator = static_cast<ISteamGameCoordinator*>(m_pSteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, STEAMGAMECOORDINATOR_INTERFACE_VERSION));
	bSuccess &= (m_pSteamGameCoordinator != nullptr);

	m_pSteamFriends = m_pSteamClient->GetISteamFriends(hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION);
	bSuccess &= (m_pSteamFriends != nullptr);

	m_pSteamUtils = m_pSteamClient->GetISteamUtils(hSteamUser, STEAMUTILS_INTERFACE_VERSION);
	bSuccess &= (m_pSteamUtils != nullptr);

	m_pSteamMatchmaking = m_pSteamClient->GetISteamMatchmaking(hSteamUser, hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION);
	bSuccess &= (m_pSteamMatchmaking != nullptr);

	m_pParticleManager = GetGameParticleManager();
	bSuccess &= (m_pParticleManager != nullptr);

	return bSuccess;
}

void Interfaces::CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
	if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&m_pDevice)))
	{
		Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
		Logging::Print(X("[ error ] failed to get device from swapchain"));
		Logging::PopConsoleColor();
		CS_ASSERT(false);
	}
	else

		m_pDevice->GetImmediateContext(&m_pDeviceContext);

	static const auto GetCorrectDXGIFormat = [](DXGI_FORMAT eCurrentFormat)
		{
			switch (eCurrentFormat)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			return eCurrentFormat;
		};

	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);

	ID3D11Texture2D* pBackBuffer = nullptr;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
	{
		if (pBackBuffer)
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = static_cast<DXGI_FORMAT>(GetCorrectDXGIFormat(sd.BufferDesc.Format));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, &desc, &m_pRenderTargetView)))
			{
				Logging::PushConsoleColor(FOREGROUND_INTENSE_YELLOW);
				Logging::Print(X("[ warning ] failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D..."));
				Logging::Print(X("[ info ] retrying to create render target view with D3D11_RTV_DIMENSION_TEXTURE2DMS..."));
				Logging::PopConsoleColor();
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, &desc, &m_pRenderTargetView)))
				{
					Logging::PushConsoleColor(FOREGROUND_INTENSE_YELLOW);
					Logging::Print(X("[ warning ] failed to create render target view with D3D11_RTV_DIMENSION_TEXTURE2D..."));
					Logging::Print(X("[ info ] retrying..."));
					Logging::PopConsoleColor();

					if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
					{
						Logging::PushConsoleColor(FOREGROUND_RED);
						Logging::Print(X("[ error ] failed to create render target view"));
						Logging::PopConsoleColor();
						CS_ASSERT(false);
					}
				}
			}
			pBackBuffer->Release();
			pBackBuffer = nullptr;
		}
	}
}

void Interfaces::DestroyRenderTarget()
{
	if (m_pRenderTargetView != nullptr)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = nullptr;
	}
}