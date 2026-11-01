#include "Precompiled.h"

#define ProductStringVersion X("1.40.1.5")

DWORD WINAPI OnDllAttach(LPVOID lpParameter)
{
    while (!Memory::GetModuleBaseHandle(NAVSYSTEM_DLL))
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    try
    {
#ifdef DEBUG_CONSOLE

        if (!Logging::Attach(X("cs2")))
            throw std::runtime_error(X("failed to attach console"));

        Logging::Print(X("console opened"));

        Logging::ofsFile.open(Config::GetWorkingPath().append(X("RyzeXTR.log")), std::ios::out | std::ios::trunc);
#endif

        if (!Config::Setup(X("Default.chlen")))
            throw std::runtime_error(X("failed to setup config"));

        if (!Memory::Setup())
            throw std::runtime_error(X("failed to setup memory"));

        if (!Math::Setup())
            throw std::runtime_error(X("failed to setup"));

        if (!Functions::Setup())
            throw std::runtime_error(X("failed to setup functions"));

        if (!ReturnAddressSpoofGadgets::FindGadgets())
            throw std::runtime_error(X("failed to find gadgets"));

        if (!Interfaces::Setup())
            throw std::runtime_error(X("failed to get interfaces"));

        if (!Schema::Setup())
            throw std::runtime_error(X("failed to dump schema"));

        if (!Convar::Setup())
            throw std::runtime_error(X("failed to setup convars"));

        if (!Input::Setup())
            throw std::runtime_error(X("failed to setup inputs"));

        if (!Draw::m_bInitialized)
            Draw::Setup(Interfaces::m_pDevice, Interfaces::m_pDeviceContext);

        g_Chams->Init();

        if (!Hooks::Setup())
            throw std::runtime_error(X("failed to setup hooks"));

        Interfaces::m_pCvar->UnlockHiddenCVars();

        Utilities::m_EventListener.Setup({ X("round_start"), X("add_bullet_hit_marker"), X("bullet_impact"), X("player_hurt"), X("player_death"), X("weapon_fire"), X("vote_cast"), X("vote_started"), X("item_purchase"), X("bomb_defused"), X("bomb_begindefuse"), X("bomb_planted"), X("bomb_beginplant")});

    }
    catch (const std::exception& ex)
    {

        Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
        Logging::Print(X("[error] {}"), ex.what());
        Logging::PopConsoleColor();

#ifdef _DEBUG

        _RPT0(_CRT_ERROR, ex.what());
#else

        LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(lpParameter), EXIT_FAILURE);
#endif
    }

    return EXIT_FAILURE;
}

DWORD WINAPI OnDllDetach(LPVOID lpParameter)
{
    while (!Input::IsKeyDown(Config::i(g_Variables.m_Gui.m_iUnloadKey)) && !Globals::m_bIsUnloading)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Globals::m_bIsUnloading = true;

    Gui::m_bOpen = false;
    Gui::m_bInitialized = false;

    Utilities::m_EventListener.Destroy();

    Draw::Destroy();
    Input::Restore();
    Hooks::Destroy();

#ifdef DEBUG_CONSOLE

    Logging::Detach();
#else

    if (Logging::ofsFile.is_open())
        Logging::ofsFile.close();
#endif

    LI_FN(FreeLibraryAndExitThread)((HMODULE)lpParameter, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {

        LI_FN(DisableThreadLibraryCalls)(hModule);

        Globals::m_hDll = hModule;

        if (HANDLE hThread = LI_FN(CreateThread)(nullptr, 0U, OnDllAttach, hModule, 0UL, nullptr); hThread != nullptr)
            LI_FN(CloseHandle)(hThread);

        if (HANDLE hThread = LI_FN(CreateThread)(nullptr, 0U, OnDllDetach, hModule, 0UL, nullptr); hThread != nullptr)
            LI_FN(CloseHandle)(hThread);

        return TRUE;
    }

    return FALSE;
}