#include "../Precompiled.h"
#include "Hooks.h"
#include "../features/threading/Threader.h"
#include "../features/misc/ShotHandler.h"
#include "../features/misc/FSN handler.h"
#include <chrono>
#include <random>

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include "../features/misc/misc.h"

using namespace NRagebot;


void* GetModuleBaseAddress( void* address )
{
	HMODULE hMods[ 1024 ];
	HANDLE hProcess = GetCurrentProcess( );
	DWORD cbNeeded;

	if ( EnumProcessModules( hProcess, hMods, sizeof( hMods ), &cbNeeded ) )
	{
		for ( size_t i = 0; i < ( cbNeeded / sizeof( HMODULE ) ); i++ )
		{
			MODULEINFO modInfo;
			if ( GetModuleInformation( hProcess, hMods[ i ], &modInfo, sizeof( modInfo ) ) )
			{
				if ( ( uintptr_t )address >= ( uintptr_t )modInfo.lpBaseOfDll &&
					( uintptr_t )address < ( uintptr_t )modInfo.lpBaseOfDll + modInfo.SizeOfImage )
				{
					return modInfo.lpBaseOfDll;
				}
			}
		}
	}
	return nullptr;
}

bool Hooks::Setup( )
{
	if ( MH_Initialize( ) != MH_OK )
		throw std::runtime_error( X( "failed initialize minhook" ) );

	SubscribeHook( Detours::MouseInputEnabled.Init( &hkMouseInputEnabled, Interfaces::m_pInput, VTABLE::CLIENT::MOUSEINPUTENABLED ) );
	SubscribeHook( Detours::Present.Init( &hkPresent, Interfaces::m_IDXGISwapChain ) );
	SubscribeHook( Detours::CreateMovePrePrediction.Init( &hkCreateMovePrePrediction, Interfaces::m_pInput, VTABLE::CLIENT::CREATEMOVEPREPREDICTION ) );
	SubscribeHook( Detours::FrameStageNotify.Init( &hkFrameStageNotify, Interfaces::m_pClient, VTABLE::CLIENT::FRAMESTAGENOTIFY ) );
	SubscribeHook( Detours::LevelInit.Init( &hkLevelInit, Interfaces::m_pClientMode, VTABLE::CLIENTMODE::LEVELINIT ) );
	SubscribeHook( Detours::LevelShutDown.Init( &hkLevelShutDown, Interfaces::m_pClientMode, VTABLE::CLIENTMODE::LEVELSHUTDOWN ) );
	SubscribeHook( Detours::OverrideView.Init( &hkOverrideView, Memory::FindPattern( CLIENT_DLL, X( "40 57 48 83 EC ? 48 8B FA E8 ? ? ? ? BA" ) ) ) );

	SubscribeHook( Detours::OnAddEntity.Init( &hkOnAddEntity, Interfaces::m_pGameResourceService->pGameEntitySystem, VTABLE::ENTITYSYSTEM::ONADDENTITY ) );
	SubscribeHook( Detours::OnRemoveEntity.Init( &hkOnRemoveEntity, Interfaces::m_pGameResourceService->pGameEntitySystem, VTABLE::ENTITYSYSTEM::ONREMOVEENTITY ) );
	SubscribeHook(Detours::GetMatrixForView.Init(&hkGetMatrixForView, Memory::FindPattern( CLIENT_DLL, X( "48 8B C4 48 89 68 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 49 8B F1" ) )));
	SubscribeHook( Detours::OnRenderStart.Init( &hkOnRenderStart, Interfaces::m_pViewRender, 4 ) );
	SubscribeHook( Detours::SendMessageGC.Init( &hkSendMessageGC, Interfaces::m_pSteamGameCoordinator, VTABLE::STEAMGAMECOORDINATOR::SENDMESSAGEGC ) );

	SubscribeHook( Detours::SetWorldFov.Init( &hkSetWorldFov, Memory::GetAbsoluteAddress( Memory::FindPattern( CLIENT_DLL, X( "E8 ? ? ? ? F3 0F 11 06 48 8B 5C 24 ? 48 8B 6C 24 ? 48 83 C4 ? 41 5E 5F 5E C3 CC CC CC CC CC CC CC CC" ) ), 0x1, 0x0 ) ) );
	SubscribeHook( Detours::DrawSmokeVertex.Init( &hkDrawSmokeVertex, Memory::FindPattern( CLIENT_DLL, X( "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B 9C 24 ? ? ? ? 4D 8B F8" ) ) ) );
	SubscribeHook(Detours::IsGlowing.Init(&hkIsGlowing, Memory::GetAbsoluteAddress(Memory::FindPattern( CLIENT_DLL, X( "E8 ? ? ? ? 33 DB 84 C0 0F 84 ? ? ? ? 48 8B 4F" ) ), 0x1, 0x0)));
	SubscribeHook(Detours::GetGlowColor.Init(&hkGetGlowColor, Memory::FindPattern( CLIENT_DLL, X( "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F2 48 8B F9 48 8B 54 24" ) )));

	SubscribeHook(Detours::FlashOverlay.Init(&hkFlashOverlay, Memory::FindPattern( CLIENT_DLL, X( "85 D2 0F 88 ? ? ? ? 48 8B 4C 24" ) )));
	SubscribeHook( Detours::CalcViewmodel.Init( &hkCalcViewmodel, Memory::FindPattern( CLIENT_DLL, X( "40 55 53 56 41 56 41 57 48 8B EC" ) ) ) );
	SubscribeHook( Detours::AllowCameraAngleChange.Init( &hkAllowCameraAngleChange, Interfaces::m_pInput, 7 ) );
	SubscribeHook( Detours::IsRelativeMouseMode.Init( &hkIsRelativeMouseMode, Interfaces::m_pInputSystem, 76) );

	SubscribeHook( Detours::DrawAggeregateObject.Init( &hkDrawAggeregateObject, Memory::FindPattern( SCENESYSTEM_DLL, X( "48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70" ) ) ) );

	SubscribeHook(Detours::DrawArrayLight.Init(&hkDrawArrayLight, Memory::FindPattern(SCENESYSTEM_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 48 89 54 24"))));
	SubscribeHook( Detours::UpdateLightShadowMap.Init( &hkUpdateLightShadowMap, Memory::FindPattern( SCENESYSTEM_DLL, X( "48 89 54 24 ?? 55 57 41 56 48 83 EC" ) ) ) );
	SubscribeHook( Detours::SkyboxDrawarray.Init( &hkSkyboxDrawarray, Memory::FindPattern( SCENESYSTEM_DLL, X( "45 85 C9 0F 8E ? ? ? ? 4C 8B DC 55" ) ) ) );
	SubscribeHook( Detours::ParticleDrawArray.Init( &hkParticleDrawArray, Memory::FindPattern( PARTICLES_DLL, X( "40 55 53 56 57 48 8D 6C 24" ) ) ) );
	SubscribeHook(Detours::SomeUtlSymbolFunc.Init(&hkSomeUtlSymbolFunc, Memory::FindPattern(SOUNDSYSTEM_DLL, X("48 89 5C 24 ? 56 48 83 EC ? 48 63 F2"))));

	SubscribeHook(Detours::ReportHit.Init(&hkReportHit, Memory::FindPattern(CLIENT_DLL, X("40 53 48 83 EC ? 48 8D 05 ? ? ? ? 48 8D 59 ? 48 89 01 F6 03 ? 74 ? 48 8B CB E8 ? ? ? ? 48 8B 0B 0F B6 C1 D0 E8 A8 ? 74 ? 48 8D 59 ? 48 85 DB 74 ? 48 8B CB E8 ? ? ? ? BA ? ? ? ? 48 8B CB 48 83 C4 ? 5B E9 ? ? ? ? 48 83 C4 ? 5B C3 CC CC CC CC CC CC CC CC CC CC 48 89 5C 24 ? 57 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B F9 48 89 01 F6 41 ? ? 74 ? 48 83 C1 ? E8 ? ? ? ? EB ? 48 8B 41 ? 48 83 E0 ? 48 85 C0 75 ? 48 8D 4F ? E8 ? ? ? ? 48 8D 4F ? E8 ? ? ? ? 48 8B 5F ? 0F B6 C3 D0 E8 A8 ? 74 ? 48 83 C3 ? 74 ? 48 8B CB E8 ? ? ? ? BA ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 CC CC CC CC CC CC 48 89 5C 24 ? 57 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B D9"))));

	SubscribeHook( Detours::GeneratePrimitives.Init( &hkGeneratePrimitives, Memory::FindPattern( SCENESYSTEM_DLL, X( "48 8B C4 48 89 58 ? 48 89 50 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 45 33 F6" ) ) ) );

	SubscribeHook( Detours::DrawAggregateSceneObject.Init( &DrawAggregateSceneObject, Memory::FindPattern( SCENESYSTEM_DLL, X( "48 8B C4 4C 89 40 ? 48 89 50 ? 55 53 41 57" ) ) ) );
	SubscribeHook( Detours::DrawScopeOverlay.Init( &hkDrawScopeOverlay, Memory::FindPattern( CLIENT_DLL, X( "48 8B C4 53 57 48 83 EC ? 48 8B FA" ) ) ) );
	SubscribeHook( Detours::DrawOverHead.Init( &hkDrawOverHead, Memory::FindPattern( CLIENT_DLL, X( "40 53 48 83 EC ? 48 8B D9 83 FA ? 75 ? 48 8B 0D ? ? ? ? 48 8D 54 24 ? 48 8B 01 FF 90 ? ? ? ? 8B 10" ) ) ) );
	SubscribeHook(Detours::DrawCrosshair.Init(&hkDrawCrosshair, Memory::FindPattern( CLIENT_DLL, X( "48 89 5C 24 08 57 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 48 85" ) )));

	SubscribeHook( Detours::FirstPersonLegs.Init( &hkDrawFirstPersonLegs, Memory::FindPattern( CLIENT_DLL, X( "40 55 53 56 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? F2 0F 10 42" ) ) ) );
	SubscribeHook( Detours::HandleTeamIntro.Init( &hkHandleTeamIntro, Memory::FindPattern( CLIENT_DLL, X( "48 83 EC ? ? ? ? ? 44 38 89" ) ) ) );
	SubscribeHook(Detours::PrepareSceneMaterial.Init(&hkPrepareSceneMaterial, Memory::FindPattern( MATERIAL_SYSTEM2_DLL, X( "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30 48 8B 59 18 48 8B F2 48 63 79 10 48 C1 E7 06" ) )));
	SubscribeHook( Detours::CreateSceneObjectFromModelHandle.Init( &hkCreateSceneObjectFromModelHandle, Memory::FindPattern( L"meshsystem.dll", X( "48 89 5C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 32" ) ) ) );

	SubscribeHook(Detours::UpdatePostProcessing.Init(&hkUpdatePostProcessing, Memory::FindPattern( CLIENT_DLL, X( "48 89 5C 24 08 57 48 83 EC 60 80" ) )));

	SubscribeHook(Detours::ConsoleLog.Init(&hkConsoleLog, Memory::FindPattern(TIER0_DLL, X("4C 89 4C 24 ? 44 89 44 24 ? 89 54 24 ? 55"))));

	InitializeThreader( );

	return true;
}

void Hooks::Destroy( )
{
	for ( auto& it : vecDetours )
		it.Restore( );

	MH_DisableHook( MH_ALL_HOOKS );
	MH_RemoveHook( MH_ALL_HOOKS );

	MH_Uninitialize( );
}

void* Hooks::hkDrawFirstPersonLegs( void* a1, void* a2, void* a3, void* a4, void* a5 )
{
	static auto og = Detours::FirstPersonLegs.GetOriginal< decltype( &hkDrawFirstPersonLegs ) >( );

	if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_LOCAL_LEGS ) )
		return nullptr;

	return og( a1, a2, a3, a4, a5 );
}

void Hooks::hkHandleTeamIntro( void* a1, void* a2, bool* pbTeamIntroEnd )
{
	static const auto oOriginal = Detours::HandleTeamIntro.GetOriginal<decltype( &hkHandleTeamIntro )>( );

	if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_TEAM_INTRO ) )
		return;

	return oOriginal( a1, a2, pbTeamIntroEnd );
}

float Hooks::hkPrepareSceneMaterial(CMaterial2* pMaterial, void* pData, float flNull)
{
	static const auto oOriginal = Detours::PrepareSceneMaterial.GetOriginal<decltype(&hkPrepareSceneMaterial)>();

	static int currIndexBrightness = 0;
	static int currIndexRenderOnly = 0;
	static float lastBrightness = 0.f;
	static float lastBrightnessRenderOnly = 0.f;
	static CMaterial2* lastMaterial = 0;
	uint64_t pParameters = *(uint64_t*)((uint64_t)pMaterial + 0x18);

	if (pMaterial != lastMaterial)
	{
		bool found1 = false, found2 = false;

		uint32_t count = *(uint32_t*)((uint64_t)pMaterial + 0x10);

		for (int i = 0; i < count; i++)
		{
			uint64_t pParameter = pParameters + (i * 0x40);

			if (strcmp(*(const char**)(pParameter + 0x28), "g_flBrightnessExposureBias") == 0)
			{
				currIndexBrightness = i;
				lastBrightness = *(float*)(pParameter);
				if (found2)
					break;
				found1 = true;
			}
			else if (strcmp(*(const char**)(pParameter + 0x28), "g_flRenderOnlyExposureBias") == 0)
			{
				currIndexRenderOnly = i;
				lastBrightnessRenderOnly = *(float*)(pParameter);
				if (found1)
					break;
				found2 = true;
			}
		}

		lastMaterial = pMaterial;
	}

	if (Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals).at(REMOVE_FOG_BRIGHTNESS))
	{
		*(float*)(pParameters + (currIndexBrightness * 0x40)) = -6.f;
		*(float*)(pParameters + (currIndexRenderOnly * 0x40)) = -6.f;
	}
	else
	{
		*(float*)(pParameters + (currIndexBrightness * 0x40)) = lastBrightness;
		*(float*)(pParameters + (currIndexRenderOnly * 0x40)) = lastBrightnessRenderOnly;
	}

	return oOriginal(pMaterial, pData, flNull);
}

void* Hooks::hkCreateSceneObjectFromModelHandle( void* pMeshSystem, int** pp_hModel_arg, float* pUnkStruct, const char* szSceneObjectType_arg, __int64 someValBasedOnWorldGroupId, __int64 someMagicNum, __int64 pSomeRenderValuePointer )
{
	static auto og = Detours::CreateSceneObjectFromModelHandle.GetOriginal< decltype( &hkCreateSceneObjectFromModelHandle ) >( );

	auto result = og( pMeshSystem, pp_hModel_arg, pUnkStruct, szSceneObjectType_arg, someValBasedOnWorldGroupId, someMagicNum, pSomeRenderValuePointer );

	return result;
}

bool Hooks::hkDrawCrosshair( void* a1 ) {
	static auto og = Detours::DrawCrosshair.GetOriginal< decltype( &hkDrawCrosshair ) >( );

	if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_CROSSHAIR ) )
		return false;

	return og( a1 );
}
void Hooks::hkDrawScopeOverlay(void* a1, void* a2) {
	const auto original = Detours::DrawScopeOverlay.GetOriginal<decltype(&hkDrawScopeOverlay)>();

	if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_DEFAULT)
		return original(a1, a2);

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_OVERLAY)
		return;

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_GRADIENT)
		return;

	else if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_ALL)
		return;

	return original(a1, a2);
}
void Hooks::hkTonemapSystemRenderDebug( void* a1, void* a2, void* a3 ) {

	static auto og = Detours::ToneMap.GetOriginal< decltype( &hkTonemapSystemRenderDebug ) >( );

	struct CToneMap
	{
		MEM_PAD( 0x298 );
		float m_flMulti;
		float m_flScale;
		float m_flSomeVal;
	};

	CToneMap* pToneMap = ( CToneMap* )a1;
	if ( Config::i( g_Variables.m_WorldVisuals.m_nWorldExposure ) == 0 )
	{
		pToneMap->m_flScale = 0.f;
		pToneMap->m_flMulti = 0.f;
		pToneMap->m_flSomeVal = -1.566f;
	}
	else
	{
		pToneMap->m_flScale = std::clamp( ( ( float )( Config::i( g_Variables.m_WorldVisuals.m_nWorldExposure ) / 100.f ) * 0.5f ), 0.f, 2.5f );
		pToneMap->m_flMulti = 1.f;
		pToneMap->m_flSomeVal = FLT_MAX;
	}

	return og( a1, a2, a3 );
}

static __int64 Hooks::hkCAnimatableSceneObjectDesc__FindOrAllocateTransforms( __int64 CAnimatableSceneObjectDesc, cs2::animatable_scene_object* animatable_scene_object, cs2::mesh_instance* CMeshInstance_Imp, __int64 a4, __int64 a5, int a6, const char* a7, __int64* a8, __int64* a9 )
{
	const auto og = Detours::CAnimatableSceneObjectDesc__FindOrAllocateTransforms.GetOriginal< decltype( &hkCAnimatableSceneObjectDesc__FindOrAllocateTransforms ) >( );

	static std::shared_mutex mutex{ };
	std::unique_lock lock{ mutex };

	C_BaseEntity* entity{ };
	if ( !animatable_scene_object->m_owner_handle.IsValid( ) || !( entity = animatable_scene_object->m_owner_handle.Get( ) ) || !entity->IsPlayerPawn( ) )
		return og( CAnimatableSceneObjectDesc, animatable_scene_object, CMeshInstance_Imp, a4, a5, a6, a7, a8, a9 );

	CMeshInstance_Imp->m_transforms_cache->m_mutex = 0;

	auto ret = og( CAnimatableSceneObjectDesc, animatable_scene_object, CMeshInstance_Imp, a4, a5, a6, a7, a8, a9 );

	CMeshInstance_Imp->m_transforms_cache->m_mutex = 0;

	return ret;
}

bool FASTCALL Hooks::hkDrawOverHead( C_CSPlayerPawn* a1 ) {

	static auto og = Detours::DrawOverHead.GetOriginal<decltype( &hkDrawOverHead )>( );

	if ( !a1 )
		return og( a1 );
	if ( !Globals::m_pLocalPlayerPawn )
		return og( a1 );
	if ( a1 == Globals::m_pLocalPlayerPawn ) {
		if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_LOCAL_OVERHEAD_INFO ) )
			return false;
		return true;
	}
	if ( !a1->IsEnemy( Globals::m_pLocalPlayerPawn ) &&
		Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_TEAM_OVERHEAD_INFO ) )
		return false;
	return true;
}

struct ReportHit_t
{
	MEM_PAD( 0x18 );
	Vector m_vecPosition;
	float m_flTime;
};

_QWORD* FASTCALL Hooks::hkReportHit( __int64 a1 )
{
	const auto oOriginal = Detours::ReportHit.GetOriginal< decltype( &hkReportHit ) >( );

	if ( !Interfaces::m_pGlobalVariables )
		return oOriginal( a1 );

	const ReportHit_t* pReportHit = ( ReportHit_t* )a1;

	CShotHandler::PlayerHurtRecord_t objHurtRec;
	objHurtRec.m_flTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	objHurtRec.m_vecImpactPosition = pReportHit->m_vecPosition;

	g_ShotHandler->m_HitRecords.push_back( objHurtRec );

	return oOriginal( a1 );
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
long Hooks::hkWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	Input::Process( uMsg, wParam, lParam );

	if ( Config::i( g_Variables.m_Gui.m_iMenuKey ) > 0 && Input::IsKeyReleased( Config::i( g_Variables.m_Gui.m_iMenuKey ) ) )
	{
		LocalPlayerData::m_bRageNeedsConfigUpdate = true;
		Gui::m_bOpen = !Gui::m_bOpen;

		const auto oIsRelativeMouseMode = Detours::IsRelativeMouseMode.GetOriginal<decltype( &hkIsRelativeMouseMode )>( );
		if ( oIsRelativeMouseMode )
			oIsRelativeMouseMode( Interfaces::m_pInputSystem, Gui::m_bOpen ? false : Gui::m_bInputActive );
	}

	if ( Gui::m_bOpen )
	{
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );

		if ( wParam != 'R' && wParam != '1' && wParam != '2' && wParam != '3' && wParam != '4' && wParam != '5' && wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ImGui::GetIO( ).WantTextInput )
			return true;

		if ( Input::IsKeyDown( VK_ESCAPE ) )
			Gui::m_bOpen = false;
	}

	if ( Gui::m_bOpen && ( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE ) )
		return false;

	return static_cast< long >( CallWindowProcW( Input::m_pOldWndProc, hWnd, uMsg, wParam, lParam ) );
}
inline std::once_flag init_flag;
inline ID3D11DeviceContext* m_device_context;
inline ID3D11RenderTargetView* m_render_target;
inline ID3D11Device* m_device;
inline WNDPROC m_window_proc_original = 0;
inline HWND m_window;
HRESULT WINAPI Hooks::hkPresent( IDXGISwapChain* pSwapChain, UINT uSyncInterval, UINT uFlags )
{
	const auto oPresent = Detours::Present.GetOriginal< decltype( &hkPresent ) >( );

	ID3D11Texture2D* m_back_buffer = nullptr;
	DXGI_SWAP_CHAIN_DESC m_desc;

	pSwapChain->GetDesc(&m_desc);

	m_window = m_desc.OutputWindow;

	std::call_once(init_flag, [&]()
		{
			pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_device));
			m_device->GetImmediateContext(&m_device_context);
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_back_buffer));

			m_device->CreateRenderTargetView(m_back_buffer, nullptr, &m_render_target);

			if (m_back_buffer)
				m_back_buffer->Release();

			m_window_proc_original = reinterpret_cast<WNDPROC>(
				SetWindowLongPtrA(
					m_window,
					GWLP_WNDPROC,
					reinterpret_cast<LONG_PTR>(hkWndProc)
				));

			ImGui::CreateContext();

			ImGui_ImplWin32_Init(m_window);
			ImGui_ImplDX11_Init(m_device, m_device_context);
			blurEffect.Initialize(m_device, m_device_context);
			Interfaces::m_pDevice = m_device;
			Interfaces::m_pDeviceContext = m_device_context;
			if (!Gui::m_bInitialized)
				Gui::Initialize( );
			Draw::m_bInitialized = true;
		});

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{

		Gui::Render( );

		g_ShotHandler->OnPresent( );
		Scope::Run( );

		Draw::pBackgroundDrawList = ImGui::GetBackgroundDrawList( );
		Draw::RenderDrawData( Draw::pBackgroundDrawList );
#if _DEBUG

#endif
	}
	ImGui::Render();

	m_device_context->OMSetRenderTargets(1, &m_render_target, nullptr);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(pSwapChain, uSyncInterval, uFlags);
}

HRESULT FASTCALL Hooks::hkResizeBuffers( IDXGISwapChain* pSwapChain, std::uint32_t nBufferCount, std::uint32_t nWidth, std::uint32_t nHeight, DXGI_FORMAT newFormat, std::uint32_t nFlags )
{
	const auto oResizeBuffer = Detours::ResizeBuffers.GetOriginal< decltype( &hkResizeBuffers ) >( );

	HRESULT hResult = oResizeBuffer( pSwapChain, nBufferCount, nWidth, nHeight, newFormat, nFlags );
	if ( SUCCEEDED( hResult ) )
		Interfaces::CreateRenderTarget(pSwapChain);

	return hResult;
}

HRESULT WINAPI Hooks::hkCreateSwapChain( IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain )
{
	const auto oCreateSwapChain = Detours::CreateSwapChain.GetOriginal< decltype( &hkCreateSwapChain ) >( );
	Interfaces::DestroyRenderTarget( );
	return oCreateSwapChain( pFactory, pDevice, pDesc, ppSwapChain );
}

class CSubtickBackup
{
public:
	google::protobuf::RepeatedPtrField< std::string > m_LastSubtickMoves;

	void CreateBackup( CUserCmd* pCmd )
	{
		m_LastSubtickMoves.Clear( );
		for ( int i = 0; i < pCmd->m_csgoUserCmd.mutable_base( )->subtick_moves_size( ); i++ )
		{
			auto& objSubtickMoveStep = pCmd->m_csgoUserCmd.mutable_base( )->subtick_moves( ).at( i );
			objSubtickMoveStep.AppendToString( m_LastSubtickMoves.Add( ) );
		}
	}

	void RestoreFromBackup( CUserCmd* pCmd )
	{
		if ( m_LastSubtickMoves.size( ) )
		{
			pCmd->m_csgoUserCmd.mutable_base( )->clear_subtick_moves( );
			for ( const auto& strLasSubtickMove : m_LastSubtickMoves )
			{
				if ( const auto pSubtickMoveStep = Interfaces::m_pInput->CreateNewSubTickMoveStep( pCmd->m_csgoUserCmd.mutable_base( )->mutable_subtick_moves( ) ) )
				{
					pSubtickMoveStep->MergeFromString( strLasSubtickMove );

					pCmd->m_csgoUserCmd.mutable_base( )->mutable_subtick_moves( )->AddAllocated( pSubtickMoveStep );
				}
			}
		}
	}
};
const inline auto g_SubtickBackup = std::make_unique<CSubtickBackup>( );

int s_nOldSeqNum = 0;

void Hooks::hkCreateMovePrePrediction( CCSGOInput* pInput, int nSlot, bool bActive )
{
	static auto oCreateMove = Detours::CreateMovePrePrediction.GetOriginal< decltype( &hkCreateMovePrePrediction ) >( );

	oCreateMove( pInput, nSlot, bActive );

	if ( !LocalPlayerData::Update( ) )
		return;
	if ( !Globals::m_pLocalPlayerPawn )
		return;

	if ( g_Entities->m_vecEntities.empty( ) )
		g_Entities->ForceUpdateEntityList( );

	g_Prediction->Begin( Globals::m_pCmd );
	{
		g_Movement->BunnyHop();

		g_Movement->JumpBug( );

		g_Movement->StoreCameraAngles( );

		g_Misc->AutoPistol();
		g_Misc->AutoPurchase();
		g_Misc->ShowImpacts();

		g_Misc->ShowHud();

		g_Misc->FreeLook(pInput, Globals::m_pCmd);

		if (Config::b(g_Variables.m_Legitbot.m_bEnableRCS) && Globals::m_pLocalPlayerPawn)
		{
			int shots_fired = Globals::m_pLocalPlayerPawn->m_iShotsFired();
			g_Misc->RunRCS(Globals::m_pLocalPlayerPawn, Globals::m_pCmd, shots_fired, 2.0f);
		}

		g_AntiAim->OnCreateMove( );

		g_Movement->StoreModelAngles( );

		g_Movement->HandleDirectionalStrafe( );

		g_Movement->EdgeJump(Globals::m_pCmd);

		g_Legitbot->CreateMove( );
		g_Legitbot->Triggerbot( );

		g_WorldModulation->Exposure(Globals::m_pLocalPlayerPawn);

	}
	g_Prediction->End( Globals::m_pCmd );

	g_Movement->MovementCorrection( );

	if ( s_nOldSeqNum == Globals::m_pCmd->m_nCommandNumber )
	{
		g_SubtickBackup->RestoreFromBackup( Globals::m_pCmd );
	}

	g_SubtickBackup->CreateBackup( Globals::m_pCmd );
	s_nOldSeqNum = Globals::m_pCmd->m_nCommandNumber;
	if (Globals::m_pBaseCmd->forwardmove() > 0.f)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_FORWARD, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (Globals::m_pBaseCmd->forwardmove() < 0.f)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_BACK, CInButtonState::EButtonState::IN_BUTTON_DOWN);

	if (Globals::m_pBaseCmd->leftmove() > 0.f)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVELEFT, CInButtonState::EButtonState::IN_BUTTON_DOWN);
	else if (Globals::m_pBaseCmd->leftmove() < 0.f)
		Globals::m_pCmd->m_nButtons.SetButtonState(IN_MOVERIGHT, CInButtonState::EButtonState::IN_BUTTON_DOWN);

}

void* FASTCALL Hooks::hkInputParser( CCSInputMessage* pInputMessage, CSGOInputHistoryEntryPB* pInputHistory, bool bSomeBool, TimeStamp_t a4, TimeStamp_t a5, C_CSPlayerPawn* pLocalPlayer )
{
	static auto oInputParser = Detours::InputParser.GetOriginal<decltype( &hkInputParser )>( );

	if ( !Globals::m_pCmd )
		return oInputParser( pInputMessage, pInputHistory, bSomeBool, a4, a5, pLocalPlayer );

	if ( !Globals::m_pBaseCmd )
		return oInputParser( pInputMessage, pInputHistory, bSomeBool, a4, a5, pLocalPlayer );

	auto res = oInputParser( pInputMessage, pInputHistory, bSomeBool, a4, a5, pLocalPlayer );

	return res;
}

bool FASTCALL Hooks::hkCreateMovePostPrediction( CCSGOInput* pInput, unsigned int nSlot, CUserCmd* pCmd )
{
	const auto oCreateMove = Detours::CreateMovePostPrediction.GetOriginal< decltype( &hkCreateMovePostPrediction ) >( );

	const bool bResult = ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oCreateMove, pInput, nSlot, pCmd );

	return bResult;
}

bool FASTCALL Hooks::hkMouseInputEnabled( void* pRcx )
{
	const auto oMouseInputEnabled = Detours::MouseInputEnabled.GetOriginal< decltype( &hkMouseInputEnabled ) >( );
	return Gui::m_bOpen ? false : ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oMouseInputEnabled, pRcx );
}

void FASTCALL Hooks::hkFrameStageNotify( void* pRcx, EClientFrameStage nFrameStage )
{
	static auto oFrameStageNotify = Detours::FrameStageNotify.GetOriginal< decltype( &hkFrameStageNotify ) >( );
	g_FSN->OnPreOriginal( nFrameStage );
	ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oFrameStageNotify, pRcx, nFrameStage );

}

void STDCALL Hooks::hkLevelInit( IClientMode* pClientModeShared, const char* szNewMap )
{
	const auto oLevelInit = Detours::LevelInit.GetOriginal< decltype( &hkLevelInit ) >( );

	if (Interfaces::m_pGlobalVariables == nullptr)
		Interfaces::m_pGlobalVariables = *reinterpret_cast<IGlobalVariables**>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("48 89 15 ? ? ? ? 48 89 42")), 0x3, 0x7));
	if (Interfaces::m_pEntityList == nullptr)
		Interfaces::m_pEntityList = reinterpret_cast<std::uintptr_t*>(Memory::ResolveRelativeAddress(Memory::FindPattern(CLIENT_DLL, X("4C 8B 0D ? ? ? ? 4D 85 C9 74 ? 41 83 F8 ? 74 ? 41 8B C8 81 E1 ? ? ? ? 8B C1 C1 E8 ? 4D 8B 14 C1")), 0x3, 0x7));

	Interfaces::m_pGameRules = *reinterpret_cast<C_CSGameRules**>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("48 8B 1D ? ? ? ? E9 ? ? ? ? 48 8B 0D")), 0x3));
	Interfaces::m_pParticleManager = Interfaces::GetGameParticleManager( );

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oLevelInit, pClientModeShared, szNewMap );
}

void STDCALL Hooks::hkLevelShutDown( IClientMode* pClientModeShared )
{
	static auto oLevelShutdown = Detours::LevelShutDown.GetOriginal< decltype( &hkLevelShutDown ) >( );

	Interfaces::m_pGlobalVariables = nullptr;
	Interfaces::m_pEntityList = nullptr;

	Globals::m_pLocalPlayerController = nullptr;
	Globals::m_pLocalPlayerPawn = nullptr;

	LocalPlayerData::Reset( );

	g_WorldModulation->vecSkyboxes.clear();

	g_WorldModulation->UpdateSkyboxes();
	g_WorldModulation->LoadDefaultSkyboxes();

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oLevelShutdown, pClientModeShared );
}

void FASTCALL Hooks::hkOverrideView( void* pClientModeCSNormal, CViewSetup* pSetup )
{
	const auto oOverrideView = Detours::OverrideView.GetOriginal< decltype( &hkOverrideView ) >( );

	if ( !Globals::m_pLocalPlayerController )
		return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup );

	if ( !Globals::m_pLocalPlayerPawn || Globals::m_pLocalPlayerPawn->m_iHealth( ) <= 0 )
		return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup );

	if ( !Interfaces::m_pEngine->IsConnected( ) || !Interfaces::m_pEngine->IsInGame( ) || !Globals::m_pLocalPlayerController->m_bPawnIsAlive( ) && !Interfaces::m_pGameRules->m_bTeamIntroPeriod( ) )
		return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup );

	oOverrideView( pClientModeCSNormal, pSetup );

	if (Config::b(g_Variables.m_WorldVisuals.m_bLSDMode))
	{
		static auto lastLSDUpdate = std::chrono::steady_clock::now();
		auto currentTime = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastLSDUpdate).count() >= 100)
		{
			lastLSDUpdate = currentTime;

			static std::mt19937 rng(std::random_device{}());
			std::uniform_int_distribution<int> colorDist(0, 255);
			std::uniform_int_distribution<int> fovDist(30, 150);
			std::uniform_int_distribution<int> aspectDist(0, 30);

			Config::c(g_Variables.m_WorldVisuals.m_colWorldColor) = Color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
			Config::c(g_Variables.m_WorldVisuals.m_colSunColor) = Color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
			Config::c(g_Variables.m_WorldVisuals.m_colCloudColor) = Color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
			Config::c(g_Variables.m_WorldVisuals.m_colLighting) = Color(colorDist(rng), colorDist(rng), colorDist(rng), 255);

			Config::b(g_Variables.m_WorldVisuals.m_bWorldModulation) = true;

			Config::i(g_Variables.m_Visuals.m_iWorldFov) = fovDist(rng);
			Config::b(g_Variables.m_Visuals.m_bOverrideWorldFov) = true;

			Config::i(g_Variables.m_WorldEffects.m_iAspectRatio) = aspectDist(rng);
			Config::b(g_Variables.m_WorldEffects.m_bAspectRatioChanger) = true;
		}
	}

	static float flOriginalAspectRatio = pSetup->m_flAspectRatio;

	int iAspectRatioValue = Config::i( g_Variables.m_WorldEffects.m_iAspectRatio );
	if ( iAspectRatioValue != 0)
	{
		pSetup->m_flAspectRatio = static_cast< float >( iAspectRatioValue ) / 10.f;
		pSetup->m_nSomeFlags |= 2;
	}
	else
	{
		pSetup->m_flAspectRatio = flOriginalAspectRatio;
		pSetup->m_nSomeFlags &= ~2;
	}

	if ( Config::b( g_Variables.m_Visuals.m_bRemovePunch ) )
		pSetup->m_angView = Interfaces::m_pInput->GetViewAngles( );

	if (Config::b(g_Variables.m_Misc.kbFreeLook))
	{
		if (Input::HandleInput(Config::kb(g_Variables.m_Misc.m_kbFreeLook))) {
			pSetup->m_vecOrigin = g_Misc->GetFreeLookOffset();
			pSetup->m_angView = g_Misc->GetFreeLookAngle();
		}
	}

	if ( Config::b( g_Variables.m_Visuals.m_bEnableThirdPerson ) && Input::HandleInput( Config::kb( g_Variables.m_Visuals.m_iThirdPersonKeybind ) ) )
	{
		Vector vecFinalEyePos = Globals::m_pLocalPlayerPawn->GetEyePosition( );
		if ( Config::b( g_Variables.m_Ragebot.m_bDuckPeekAssist ) && Input::HandleInput( Config::kb( g_Variables.m_Ragebot.m_kbDuckPeekAssist ) ) && ( Globals::m_pCmd->m_nButtons.m_nValue & IN_DUCK ) )
		{
			vecFinalEyePos.z = Globals::m_pLocalPlayerPawn->GetEyePosition( ).z + ( 18 * Globals::m_pLocalPlayerPawn->m_pMovementServices( )->m_flDuckAmount( ) );
		}

		QAngle angAdjustedCameraViewAngle = Interfaces::m_pInput->GetViewAngles( );
		angAdjustedCameraViewAngle.x = -angAdjustedCameraViewAngle.x;
		pSetup->m_vecOrigin = Math::CalculateCameraPosition( vecFinalEyePos, -( float )Config::i( g_Variables.m_Visuals.m_iThirdPersonDistance ), angAdjustedCameraViewAngle );

		Ray_t ray{};
		GameTrace_t trace{};
		TraceFilter_t filter{ 0x1C3003, Globals::m_pLocalPlayerPawn, NULL, 4 };

		if ( Interfaces::m_pVPhys2World->TraceShape( &ray, vecFinalEyePos, pSetup->m_vecOrigin, &filter, &trace ) ) {
			if ( trace.DidHitWorld( ) )
			{
				pSetup->m_vecOrigin = trace.m_vecPosition;
			}
		}

		pSetup->m_angView = Math::CalcAngle( pSetup->m_vecOrigin, vecFinalEyePos ).Normalize( );
	}

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oOverrideView, pClientModeCSNormal, pSetup );
}

void* FASTCALL Hooks::hkOnAddEntity( void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle )
{
	const auto oOnAddEntity = Detours::OnAddEntity.GetOriginal< decltype( &hkOnAddEntity ) >( );
	g_Entities->OnAddEntity( pInstance, uHandle );
	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pEngineGadet, oOnAddEntity, pRcx, pInstance, uHandle );
}

void* FASTCALL Hooks::hkOnRemoveEntity( void* pRcx, CEntityInstance* pInstance, CBaseHandle uHandle )
{
	const auto oOnRemoveEntity = Detours::OnRemoveEntity.GetOriginal< decltype( &hkOnRemoveEntity ) >( );
	g_Entities->OnRemoveEntity( pInstance, uHandle );
	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pEngineGadet, oOnRemoveEntity, pRcx, pInstance, uHandle );
}

ViewMatrix_t* FASTCALL Hooks::hkGetMatrixForView( CRenderGameSystem* pRenderGameSystem, IViewRender* pViewRender, ViewMatrix_t* pOutWorldToView, ViewMatrix_t* pOutViewToProjection, ViewMatrix_t* pOutWorldToProjection, ViewMatrix_t* pOutWorldToPixels )
{
	const auto oGetMatrixForView = Detours::GetMatrixForView.GetOriginal< decltype( &hkGetMatrixForView ) >( );
	if ( !oGetMatrixForView )
		return pOutWorldToProjection ? pOutWorldToProjection : pOutWorldToView;

	ViewMatrix_t* matResult = oGetMatrixForView( pRenderGameSystem, pViewRender, pOutWorldToView, pOutViewToProjection, pOutWorldToProjection, pOutWorldToPixels );

	if ( pOutWorldToProjection && matResult )
		Globals::m_ViewMatrix = *pOutWorldToProjection;

	return matResult ? matResult : ( pOutWorldToProjection ? pOutWorldToProjection : pOutWorldToView );
}

bool FASTCALL Hooks::hkIsLoadoutAllowed( )
{
	const auto oIsLoadoutAllowed = Detours::IsLoadoutAllowed.GetOriginal<decltype( &hkIsLoadoutAllowed )>( );

	if ( Config::b( g_Variables.m_Misc.m_bUnlockLoadout ) )
		return true;

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oIsLoadoutAllowed );
}

cs2::mesh_instance* Hooks::hkCreateMeshInstance( __int64 CMeshSystem, __int64 CBaseSceneObjectDesc, int maybe_spawn_tick, __int64 model_instance_thing, __int16 zero, __int64 null_mem_addr, __int64 CSceneObject, __int64 minus_one, char minus_one2, __int64 zero_val )
{
	const auto oOriginal = Detours::CreateMeshInstance.GetOriginal<decltype( &hkCreateMeshInstance )>( );

	cs2::mesh_instance* pReturn = oOriginal( CMeshSystem, CBaseSceneObjectDesc, maybe_spawn_tick, model_instance_thing, zero, null_mem_addr, CSceneObject, minus_one, minus_one2, zero_val );

	cs2::unk_mesh_cache* cache = *( cs2::unk_mesh_cache** )model_instance_thing;
	std::string str = cache->m_model_imp->m_path;
	if ( str.find( "characters/" ) != std::string::npos ) {

		for ( int i = 0; i < g_Chams->m_mapMeshInstances[ pReturn ].size( ); i++ ) {
			g_Chams->m_mapMeshInstances[ pReturn ][ i ] = oOriginal( CMeshSystem, CBaseSceneObjectDesc, maybe_spawn_tick, model_instance_thing, zero, null_mem_addr, CSceneObject, minus_one, minus_one2, zero_val );
		}

	}

	return pReturn;
}

void* __fastcall Hooks::hkFreeMeshInstance( void* a1, char a2 )
{
	const auto oOriginal = Detours::FreeMeshInstance.GetOriginal<decltype( &hkFreeMeshInstance )>( );
	auto* original_instance = static_cast< cs2::mesh_instance* >( a1 );

	auto it = g_Chams->m_mapMeshInstances.find( original_instance );
	if ( it != g_Chams->m_mapMeshInstances.end( ) )
	{
		for ( auto* instance : it->second )
		{
			oOriginal( instance, a2 );
		}

		g_Chams->m_mapMeshInstances.erase( it );
	}
	return oOriginal( a1, a2 );
}

void FASTCALL Hooks::hkQuantizeMovement( uintptr_t a1, uintptr_t a2 )
{
	const auto oQuantizeMovement = Detours::QuantizeMovement.GetOriginal<decltype( &hkQuantizeMovement )>( );

	*( float* )( a2 + 0x0 ) = Globals::m_pBaseCmd->forwardmove( );
	*( float* )( a2 + 0x0 ) = Globals::m_pBaseCmd->leftmove( );
	*( float* )( a2 + 0x0 ) = Globals::m_pBaseCmd->upmove( );

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oQuantizeMovement, a1, a2 );
}

int FASTCALL Hooks::hkSendMessageGC( ISteamGameCoordinator* thisptr, std::uint32_t uMsgType, const void* pData, std::uint32_t uData )
{
	const auto oSendMessage = Detours::SendMessageGC.GetOriginal<decltype( &hkSendMessageGC )>( );
	const std::uint32_t uMessageType = ( uMsgType & 0x7FFFFFFF );

	if ( uMessageType == 9144 )
	{

		return k_EGCResultOK;
	}

	const int iStatus = ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oSendMessage, thisptr, uMsgType, pData, uData );

	return iStatus;
}

int FASTCALL Hooks::hkRetrieveMessageGC( ISteamGameCoordinator* thisptr, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize )
{
	const auto oRetrieveMessage = Detours::RetrieveMessageGC.GetOriginal<decltype( &hkRetrieveMessageGC )>( );
	const int iStatus = ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oRetrieveMessage, thisptr, puMsgType, pDest, uDest, puMsgSize );

	if ( iStatus == k_EGCResultNotLoggedOn )
		return k_EGCResultOK;

	if ( iStatus != k_EGCResultOK )
		return iStatus;

	std::uint32_t uMessageType = ( *puMsgType & 0x7FFFFFFF );

	return iStatus;
}

__int64 __fastcall Hooks::hkPanoramaEvent( void* pUnk, const char* szEventName, void* pUnk1, float flUnk )
{
	const auto oPanoramaEvent = Detours::PanoramaEvent.GetOriginal< decltype( &hkPanoramaEvent ) >( );

	if ( FNV1A::Hash( szEventName ) == FNV1A::HashConst( "popup_accept_match_found" ) )
	{
		if ( Config::b( g_Variables.m_Misc.m_bAutoAccept ) )
		{
			Utilities::SetLocalPlayerReady( );

			Utilities::FlashWindow( Input::m_hWindow );
		}
	}

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oPanoramaEvent, pUnk, szEventName, pUnk1, flUnk );
}

void* FASTCALL Hooks::hkCVarGetValue( void* pRcx, const char* szCVar, int iDefaultValue )
{
	const auto oCVarGetValue = Detours::CVarGetValue.GetOriginal<decltype( &hkCVarGetValue )>( );
	static std::uint8_t* pCAM_ThinkReturn = Memory::FindPattern( CLIENT_DLL, X( "48 8B 01 BA 04 00 00 00" ) );
	static bool bWantedValue = true;

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pClientGadet, oCVarGetValue, pRcx, szCVar, iDefaultValue );
}
void Hooks::DrawAggregateSceneObject(void* a1, void* a2, c_aggregate_object_array* a3) {
	static auto original = Detours::DrawAggregateSceneObject.GetOriginal< decltype(&DrawAggregateSceneObject) >();
	original(a1, a2, a3);

	if (!a3->data || !Interfaces::m_pSceneSystem || !Interfaces::m_pSceneSystem->light_data_queue || !Interfaces::m_pSceneSystem->light_data_queue->light_data)
		return;

	for (int i = 0; i < a3->data->count; i++)
	{
		int index = (a3->data->index + i) << 5;
		*reinterpret_cast<Color*>(reinterpret_cast<uintptr_t>(Interfaces::m_pSceneSystem->light_data_queue->light_data) + index) = Config::c(g_Variables.m_WorldVisuals.m_colWorldColor);
	}
}
void* Hooks::hkUpdateAggeregateObject(void* a1, void* a2, CAggregateSceneObjectWorld* a3)
{
	const auto oOriginal = Detours::UpdateAggeregateObject.GetOriginal<decltype( &hkUpdateAggeregateObject )>( );

	if ( !Config::b( g_Variables.m_WorldVisuals.m_bWorldModulation ) )
		return oOriginal( a1, a2,a3 );

	void* result = oOriginal( a1, a2 ,a3);

	int count = a3->count;

	for ( int i = 0; i < count; i++ ) {
		CAggregateSceneObjectDataWorld* scene_object = &a3->array[ i ];
		if ( !scene_object )
			continue;

		scene_object->r = Config::c( g_Variables.m_WorldVisuals.m_colWorldColor ).r( );
		scene_object->g = Config::c( g_Variables.m_WorldVisuals.m_colWorldColor ).g( );
		scene_object->b = Config::c( g_Variables.m_WorldVisuals.m_colWorldColor ).b( );
	}

	return result;
}

enum e_model_type : int
{
	MODEL_SUN,
	MODEL_EFFECTS,
	MODEL_OTHER,
};

int get_model_type( const std::string_view& name ) {
	if ( name.find( "sun" ) != std::string::npos
		|| name.find( "clouds" ) != std::string::npos )
		return MODEL_SUN;

	if ( name.find( "effects" ) != std::string::npos )
		return MODEL_EFFECTS;

	return MODEL_OTHER;
}

void* Hooks::hkDrawAggeregateObject( void* a1, void* a2, CDrawMesh* a3, int a4, int a5, void* a6, void* a7, void* a8 )
{
	const auto oOriginal = Detours::DrawAggeregateObject.GetOriginal<decltype( &hkDrawAggeregateObject )>( );

	if ( !Config::b( g_Variables.m_WorldVisuals.m_bWorldModulation ) )
		return oOriginal( a1, a2, a3, a4, a5, a6, a7, a8 );

	Color color = Color( a3->m_colValue );

	switch ( get_model_type( a3->m_pMaterial->GetName( ) ) ) {
	case MODEL_SUN:
		color = Config::c( g_Variables.m_WorldVisuals.m_colSunColor );
		break;
	case MODEL_EFFECTS:
		color = Config::c( g_Variables.m_WorldVisuals.m_colCloudColor );
		break;
	case MODEL_OTHER:
		color = Config::c( g_Variables.m_WorldVisuals.m_colWorldColor );
		break;
	}

	for ( int i = 0; i < a4; ++i )
	{
		CDrawMesh* pMaterialData = &a3[ i ];
		if ( pMaterialData )
		{
			pMaterialData->m_colValue = color;
		}
	}

	return oOriginal( a1, a2, a3, a4, a5, a6, a7, a8 );
}

CStrongHandle<CMaterial2> CreateMaterial_TEST( const char* szMaterialName, const char szVmatBuffer[ ] )
{
	CKeyValues3* pKeyValues3 = CKeyValues3::CreateMaterialResource( );
	pKeyValues3->LoadFromBuffer( szVmatBuffer );

	CStrongHandle<CMaterial2> pCustomMaterial = {};
	Functions::fnCreateMaterial( nullptr, &pCustomMaterial, szMaterialName, pKeyValues3, 0, 1 );

	return pCustomMaterial;
}

struct CSceneLayer
{
	_BYTE gap0[ 72 ];
	_DWORD m_nDrawFlag;
	_BYTE gap4C[ 1108 ];
	const char char4A0;
	_BYTE gap4A1[ 559 ];
	__int64( __fastcall*** pppfunc6D0 )( _QWORD );
	_BYTE gap6D8[ 1072 ];
	void( __fastcall* pfuncB08 )( __int64, _QWORD );
};

void Hooks::hkSortSceneObjects( __int64 a1, __int64 _pSceneLayer, __int64 _pDrawMesh, unsigned int nDataCount )
{

	static const char szInvisLatexVmatBuffer[ ] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_BUFFERING = 1
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

	static const char szLatexVmatBuffer[ ] = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    shader = "csgo_character.vfx"
    F_DISABLE_Z_PREPASS = 1
    F_DISABLE_Z_WRITE = 1
    F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

    g_vColorTint = [1.0, 1.0, 1.0, 1.0]
    g_bFogEnabled = 0
    g_flMetalness = 0.000
    g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
    g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
    g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
    g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
} )";

	const auto oOriginal = Detours::SortSceneObjects.GetOriginal<decltype( &hkSortSceneObjects )>( );

	CSceneLayer* pSceneLayer = ( CSceneLayer* )_pSceneLayer;
	CDrawMesh* pDrawMesh = ( CDrawMesh* )_pDrawMesh;

	if ( pSceneLayer->pfuncB08 )
		pSceneLayer->pfuncB08( _pDrawMesh, nDataCount );

	for ( int i = 0; i < nDataCount; ++i )
	{
		CDrawMesh& mesh = pDrawMesh[ i ];

		if ( !mesh.m_pSceneAnimatableObject )
			continue;

		auto pOwner = mesh.m_pSceneAnimatableObject->m_hOwnerHandle.Get( );
		if ( !pOwner )
			continue;

		if ( pOwner == Globals::m_pLocalPlayerPawn )
			continue;

		if ( mesh.m_pMaterial )
			if ( CRT::StringCompare( mesh.m_pMaterial->GetName( ), X( "materials/dev/glowproperty.vmat" ) ) == 0 )
				return;

		bool bIsBulletBeam = pSceneLayer->m_nDrawFlag & 1 || HIBYTE( pSceneLayer->m_nDrawFlag ) & 1;
		if ( bIsBulletBeam )
			continue;

		SchemaClassInfoData_t* pSchemaData;
		pOwner->GetSchemaClassInfo( &pSchemaData );
		if ( !pSchemaData )
			continue;

		if ( FNV1A::Hash( pSchemaData->szName ) == FNV1A::HashConst( "C_CSPlayerPawn" )
			 )
		{
			static const auto material = g_Chams->CreateMaterial( "testmat", szInvisLatexVmatBuffer );
			mesh.m_pMaterial = material;
			mesh.m_colValue = ( pSceneLayer->m_nDrawFlag & 2 ) ? Color( 255, 0, 0 ) : Color( 0, 255, 0 );;

		}
	}

	return oOriginal( a1, _pSceneLayer, _pDrawMesh, nDataCount );
}

void Hooks::hkGeneratePrimitives( CAnimatableSceneObjectDesc* pDesc, CSceneAnimatableObject* pObject, void* a3, CMeshPrimitiveOutputBuffer* pRenderBuffer )
{
	const auto oOriginal = Detours::GeneratePrimitives.GetOriginal<decltype( &hkGeneratePrimitives )>( );

	if ( Interfaces::m_pEngine->IsInGame( ) && Interfaces::m_pEngine->IsConnected( ) )
		if ( g_Chams->OnGeneratePrimitives( pDesc, pObject, a3, pRenderBuffer ) )
			return;

	oOriginal( pDesc, pObject, a3, pRenderBuffer );
}

void FASTCALL Hooks::hkDrawArray( CAnimatableSceneObjectDesc* pAnimatableSceneObjectDesc, void* pDx11, CDrawMesh* pMaterialData, int nDataCount, void* pSceneView, void* pSceneLayer, void* pUnk, void* pUnk2 )
{
	const auto oDrawObject = Detours::DrawArray.GetOriginal<decltype( &hkDrawArray )>( );
	if ( !Interfaces::m_pEngine->IsConnected( ) || !Interfaces::m_pEngine->IsInGame( ) )
		return 	ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2 );

	if ( !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerPawn )
		return 	ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2 );

	ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pSceneSystemGadget, oDrawObject, pAnimatableSceneObjectDesc, pDx11, pMaterialData, nDataCount, pSceneView, pSceneLayer, pUnk, pUnk2 );
}

void Hooks::hkDrawArrayLight( void* pLightBinnerGPU, CAggregateSceneObject* pAggregateSceneObject, void* pUnk )
{
	const auto oOriginal = Detours::DrawArrayLight.GetOriginal<decltype( &hkDrawArrayLight )>( );

	if ( !Config::b( g_Variables.m_WorldVisuals.m_bWorldModulation ) )
		return oOriginal( pLightBinnerGPU, pAggregateSceneObject, pUnk );

	pAggregateSceneObject->red = Config::c( g_Variables.m_WorldVisuals.m_colLighting ).r( ) / 255.f;
	pAggregateSceneObject->green = Config::c( g_Variables.m_WorldVisuals.m_colLighting ).g( ) / 255.f;
	pAggregateSceneObject->blue = Config::c( g_Variables.m_WorldVisuals.m_colLighting ).b( ) / 255.f;

	oOriginal( pLightBinnerGPU, pAggregateSceneObject, pUnk );
}

void Hooks::hkUpdateLightShadowMap( CLightBinnerGpu* pLightBinnerGPU, void* a2 )
{
	static const auto fnOriginal = Detours::UpdateLightShadowMap.GetOriginal<decltype( &hkUpdateLightShadowMap )>( );

	if ( Config::b( g_Variables.m_WorldVisuals.m_bModifyLight ) )
	{
		Vector vCustomDirection = {
			static_cast<float>( Config::i( g_Variables.m_WorldVisuals.m_iLightPitch ) ),
			static_cast<float>( Config::i( g_Variables.m_WorldVisuals.m_iLightYaw ) ),
			static_cast<float>( Config::i( g_Variables.m_WorldVisuals.m_iLightRoll ) )
		};
		vCustomDirection.Normalize( );

		if ( CSceneLightObject* pLightObject = *pLightBinnerGPU->m_ppSceneLightObject )
			pLightObject->m_Data.m_vDirectionNormalized = vCustomDirection;
	}

	fnOriginal( pLightBinnerGPU, a2 );
}

void Hooks::hkCreateAnimatableSceneObject( CModelState* pModelState, void* a2, void* a3 )
{
	const auto oOriginal = Detours::CreateAnimatableSceneObject.GetOriginal<decltype( &hkCreateAnimatableSceneObject )>( );
	oOriginal( pModelState, a2, a3 );

	void* pAnimSceneObject = (void*)( ( __int64 )pModelState + 0xd8 );

	int breakpointhere = 0;
	return;
}

void Hooks::hkSkyboxDrawarray( __int64 pObjectDesc, __int64 a2, __int64 pSceneData, int iDataCount, int a5, __int64 a6, __int64 a7 )
{
	static auto original = Detours::SkyboxDrawarray.GetOriginal<decltype( &hkSkyboxDrawarray )>( );

	bool bUseDefaultSkybox = Config::b(g_Variables.m_WorldEffects.m_bCustomSkybox);

	if (!bUseDefaultSkybox)
		return original( pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7 );

	struct CSkyboxSceneObject {
		MEM_PAD( 0x110 );
		__int64*** m_pFaceData;
		MEM_PAD( 0x30 );
	};
	static_assert( sizeof( CSkyboxSceneObject ) == 0x148 );

	if ( iDataCount > 0 ) {
		auto pSceneSkyboxObject = *( CSkyboxSceneObject** )( 0x68 * ( iDataCount )+( pSceneData )-0x50 );
		if ( pSceneSkyboxObject ) {

			Color c = Config::c( g_Variables.m_WorldEffects.m_colSkybox );
			Vector skycol = Vector(
				static_cast< float >( c.r( ) ) / 255.f,
				static_cast< float >( c.g( ) ) / 255.f,
				static_cast< float >( c.b( ) ) / 255.f
			);
			*( Vector* )( ( __int64 )pSceneSkyboxObject + 0x100) = skycol;

		}
	}

	original( pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7 );
}

class CParticleMgrr {
private:
	void* m_pParticles = nullptr;
public:
	void SetColor(const char* szType, Color colParticle, int nIndex) {
		if (m_pParticles) {
			const auto pParticle = (__int64)m_pParticles + 0x50 + nIndex * 0x68;
			const auto pParticleParent = *(__int64*)(pParticle - 0x50);
			const auto pParticleNamePointer = *(__int64*)(pParticleParent + 0x18);

			char* szParticleName = (*(char**)(*(void***)(pParticleNamePointer + 0x8)));

			if (strstr(szParticleName, szType)) {
				*(Color*)(pParticle) = colParticle;
			}
		}
	}

	CParticleMgrr(void* pParticles) {
		m_pParticles = pParticles;
	}
};

void* Hooks::hkParticleDrawArray(void* pParticleObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7)
{
	const auto pOriginal = Detours::ParticleDrawArray.GetOriginal<decltype(&hkParticleDrawArray)>();

	if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected())
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	if (!Globals::m_pLocalPlayerPawn)
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	if (!a3 || nDataCount <= 0)
		return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);

	CParticleMgrr mgr(a3);
	for (int i = 0; i < nDataCount; i++) {
		if (Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation)) {
			Color colInferno = Config::c(g_Variables.m_WorldVisuals.m_colParticleColor);
			mgr.SetColor(X("molotov"), colInferno, i);
			mgr.SetColor(X("incendiary"), colInferno, i);
		}

		if (Config::b(g_Variables.m_Visuals.m_bShockEffect)) {
			mgr.SetColor(X("impact_taser_bodyfx"), Config::c(g_Variables.m_Visuals.m_colShockEffect), i);
		}

		if (Config::b(g_Variables.m_WorldVisuals.m_bGrenadeModulation)) {
			mgr.SetColor(X("explosion_basic"), Config::c(g_Variables.m_WorldVisuals.m_colGrenade), i);
		}

		if (Config::b(g_Variables.m_WorldVisuals.m_bTaserStimModulation)) {
			Color colStim = Config::c(g_Variables.m_WorldVisuals.m_colTaserStim);
			mgr.SetColor(X("weapon_tracers_taser"), colStim, i);
			mgr.SetColor(X("weapon_taser_glow"), colStim, i);
			mgr.SetColor(X("weapon_taser_flash"), colStim, i);
			mgr.SetColor(X("weapon_muzzle_flash_taser"), colStim, i);
		}
	}

	return pOriginal(pParticleObjectDesc, a2, a3, nDataCount, a5, a6, a7);
}

uint64_t ValveHash( const char* szVar, int32_t iSeed ) {

	return reinterpret_cast< uint64_t( __fastcall* )( const char*, int32_t ) >( Memory::ResolveRelativeAddress( Memory::FindPattern( CLIENT_DLL, X( "E8 ? ? ? ? 8B D0 48 8D 0D ? ? ? ? E8 ? ? ? ? 0F 10 45 ? 69 C8" ) ), 0x1, 0x4 ) )( szVar, iSeed );
}

void Hooks::hkSetPostProcessVec( void* a1, unsigned int uHash, Vector4D* vVal )
{
	const auto pOriginal = Detours::SetPostProcessVec.GetOriginal<decltype( &hkSetPostProcessVec )>( );
	{
		int v120 = ValveHash( X( "DofRanges" ), 0x3141592F );
		unsigned int v121 = ( 0x5BD1E995 * ( v120 ^ 0x73 ) ) ^ ( ( 0x5BD1E995 * ( v120 ^ 0x73u ) ) >> 13 );
		unsigned int uDOFParamHash = ( 0x5BD1E995 * v121 ) ^ ( ( 0x5BD1E995 * v121 ) >> 15 );
		auto vValue = Vector4D( 0.f, 180.f, -100.f, 2000.f );
		pOriginal( a1, uDOFParamHash, &vValue );
	}

	return pOriginal( a1, uHash, vVal );;
}

int Hooks::hkPlayGunSound( int a1, void* a2, int a3, __int64 a4, int a5, int a6, int a7 )
{
	const auto oOriginal = Detours::PlayGunSound.GetOriginal<decltype( &hkPlayGunSound )>( );

	return oOriginal( a1, a2, a3, a4, a5, a6, a7 );
}

class EmitSound_t
{
public:
	MEM_PAD( 0x8 );
	const char* m_szSoundName;
	float m_flVolume;
};

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter( ) {}

	virtual bool	IsReliable( void ) const = 0;
	virtual bool	IsInitMessage( void ) const = 0;

	virtual int		GetRecipientCount( void ) const = 0;
	virtual int		GetRecipientIndex( int slot ) const = 0;
};

_DWORD* Hooks::hkEmitSound( _DWORD* a1, __int64 a2, unsigned int iEntIndex, __int64 pEmitSound )
{
	const auto oOriginal = Detours::EmitSound.GetOriginal<decltype( &hkEmitSound )>( );

	IRecipientFilter* pFilter = ( IRecipientFilter* )a2;
	EmitSound_t* pESound = ( EmitSound_t* )pEmitSound;

	std::cout << "-------------" << std::endl;
	std::cout << "GetRecipientCount: " << pFilter->GetRecipientCount( ) << std::endl;
	std::cout << "Entity Index: " << iEntIndex << std::endl;
	std::cout << "Sound Name: " << pESound->m_szSoundName << std::endl;
	std::cout << "Sound Volume: " << pESound->m_flVolume << std::endl;
	std::cout << "-------------" << std::endl;

	std::string strSoundName = pESound->m_szSoundName;

	if ( strSoundName.contains( "flash" ) )
	{
		pESound->m_szSoundName = "";
		pESound->m_flVolume = 0.f;
	}

	return oOriginal( a1, a2, iEntIndex, ( __int64 )pESound );
}

__int64 Hooks::hkSomeUtlSymbolFunc( __int64 pVSoundTypeManager, int a2 )
{
	const auto oOriginal = Detours::SomeUtlSymbolFunc.GetOriginal<decltype( &hkSomeUtlSymbolFunc )>( );

	const __int64 result = oOriginal( pVSoundTypeManager, a2 );

	const char* szResult = ( const char* )result;

	if ( strstr( szResult, "sounds/weapons/ssg08/ssg08_01.vsnd" )
		&& Config::b( g_Variables.m_Misc.m_bDistantScoutSound )
		)
	{
		szResult = "sounds/weapons/ssg08/ssg08_distant.vsnd";
	}

	return ( __int64 )szResult;
}

double Hooks::hkBloodEffectHit( void* pUnk )
{
	return -1.0;
}

void Hooks::hkSomeRainShit( void* pPrecipitation, void* pLocalEntity, int nSomethingAlways0 )
{
	Detours::SomeRainShit.GetOriginal<decltype( &hkSomeRainShit )>( )( nullptr, nullptr, 0 );
}

void FASTCALL Hooks::hkPlayVSound( const char* szSound )
{
	const auto oOriginal = Detours::PlayVSound.GetOriginal<decltype( &hkPlayVSound )>( );

}

void Hooks::hkDecreaseSpeed( void* pMovementServices, void* pUnk )
{

}

void* Hooks::hkDrawParticleChildrenSeparateFromParents( void* unkptr, void* pDrawArray )
{
	const auto pOriginal = Detours::DrawParticleChildrenSeparateFromParents.GetOriginal<decltype( &hkDrawParticleChildrenSeparateFromParents )>( );

	return pOriginal( unkptr, pDrawArray );
}

CParticleColor Hooks::hkGetParticleColor( CParticleColor pOutputColor, unsigned __int8* pOriginalParticleColor )
{
	const auto pOriginal = Detours::GetParticleColor.GetOriginal<decltype( &hkGetParticleColor )>( );

	if ( !Config::b( g_Variables.m_WorldVisuals.m_bParticleModulation ) )
		return pOriginal( pOutputColor, pOriginalParticleColor );

	return pOriginal( pOutputColor, pOriginalParticleColor );
}

void* Hooks::hkAODrawArray( void* pAmbientOcclusionProxySceneObjectDesc, void* a2, void* a3, int nDataCount, void* a5, void* a6, void* a7 )
{
	const auto pOriginal = Detours::AODrawArray.GetOriginal<decltype( &hkAODrawArray )>( );

	if ( !Config::b( g_Variables.m_WorldVisuals.m_bParticleModulation ) )
		return pOriginal( pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7 );

	if ( !a3 || nDataCount <= 0 )
		return pOriginal( pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7 );

	int objectCount = nDataCount;

	for ( int i = 0; i < objectCount; ++i )
	{
		uintptr_t objectData = reinterpret_cast< uintptr_t >( a3 ) + i * 104;

	}

	return pOriginal( pAmbientOcclusionProxySceneObjectDesc, a2, a3, nDataCount, a5, a6, a7 );
}

void* FASTCALL Hooks::hkFlashOverlay( __int64 a1, int a2, __int64* a3, __int64 a4, float a5[ 4 ] )
{
	const auto oFlashOverlay = Detours::FlashOverlay.GetOriginal<decltype( &hkFlashOverlay )>( );

	if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_FLASH ) )
		return nullptr;

	return oFlashOverlay( a1, a2, a3, a4, a5 );
}

void* Hooks::hkCalcViewmodel( float* flUnk, float* flOffsets, float* flFov )
{
	const auto pOriginal = Detours::CalcViewmodel.GetOriginal<decltype( &hkCalcViewmodel )>( );

	void* pReturn = pOriginal( flUnk, flOffsets, flFov );

	if ( !Config::b( g_Variables.m_Visuals.m_bViewmodelChanger ) )
		return pOriginal( flUnk, flOffsets, flFov );

	float flXOffset = Config::i( g_Variables.m_Visuals.iXOffset );
	float flYOffset = Config::i( g_Variables.m_Visuals.iYOffset );
	float flZOffset = Config::i( g_Variables.m_Visuals.iZOffset );
	float flXRotation = Config::i( g_Variables.m_Visuals.iXRotation );
	float flYRotation = Config::i( g_Variables.m_Visuals.iYRotation );
	float flZRotation = Config::i( g_Variables.m_Visuals.iZRotation );
	float flViewmodelFov = Config::i( g_Variables.m_Visuals.iViewmodelFov );

	flOffsets[ 0 ] += flXOffset;
	flOffsets[ 1 ] += flYOffset;
	flOffsets[ 2 ] += flZOffset;

	*flFov = flViewmodelFov;

	QAngle* rot = ( QAngle* )( ( __int64 )flOffsets + 0x20 );

	rot->x += flXRotation;
	rot->y += flYRotation;
	rot->z += flZRotation;

	rot->Normalize( );

	return pReturn;
}

__int64 Hooks::hkGetInterpolatedShootPosition( CPlayer_WeaponServices* pWeaponServices, Vector* vecEyePosition, int nShootTick, float flFraction )
{
	static auto oOriginal = Detours::GetInterpolatedShootPosition.GetOriginal<decltype( &hkGetInterpolatedShootPosition )>( );

	auto result = oOriginal( pWeaponServices, vecEyePosition, nShootTick, flFraction );;

	return result;
}

static bool FASTCALL Hooks::hkRemoveLegs( C_CSPlayerPawn pPawn )
{
	const auto oRemoveLegs = Detours::RemoveLegs.GetOriginal<decltype( &hkRemoveLegs )>( );

	if ( !Interfaces::m_pEngine->IsConnected( ) || !Interfaces::m_pEngine->IsInGame( ) )
		return oRemoveLegs( pPawn );;

	if ( Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_LOCAL_LEGS ) && !Config::b( g_Variables.m_Visuals.m_bEnableThirdPerson ) )
		return false;

	return oRemoveLegs( pPawn );
}

void FASTCALL Hooks::hkAllowCameraAngleChange( CCSGOInput* pCSGOInput, int a2 )
{
	const auto Original = Detours::AllowCameraAngleChange.GetOriginal<decltype( &hkAllowCameraAngleChange )>( );

	QAngle angViewAngles = pCSGOInput->GetViewAngles( );

	Original( pCSGOInput, a2 );

	pCSGOInput->SetViewAngle( angViewAngles );
}

void* FASTCALL Hooks::hkIsRelativeMouseMode( void* pThisptr, bool bActive )
{
	const auto oIsRelativeMouseMode = Detours::IsRelativeMouseMode.GetOriginal<decltype( &hkIsRelativeMouseMode )>( );

	Gui::m_bInputActive = bActive;
	return oIsRelativeMouseMode( pThisptr, Gui::m_bOpen ? false : bActive );
}

float s_flLastUpdatedRealTime = 0.f;
float s_flOldSmoothedValue = 0.f;

void FASTCALL Hooks::hkOnRenderStart( CViewRender* pViewRender )
{
	const auto oOnRenderStart = Detours::OnRenderStart.GetOriginal<decltype( &hkOnRenderStart )>( );

	oOnRenderStart( pViewRender );

	if ( !Interfaces::m_pGlobalVariables )
		return;

	{

		static CTimer TimerObj( true );
		static int nFrameCount = 0;
		nFrameCount++;

		float flElapsed = TimerObj.Elapsed( );
		if ( flElapsed > 1.f )
			Gui::m_iAverageFPS = static_cast<int>( nFrameCount / ( flElapsed / 1000.0f ) );
		if ( flElapsed > 1000.0f )
		{
			nFrameCount = 0;
			TimerObj.Reset( );
		}
	}

	return Features::OnRenderStart( pViewRender );
}

float FASTCALL Hooks::hkSetWorldFov( void* pRcx )
{
	const auto oGetFOV = Detours::SetWorldFov.GetOriginal<decltype( &hkSetWorldFov )>( );
	float flOriginalFOV = oGetFOV( pRcx );

	if ( !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive( ) )
		return flOriginalFOV;
	if ( !Globals::m_pLocalPlayerPawn )
		return flOriginalFOV;

	bool bShouldChangeWorldFov = Config::i( g_Variables.m_Visuals.m_iWorldFov ) != 79;

	float flWorldFov = Config::i( g_Variables.m_Visuals.m_iWorldFov );
	bool bIsScoped = Globals::m_pLocalPlayerPawn->m_bIsScoped( );

	if ( bShouldChangeWorldFov && Globals::m_pLocalPlayerPawn && !bIsScoped )
	{
		flOriginalFOV = flWorldFov;

		if ( Config::b( g_Variables.m_Visuals.m_bOverrideWorldFov ) )
		{
			Vector vecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
			float flSpeed = vecVelocity.Length2D();

			const float flMaxSpeed = 250.0f;

			float flSpeedRatio = std::clamp( flSpeed / flMaxSpeed, 0.0f, 1.0f );

			int iMaxOverride = Config::i( g_Variables.m_Visuals.m_iOverrideWorldFovMax );
			float flOverrideValue = flSpeedRatio * static_cast<float>( iMaxOverride );

			flOriginalFOV += flOverrideValue;
		}
	}

	if ( bIsScoped && Config::b( g_Variables.m_Visuals.m_bRemoveScopeFov ) && LocalPlayerData::m_pWeapon )
	{
		float flScopedPercentage = Config::i( g_Variables.m_Visuals.m_iScopedFov ) / 100.0f;
		float flScopedFov = flOriginalFOV;
		float flNormalFov = bShouldChangeWorldFov ? flWorldFov : flOriginalFOV;

		flOriginalFOV = flScopedFov + flScopedPercentage * ( flNormalFov - flScopedFov );
	}

	return flOriginalFOV;
}

void* Hooks::hkDrawParticleEffect( const char* szEffect, __int64 a2, __int64* angSomeAngles_1, __int64 a4, char a5, unsigned int a6, __int64 a7, bool a8, __int64 a9, __int64 a10, __int64 a11, __int64 a12 )
{
	auto oOriginal = Detours::DrawParticleEffect.GetOriginal<decltype( &hkDrawParticleEffect )>( );

	if ( szEffect == "fx_blood_bullet_impact" )
		return nullptr;

	return oOriginal( szEffect, a2, angSomeAngles_1, a4, a5, a6, a7, a8, a9, a10, a11, a12 );
}

void* FASTCALL Hooks::hkDrawSmokeVertex( void* a1, void* a2, int a3, int a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10 )
{
	const auto oOriginal = Detours::DrawSmokeVertex.GetOriginal<decltype( &hkDrawSmokeVertex )>( );

	return Config::vb( g_Variables.m_WorldVisuals.m_vbRemovals ).at( ERemovals::REMOVE_SMOKE ) ? nullptr : oOriginal( a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 );
}

const char* FASTCALL Hooks::hkGetInfernoType( __int64 a1 )
{

	return "";
}

void Hooks::hkMergeBones( CSkeletonInstance* pSkeletonInstance, uint32_t iUnk ) 
{
	const auto oMergeBones = Detours::MergeBones.GetOriginal<decltype( &hkMergeBones )>( );

	if ( !Globals::m_pLocalPlayerPawn )
		return oMergeBones( pSkeletonInstance, iUnk );

	bool bIsLocal = pSkeletonInstance->m_pOwner( ) == Globals::m_pLocalPlayerPawn->m_pGameSceneNode( )->GetSkeletonInstance( )->m_pOwner( );

	if ( iUnk == EBoneFlags::FLAG_HITBOX )
		return;

	return oMergeBones( pSkeletonInstance, iUnk );
}

void Hooks::hkRunCommand( CCSPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd )
{
	const auto oRunCommand = Detours::RunCommand.GetOriginal<decltype( &hkRunCommand )>( );

	oRunCommand( pMovementServices, pUserCmd );
}

void* Hooks::hkUpdateSkybox( ISceneSystem* pSceneSystem, CMaterial2** pMaterial, void* pSceneWorld )
{
	return Detours::UpdateSkybox.GetOriginal<decltype( &hkUpdateSkybox )>( )( pSceneSystem, pMaterial, pSceneSystem );
}

void FASTCALL Hooks::hkNiggerKickFunction( __int64 a1, unsigned int a2 )
{
	const auto original = Detours::NiggerKickFunction.GetOriginal<decltype( &hkNiggerKickFunction )>( );

	if ( Config::b( g_Variables.m_Misc.m_bRemoveNoSteamLogonKick ) )
		return;

	return original( a1, a2 );
}

class CAnimGraphModelBinding
{
public:
	void* vtbl;
	const char* m_szModelName;
	void* m_pAnimUpdateSharedData;
	void* m_pAnimUpdateInstanceData;
	void* m_pAnimNetVarRegistry;
};

void* FASTCALL Hooks::hkDisableServerAnimation( void* a1, void* a2, void* a3 )
{
	const auto oDisableServerAnimation = Detours::DisableServerAnimation.GetOriginal<decltype( &hkDisableServerAnimation )>( );

	if ( !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive( ) )
		return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pAnimationSystemGadget, oDisableServerAnimation, a1, a2, a3 );

	CAnimGraphModelBinding* pAnimGraphModelBinding = ( CAnimGraphModelBinding* )a1;

	const char* szModelPath = pAnimGraphModelBinding->m_szModelName;

	if ( CRT::StringCompareN( X( "weapons/models/knife/" ), szModelPath, 21 ) == 0 )
		*( __int64* )( ( uintptr_t )a3 + 0x30 ) = 0;

	return ReturnAddressSpoofer::SpoofCall( ReturnAddressSpoofGadgets::m_pAnimationSystemGadget, oDisableServerAnimation, a1, a2, a3 );
}

bool FASTCALL Hooks::hkIsGlowing( CGlowProperty* pGlow )
{
	const auto oIsGlowing = Detours::IsGlowing.GetOriginal<decltype( &hkIsGlowing )>( );

	if ( !pGlow || !pGlow->m_pOwner || !pGlow->m_pOwner->IsPlayerPawn( ) || pGlow->m_pOwner == Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController->m_bPawnIsAlive( ) )
		return oIsGlowing( pGlow );

	if ( pGlow->m_pOwner->m_iHealth( ) <= 0 )
		return oIsGlowing( pGlow );

	if ( pGlow->m_pOwner->m_iTeamNum( ) == Globals::m_pLocalPlayerPawn->m_iTeamNum( ) )
		return oIsGlowing( pGlow );

	return Config::b( g_Variables.m_Visuals.m_bGlow );
}

void FASTCALL Hooks::hkGetGlowColor( CGlowProperty* pGlow, float* pColor )
{
	const auto oGetGlowColor = Detours::GetGlowColor.GetOriginal<decltype( &hkGetGlowColor )>( );

	if ( !pGlow || !pColor || !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive( ) )
		return oGetGlowColor( pGlow, pColor );

	if ( pGlow->m_pOwner == Globals::m_pLocalPlayerPawn )
		return oGetGlowColor( pGlow, pColor );

	Color colColor = Config::c( g_Variables.m_Visuals.m_colGlow );
	pColor[ 0 ] = colColor.r( ) / 255.f;
	pColor[ 1 ] = colColor.g( ) / 255.f;
	pColor[ 2 ] = colColor.b( ) / 255.f;
	pColor[ 3 ] = colColor.a( ) / 255.f;
}
void Hooks::hkUpdatePostProcessing(C_PostProcessingVolume* a1, int a2) {
	const static auto original = Detours::UpdatePostProcessing.GetOriginal<decltype(&hkUpdatePostProcessing)>();

	original(a1, a2);

	g_WorldModulation->Exposure(a1);
}

bool m_bConsoleLog = false;

int Hooks::hkConsoleLog(__int64 a1, int channel, int serverity, __int64 color_mb, const char* text, __int64* a6)
{
	if (m_bConsoleLog)
	{
		const static auto original = Detours::ConsoleLog.GetOriginal<decltype(&hkConsoleLog)>();
		return original(a1, channel, serverity, color_mb, text, a6);
	}
	return 0;
}