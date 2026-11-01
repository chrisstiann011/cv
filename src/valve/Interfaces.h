#pragma once

#define GAME_RESOURCE_SERVICE_CLIENT X("GameResourceServiceClientV00")
#define SOURCE2_CLIENT X("Source2Client00")
#define SOURCE2_CLIENT_PREDICTION X("Source2ClientPrediction00")
#define SCHEMA_SYSTEM X("SchemaSystem_00")
#define INPUT_SYSTEM_VERSION X("InputSystemVersion00")
#define INPUT_SERVICE X("InputService_00")
#define SOURCE2_ENGINE_TO_CLIENT X("Source2EngineToClient00")
#define ENGINE_CVAR X("VEngineCvar00")
#define LOCALIZE X("Localize_00")
#define NETWORK_CLIENT_SERVICE X("NetworkClientService_00")
#define MATERIAL_SYSTEM2 X("VMaterialSystem2_00")
#define RESOURCE_SYSTEM X("ResourceSystem013")
#define RESOURCE_HANDLE_UTILS X("ResourceHandleUtils001")
#define LOCALIZE X("Localize_00")
#define FILE_SYSTEM X("VFileSystem0")
#define GAME_UI_SERVICE X("GameUIService_00")
#define PARTICLE_SYSTEM_MANAGER X("ParticleSystemMgr00")
#define SCENE_SYSTEM X("SceneSystem_00")

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

class IMemAlloc;
class ISwapChainDx11;
class CCSGOInput;
class ISchemaSystem;
class IGlobalVariables;
class IInputSystem;
class IInputService;
class IGameResourceService;
class IEngineClient;
class IEngineCVar;
class INetworkClientService;
class IGameUIServices;
class IViewRender;
class IMaterialSystem2;
class ISource2Client;
class IGameEventManager2;
class IResourceSystem;
class CResourceHandleUtils;
class IParticleSystemMgr;
class CGameParticleManager;
class CParticleCollection;
class ISceneSystem;

class CVPhys2World;
class CLocalize;
class CBaseFileSystem;
class CPrediction;
class CEnginePVSManager
{
public:

	void Set(bool bState)
	{
		Memory::CallVFunc<void*, 6U>(this, bState);
	}
};
class CDebugOverlayGameSystem;
class C_CSGameRules;

class CRenderGameSystem;
class IClientMode;
class CMeshSystem;

namespace Interfaces
{
	bool Setup( );

	void CreateRenderTarget(IDXGISwapChain* pSwapChain);
	void DestroyRenderTarget( );
	CGameParticleManager* GetGameParticleManager();

	inline IMemAlloc* m_pMemAlloc = nullptr;
	inline IDXGISwapChain* m_IDXGISwapChain = nullptr;
	inline ISwapChainDx11* m_pSwapChain = nullptr;
	inline IDXGISwapChain* Dummy_Swapchain = nullptr;
	inline ID3D11Device* m_pDevice = nullptr;
	inline ID3D11DeviceContext* m_pDeviceContext = nullptr;
	inline ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	inline CCSGOInput* m_pInput = nullptr;
	inline ISchemaSystem* m_pSchemaSystem = nullptr;
	inline IGlobalVariables* m_pGlobalVariables = nullptr;
	inline std::uintptr_t* m_pEntityList = nullptr;
	inline IInputSystem* m_pInputSystem = nullptr;
	inline IInputService* m_pInputService = nullptr;
	inline IGameResourceService* m_pGameResourceService = nullptr;
	inline ISource2Client* m_pClient = nullptr;
	inline IEngineClient* m_pEngine = nullptr;
	inline IEngineCVar* m_pCvar = nullptr;
	inline INetworkClientService* m_pNetworkClientService = nullptr;
	inline IGameUIServices* m_pGameUIServices = nullptr;
	inline IMaterialSystem2* m_pMaterialSystem2 = nullptr;
	inline IResourceSystem* m_pResourceSystem = nullptr;
	inline CResourceHandleUtils* m_pResourceHandleUtils = nullptr;

	inline IViewRender* m_pViewRender = nullptr;
	inline IClientMode* m_pClientMode = nullptr;
	inline CVPhys2World* m_pVPhys2World = nullptr;

	inline CLocalize* m_pLocalize = nullptr;
	inline CBaseFileSystem* m_pFileSystem = nullptr;

	inline ISteamClient* m_pSteamClient = nullptr;
	inline ISteamUser* m_pSteamUser = nullptr;
	inline ISteamUtils* m_pSteamUtils = nullptr;
	inline ISteamFriends* m_pSteamFriends = nullptr;
	inline ISteamGameCoordinator* m_pSteamGameCoordinator = nullptr;
	inline ISteamMatchmaking* m_pSteamMatchmaking = nullptr;

	inline CPrediction* m_pPrediction = nullptr;

	inline CEnginePVSManager* m_pPVSManager = nullptr;

	inline IGameEventManager2* m_pGameEventManager = nullptr;

	inline IParticleSystemMgr* m_pParticleSystemMgr = nullptr;
	inline CGameParticleManager* m_pParticleManager = nullptr;
	inline CParticleCollection* m_pParticleCollection = nullptr;

	inline C_CSGameRules* m_pGameRules = nullptr;

	inline ISceneSystem* m_pSceneSystem = nullptr;
	inline CMeshSystem* m_pMeshSystem = nullptr;
}
