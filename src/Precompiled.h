#pragma once

#ifndef CS_DEBUG_BREAK
#define CS_DEBUG_BREAK() __debugbreak()
#endif

#ifndef CS_ASSERT
#ifdef _DEBUG
#define CS_ASSERT(EXPRESSION) static_cast<void>(!!(EXPRESSION) || (CS_DEBUG_BREAK(), 0))
#else

#define CS_ASSERT(EXPRESSION) static_cast<void>(0)
#endif
#endif

#ifndef CS_PARANOID
#ifdef _DEBUG
#define CS_PARANOID
#endif
#endif

#define CS_CLASS_NO_CONSTRUCTOR(CLASS)	\
CLASS() = delete;						\
CLASS(CLASS&&) = delete;				\
CLASS(const CLASS&) = delete;

#define CS_CLASS_NO_ASSIGNMENT(CLASS)	\
CLASS& operator=(CLASS&&) = delete;		\
CLASS& operator=(const CLASS&) = delete;

#define CS_CLASS_NO_INITIALIZER(CLASS)	\
CS_CLASS_NO_CONSTRUCTOR(CLASS)			\
CS_CLASS_NO_ASSIGNMENT(CLASS)

#define CS_CLASS_NO_ALLOC()								\
void* operator new(const std::size_t nSize) = delete;	\
void operator delete(void* pMemory) = delete;

#include <ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <xstring>
#include <string>
#include <chrono>
#include <thread>
#include <cstdint>
#include <string_view>
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <any>
#include <array>
#include <format>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <cmath>
#include <limits>
#include <map>
#include <random>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <utility>
#include <minwindef.h>

#include <shlobj_core.h>
#include <KnownFolders.h>

#include <d3d11.h>
#include <D3D11.h>
#include <DirectXMath.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_freetype.h>
#include <cpp/imgui_stdlib.h>

#include <json.hpp>

#include <LazyImporter.h>

#include "utilities/CRT.h"

#include "memory/crypt/XorStr.h"
#include "memory/crypt/FNV1A.h"
#include "memory/crypt/CSHA1.h"
#include "memory/crypt/MurMur.h"

#include <steamtypes.h>
#include <isteamgamecoordinator.h>
#include <isteammatchmaking.h>
#include <isteamfriends.h>
#include <isteamclient.h>
#include <isteamutils.h>
#include <isteamuser.h>

#include "utilities/Logging.h"

#include "memory/datatypes/Pe64.h"
#include "memory/RetSpoof.h"
#include "memory/Memory.h"

#include "valve/interface list/IMemAlloc.h"
#include "valve/Interfaces.h"

#include "utilities/Math.h"
#include "utilities/IdaDefs.h"

#include "memory/datatypes/Vector.h"
#include "memory/datatypes/Matrix.h"
#include "memory/datatypes/Quaternion.h"
#include "memory/datatypes/QAngle.h"
#include "memory/datatypes/Color.h"
#include "memory/datatypes/CBaseHandle.h"
#include "memory/datatypes/CStrongHandle.h"
#include "memory/datatypes/KeyValues3.h"
#include "memory/datatypes/CUserCmd.h"

#include "memory/datatypes/CThreadMutex.h"

#include "memory/datatypes/CThreadSpinMutex.h"

#include "memory/datatypes/CThreadSpinRWLock.h"

#include "memory/datatypes/CInterlockedInt.h"

#include "memory/datatypes/CTSList.h"

#include "memory/datatypes/CUtlMemory.h"
#include "memory/datatypes/CUtlFixedMemory.h"

#include "memory/datatypes/CUtlVector.h"

#include "memory/datatypes/CUtlMemoryPoolBase.h"

#include "memory/datatypes/CUtlHash.h"

#include "memory/datatypes/CUtlLinkedList.h"

#include "memory/datatypes/CUtlString.h"

#include "memory/datatypes/CUtlMap.h"

#include "memory/datatypes/CUtlSymbolLarge.h"

#include "memory/datatypes/CUtlStringToken.h"

#include "memory/datatypes/QuantanizedDataTypes.h"

#include "Globals.h"
#include "config/Config.h"
#include "config/Variables.h"
#include "valve/Enums.h"
#include "valve/Definitions.h"
#include "valve/Functions.h"
#include "utilities/InputSytem.h"

#include "valve/Trace.h"

#include "valve/interface list/CLagCompensationManager.h"
#include "valve/interface list/ISource2Client.h"
#include "valve/interface list/ISchemaSystem.h"
#include "valve/interface list/ISwapChainDX11.h"
#include "valve/interface list/IEngineClient.h"
#include "valve/interface list/IGameResourceervice.h"
#include "valve/interface list/IEngineCvar.h"
#include "valve/interface list/IInputSystem.h"
#include "valve/interface list/IInputService.h"
#include "valve/interface list/IViewRender.h"
#include "valve/interface list/INetworkClientService.h"
#include "valve/interface list/IMaterialSystem.h"
#include "valve/interface list/IClientModeShared.h"
#include "valve/interface list/IResourceSystem.h"
#include "valve/interface list/IGameUIService.h"
#include "valve/interface list/IParticleSystem.h"
#include "valve/interface list/ISceneSystem.h"

#include "valve/interface list/CPrediction.h"
#include "valve/interface list/GlobalVariables.h"
#include "valve/interface list/CCSGOInput.h"
#include "valve/interface list/CGameEntitySystem.h"
#include "valve/interface list/CGameTraceManager.h"
#include "valve/interface list/CGameEvent.h"
#include "valve/interface list/CEventManager.h"
#include "valve/interface list/CLocalize.h"
#include "valve/interface list/CDebugOverlayGameSystem.h"
#include "valve/interface list/CCSGameRules.h"

#include "valve/hudpanels/CCSGOMapOverview.h"

#include "valve/cgcclient/CGCClient.h"
#include "valve/cgcclient/CGCClientSystem.h"
#include "valve/cgcclient/CGCClientSharedObjectCache.h"
#include "valve/cgcclient/CGCClientSharedObjectTypeCache.h"

#include "valve/playerinventory/CCSPlayerInventory.h"
#include "valve/playerinventory/CCSPlayerInventoryManager.h"

#include "valve/econ/CEconItemSchema.h"
#include "valve/econ/CEconItemSystem.h"
#include "valve/econ/CEconItemDefinition.h"
#include "valve/econ/CEconItem.h"

#include "valve/Convars.h"
#include "valve/Schema.h"
#include "valve/Entity.h"

#include "utilities/Utilities.h"
#include "utilities/ResourceManager.h"

#include "utilities/Draw.h"

#include "gui/FontAwesome.h"
#include "gui/DX11BlurEffect.h"
#include "gui/ChlenGui.h"
#include "gui/SmallestPixel.h"
#include "gui/GunIcons.h"
#include "gui/Gui.h"
#include "gui/Helpers.h"

#include "features/prediction/prediction.h"
#include "features/legitbot/legitbot.h"
#include "features/ragebot/NoSpread.h"
#include "features/ragebot/doubletap.h"
#include "features/ragebot/antiaim.h"
#include "features/ragebot/autowall.h"
#include "features/ragebot/lagcomp.h"
#include "features/ragebot/ragebot.h"
#include "features/ragebot/zeusbot.h"
#include "features/entities/EntityList.h"
#include "features/misc/ChatHandler.h"
#include "features/misc/ShotHandler.h"
#include "features/misc/Movement.h"
#include "features/visuals/inventory.h"
#include "features/visuals/modulation.h"
#include "features/visuals/ParticleManager.h"
#include "features/visuals/GrenadePrediction.h"
#include "features/visuals/Grenade.h"
#include "features/visuals/PlayerESP.h"
#include "features/visuals/scope.h"
#include "features/visuals/chams.h"

#include "features/Features.h"

#include <minhook.h>
#include "hooks/DetourHook.h"
#include "hooks/Hooks.h"

#define WIN32_LEAN_AND_MEAN