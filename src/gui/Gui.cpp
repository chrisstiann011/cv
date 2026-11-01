#include "../Precompiled.h"
#include "ChlenGui.h"
#include "Helpers.h"

constexpr ImVec2 child_pos = { 346, 252 };
constexpr ImVec2 MENU_SIZE = { 680,480 };
constexpr ImColor ORANGE_COLOR = ImColor(255, 165, 0, 255);

const char* arrChamMaterials[] = {
    "Latex",
    "Bloom",
    "Glow",
    "Flat",
    "Ghost",
    "Invisible"
};

const char* arrCustomWeatherEffects[] = {
    "None",
    "Ashes",
    "Snow",
    "Stars",
};
const char* arrScopeNames[] =
{
    "Default",
    "Remove overlay",
    "Static gradient",
    "Remove all",
};
const char* arrSkyboxNames[] = {
    "Sunset",
    "Overcast",
    "Night",
    "Aztec",
    "Jungle",
    "Cloudy",
    "Overpass",
    "Nuke",
    "Office",
    "Anubis",
    "Train",
    "Vertigo"
};

const char* szHitsounds[] = {
    "Coin",
    "Click",
    "Whack"
};

const char* szWatermark[ ] =
{
    "FPS",
    "Ping",
    "Speed",
};

const int iHitsoundCount = sizeof(szHitsounds) / sizeof(szHitsounds[0]);

enum EMenuHitsounds : int {
    COINDROP = 0,
    SHOPMENUCLICK,
    WHACK
};
enum ETabs : int
{
    TABS_RAGEBOT = 0,
    TABS_LEGITBOT,
    TABS_VISUALS,
    TABS_INVENTORY,
    TABS_MISC,
    TABS_CONFIG,
	TABS_COUNT
};

enum ESubTabs_Ragebot : int
{
    SUBTAB_AIMBOT = 0,
    SUBTAB_ANTIAIM
};

enum ESubTabs_Visuals : int
{
    SUBTAB_LOCAL = 0,
    SUBTAB_ENEMY,
    SUBTAB_WORLD
};

ImVec2 child_size1 = ImVec2(324, 193);
ImVec2 child_size = ImVec2(330, 390);
static float separator_thickness = 1.5f;
static ImVec4 separator_color = ImVec4(0.32f, 0.47f, 0.69f, 1.0f);

void Gui::Initialize( unsigned int uFontFlags )
{
    ImGui::StyleColorsDark( );

    ChlenGui::SetStyles( MENU_SIZE );

    ImGuiIO& io = ImGui::GetIO( );

    ImFontConfig imVerdanaConfig = { };
    imVerdanaConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;

    Fonts::Default = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\calibri.ttf" ), 14, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::GunIcons = io.Fonts->AddFontFromMemoryTTF( GunIcons, sizeof( GunIcons ), 12, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::ESP = io.Fonts->AddFontFromMemoryTTF( SmallestPixel, sizeof( SmallestPixel ), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::Verdana = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\Verdana.ttf" ), 14, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::VerdanaBold = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\Verdana.ttf" ), 14, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::VerdanaEsp = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\Verdana.ttf" ), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::SegoeUIBold = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\segoeui.ttf" ), 12, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::SegoeUIBold18 = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\segoeuib.ttf" ), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::SegoeUIBold18_ITALIC = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\seguisbi.ttf" ), 18, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );

    static const ImWchar arrIconRanges[ ] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig imFAFontConfig;
    imFAFontConfig.MergeMode = true;
    imFAFontConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
    Fonts::imFA = io.Fonts->AddFontFromMemoryCompressedTTF( FA_FONT, FA_compressed_size, 15.f, &imFAFontConfig, arrIconRanges );

    m_bInitialized = ImGuiFreeType::BuildFontAtlas( io.Fonts, uFontFlags );
}

void Gui::Update(ImGuiIO& io)
{
    Gui::m_vecScreenSize = io.DisplaySize;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.f;
    style.Alpha = 1.f;
}

Color GetRarityColor(int nRarity, int nID)
{
    if (nID == 0)
        return Color(176, 195, 217, 255);
    else
    {
        switch (nRarity)
        {
        case 0:
            return Color(0, 0, 0, 0);
        case 1:
            return Color(176, 195, 217, 255);
        case 2:
            return Color(94, 152, 217, 255);
        case 3:
            return Color(75, 105, 255, 255);
        case 4:
            return Color(136, 71, 255, 255);
        case 5:
            return Color(211, 44, 230, 255);
        case 6:
            return Color(235, 75, 75, 255);
        case 7:
            return Color(228, 174, 57, 255);
        default:
            return Color(176, 195, 217, 255);
        }
    }
}

static float s_flWatermarkWidth = 0.f;
void Gui::RenderWatermark( )
{
    if ( !Config::b( g_Variables.m_Misc.m_bWatermark ) )
        return;

    struct StatComponent
    {
        int m_iValue;
        const char* szLabel;
        char m_szValueBuffer[ 16 ];
        ImVec2 m_vValueSize;
        ImVec2 m_vLabelSize;
    };

    std::vector<StatComponent> vecComponents;

    if ( Config::vb( g_Variables.m_Misc.m_vbWatermark ).at( EWatermark::WATERMARK_FPS ) )
    {
        vecComponents.push_back( { Gui::m_iAverageFPS, "fps" } );
    }

    if ( Config::vb( g_Variables.m_Misc.m_vbWatermark ).at( EWatermark::WATERMARK_PING ) )
    {
        auto pEngine = Interfaces::m_pEngine;
        if ( pEngine )
        {
            auto pNetChannelInfo = pEngine->GetNetChannelInfo( );
            if ( pNetChannelInfo )
            {
                int iLatency = (int)( pNetChannelInfo->GetNetLatency( ) * 1000.f );
                vecComponents.push_back( { iLatency, "ms" } );
            }
        }
    }

    if ( Config::vb( g_Variables.m_Misc.m_vbWatermark ).at( EWatermark::WATERMARK_SPEED ) )
    {
        int iSpeed = 0;

        if ( C_CSPlayerPawn* pPawn = Globals::m_pLocalPlayerPawn )
        {
            iSpeed = pPawn->m_vecAbsVelocity( ).Length2D( );
        }

        vecComponents.push_back( { iSpeed, "speed" } );
    }

    constexpr float PADDING = 8.0f;
    constexpr float ROUNDING = 4.0f;
    constexpr float BLUR_INTENSITY = 5.0f;
    constexpr float ANIMATION_SPEED = 0.3f;
    constexpr ImColor BG_COLOR = { 25, 25, 25, 220 };

    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList( );
    ImVec2 vDisplaySize = ImGui::GetIO( ).DisplaySize;

    const char* szMainLabel = "chlenixx";
    const char* szBuildLabel = " alpha";
    float flMainFontSize = Fonts::VerdanaBold->FontSize;
    ImVec2 vMainLabelSize = Fonts::VerdanaBold->CalcTextSizeA( flMainFontSize, FLT_MAX, 0, szMainLabel );
    ImVec2 vBuildLabelSize = Fonts::VerdanaBold->CalcTextSizeA( flMainFontSize, FLT_MAX, 0, szBuildLabel );

    float flSmallFontSize = Fonts::Verdana->FontSize;
    float flVerticalOffset = ( flMainFontSize - flSmallFontSize ) / 2;
    float flSpacing = flMainFontSize * 0.5f;

    float flTotalContentWidth = vMainLabelSize.x + vBuildLabelSize.x + ( vecComponents.empty() ? 0.f : flSpacing );

    for ( int i = 0; i < vecComponents.size( ); ++i )
    {
        auto& component = vecComponents[ i ];
        _snprintf_s( component.m_szValueBuffer, sizeof( component.m_szValueBuffer ), "%d", component.m_iValue );
        component.m_vValueSize = Fonts::VerdanaBold->CalcTextSizeA( flMainFontSize, FLT_MAX, 0, component.m_szValueBuffer );
        component.m_vLabelSize = Fonts::Verdana->CalcTextSizeA( flSmallFontSize, FLT_MAX, 0, component.szLabel );

        if ( i < vecComponents.size( ) - 1 )
            flTotalContentWidth += flSpacing + component.m_vValueSize.x + ( flSpacing * 0.4f ) + component.m_vLabelSize.x;

        else
            flTotalContentWidth += component.m_vValueSize.x + ( flSpacing * 0.4f ) + component.m_vLabelSize.x;
    }

    static float s_flWatermarkWidth = flTotalContentWidth + PADDING * 2;
    s_flWatermarkWidth = ImLerp( s_flWatermarkWidth, flTotalContentWidth + PADDING * 2, ANIMATION_SPEED );

    ImVec2 vPosition =
    {
        vDisplaySize.x - s_flWatermarkWidth - PADDING,
        PADDING
    };

    ImVec2 vSize = { s_flWatermarkWidth, flMainFontSize + PADDING * 2 };
    ImVec2 vEndPos = { vPosition.x + vSize.x, vPosition.y + vSize.y };

    blurEffect.ApplyBlur( pDrawList, vPosition, vSize, BLUR_INTENSITY, ROUNDING );

    pDrawList->AddRectFilled( vPosition, vEndPos, ImGui::ColorConvertFloat4ToU32( BG_COLOR ), ROUNDING );

    pDrawList->PushClipRect( vPosition, vEndPos );

    ImVec2 vCurrentPos =
    {
        vPosition.x + PADDING,
        vPosition.y + ( vSize.y - flMainFontSize ) / 2 - 1.5f
    };

    ImColor accentColor = GetAccentImColor();
    pDrawList->AddText( Fonts::VerdanaBold, flMainFontSize, vCurrentPos, accentColor, szMainLabel );
    vCurrentPos.x += vMainLabelSize.x;

    pDrawList->AddText( Fonts::VerdanaBold, flMainFontSize, vCurrentPos, ImColor( 255, 255, 255, 255 ), szBuildLabel );
    vCurrentPos.x += vBuildLabelSize.x + flSpacing;

    for ( int i = 0; i < vecComponents.size( ); ++i )
    {
        StatComponent& Component = vecComponents[ i ];

        pDrawList->AddText( Fonts::VerdanaBold, flMainFontSize, vCurrentPos, ImColor( 255, 255, 255, 255 ), Component.m_szValueBuffer );
        vCurrentPos.x += Component.m_vValueSize.x + ( flSpacing * 0.4f );

        ImVec2 vLabelPos = { vCurrentPos.x, vCurrentPos.y + flVerticalOffset };
        pDrawList->AddText( Fonts::Verdana, flSmallFontSize, vLabelPos, ImColor( 200, 200, 200, 220 ), Component.szLabel );

        if ( i < vecComponents.size( ) - 1 )
            vCurrentPos.x += Component.m_vLabelSize.x + flSpacing;
        else
            vCurrentPos.x += Component.m_vLabelSize.x;
    }

    pDrawList->PopClipRect( );

}

const char* arrFlagsNames[] =
{
    "Money flag",
    "Armor flag",
    "Kit flag",
    "Defusing flag",
    "Zoom flag"
};

void Gui::RenderSpectatorList()
{
    if (!Config::b(g_Variables.m_Misc.m_bSpectatorList))
        return;

    if (!Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected() || !Globals::m_pLocalPlayerPawn)
        return;

    ImGui::SetNextWindowSize(ImVec2(160, 0));

    std::vector<const char*> vecNames;
    vecNames.clear();

    for (EntityObject_t& object : g_Entities->m_vecEntities)
    {
        if (object.m_eType != EEntityType::ENTITY_PLAYER)
            continue;

        CCSPlayerController* pPlayerController = reinterpret_cast<CCSPlayerController*>(object.m_pEntity);
        if (!pPlayerController || pPlayerController->m_bPawnIsAlive())
            continue;

        C_CSObserverPawn* pPawn = pPlayerController->m_hObserverPawn().Get();
        if (!pPawn)
            continue;

        CPlayer_ObserverServices* pObserverServices = pPawn->m_pObserverServices();
        if (!pObserverServices)
            continue;

        C_BaseEntity* pObserverTarget = pObserverServices->m_hObserverTarget().Get();
        if (!pObserverTarget)
            continue;

        if (pObserverTarget->GetRefEHandle().GetEntryIndex() != Globals::m_pLocalPlayerPawn->GetRefEHandle().GetEntryIndex())
            continue;

        if (pPlayerController->m_sSanitizedPlayerName())
        {
            vecNames.push_back(pPlayerController->m_sSanitizedPlayerName());
        }
    }

    bool bShouldRender = Gui::m_bOpen || !vecNames.empty( );;

    if (!bShouldRender)
        return;

    if ( ImGui::Begin( X( "Spectators" ), nullptr, ImGuiWindowFlags_NoDecoration ) )
    {
        ImGui::PushFont( Fonts::Verdana );

        for ( auto szName : vecNames )
        {
            ChlenGui::Label( szName );
        }

        ImGui::PopFont( );
    }
    ImGui::End( );
}

const char* InputTypeToString(KeyBind_t kb) {
    static const std::unordered_map<int, const char*> modeToString = {
        {HOLD, "    hold"},
        {TOGGLE, "    toggle"},
        {ALWAYS_ON, "    always on"}
    };
    auto it = modeToString.find(kb.m_iMode);
    return (it != modeToString.end()) ? it->second : "(unknown)";
}

bool IsKeybindActive(bool featureEnabled, KeyBind_t keybind) {
    return featureEnabled && Input::HandleInput(keybind);
}

void RenderKeybinds() {
    if (!Config::b(g_Variables.m_Misc.m_bKeybindList))
        return;

    struct KeybindFeature {
        const char* label;
        bool featureEnabled;
        KeyBind_t keybind;
    };

    const KeybindFeature keybindFeatures[] = {
        {"Aimbot", Config::b(g_Variables.m_Legitbot.m_bEnabled), Config::kb(g_Variables.m_Legitbot.m_bAimbot)},
        {"Triggerbot", Config::b(g_Variables.m_Legitbot.m_bTriggerbotEnabled), Config::kb(g_Variables.m_Legitbot.m_kbTriggerbot)},
        {"Trigger Autowall", Config::b(g_Variables.m_Legitbot.m_bTriggerbotAutowall), Config::kb(g_Variables.m_Legitbot.m_kbTriggerbotAutowall)},
        {"Edge Jump", Config::b(g_Variables.m_Movement.kEdgeJump), Config::kb(g_Variables.m_Movement.m_kEdgeJump)},
        {"Third Person", Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson), Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind)},
        {"Min Damage Override", Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey)},
        {"Duck Peek Assist", Config::b(g_Variables.m_Ragebot.m_bDuckPeekAssist), Config::kb(g_Variables.m_Ragebot.m_kbDuckPeekAssist)},
        {"Right Antiaim", Config::b(g_Variables.m_Ragebot.m_bAntiaim), Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind)},
        {"Left Antiaim", Config::b(g_Variables.m_Ragebot.m_bAntiaim), Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind)},
        {"No Spread", Config::b(g_Variables.m_Ragebot.m_bNoSpread), Config::kb(g_Variables.m_Ragebot.m_iNoSpreadKey)},
        {"Free Cam", true, Config::kb(g_Variables.m_Misc.m_kbFreeLook)}
    };

    int maxTextWidth = 0;

    struct Feature_t
    {
        int iTextWidth;
        std::string strKeybind;
    };

    std::vector<Feature_t> activeKeybinds;

    for (const auto& feature : keybindFeatures) {
        if (IsKeybindActive(feature.featureEnabled, feature.keybind)) {
            std::string displayString = std::string(feature.label) + InputTypeToString(feature.keybind);

            int textWidth = ImGui::CalcTextSize(displayString.c_str()).x + 30;

            activeKeybinds.push_back(Feature_t(textWidth, displayString));

            if (textWidth > maxTextWidth) {
                maxTextWidth = textWidth;
            }
        }
    }

    if (activeKeybinds.empty())
        return;

    ImGui::SetNextWindowSize(ImVec2(maxTextWidth, 0));

    ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoDecoration);
    {
        const char* titleText = "KEYBINDS";
        float titleWidth = ImGui::CalcTextSize(titleText).x;
        ImGui::SetCursorPos(ImVec2((maxTextWidth - titleWidth) / 2.0f, 7));
        ImGui::Text("%s", titleText);
        ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
        for (const auto& feature : activeKeybinds) {
            blurEffect.ApplyBlur(ImGui::GetWindowDrawList(), ImGui::GetCursorPos(), ImVec2(feature.iTextWidth, ImGui::CalcTextSize(feature.strKeybind.c_str()).y - 5), 5, 5);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorPos(), ImVec2(feature.iTextWidth, ImGui::CalcTextSize(feature.strKeybind.c_str()).y - 5), ImColor(55, 55, 55, 120));
            ChlenGui::Label(feature.strKeybind.c_str());
        }
    }
    ImGui::End();
}

static int iButtonCount = 7;
static int iButtonWidth = 20;

static int iSelectedTab = 0;
static int iSelectedSubTabRagebot = 0;
static int iSelectedSubTabVisuals = 0;

void RenderTabButtons()
{
    ImGui::PushFont(Fonts::SegoeUIBold18);

    constexpr float flAnimationSpeed = 10.0f;
    const ImVec4 defaultColor = ImVec4(1.f, 1.f, 1.f, 1.f);

    struct TabConfig
    {
        const char* label;
        ETabs tab;
    };

    constexpr std::array<TabConfig, ETabs::TABS_COUNT> tabs =
    { {
        { "RAGE",   ETabs::TABS_RAGEBOT },
        { "LEGIT",  ETabs::TABS_LEGITBOT },
        { "VISUALS",ETabs::TABS_VISUALS },
        { "SKINS",  ETabs::TABS_INVENTORY },
        { "MISC",   ETabs::TABS_MISC },
        { "CONFIG", ETabs::TABS_CONFIG }
    } };

    static std::array<float, ETabs::TABS_COUNT> tabAlphas = {};

    auto AnimateTab = [](float& currentAlpha, bool selected, float speed)
        {
            const float target = selected ? 1.0f : 0.5f;
            currentAlpha = ImLerp(currentAlpha, target, ImGui::GetIO().DeltaTime * speed);
        };

    float menuWidth = ImGui::GetWindowWidth();

    float maxWidth = 0.0f;
    for (const auto& tab : tabs)
        maxWidth = std::max(maxWidth, ImGui::CalcTextSize(tab.label).x + 7.5f);

    float buttonWidth = maxWidth;
    constexpr float spacing = 2.f;

    float totalWidth =
        tabs.size() * buttonWidth +
        (tabs.size() - 1) * spacing;

    float startX = (menuWidth - totalWidth) * 0.5f + 50.f;
    ImGui::SetCursorPosX(startX);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.f);

    for (int i = 0; i < tabs.size(); ++i)
    {
        const auto& tab = tabs[i];
        float& alpha = tabAlphas[tab.tab];

        AnimateTab(alpha, iSelectedTab == tab.tab, flAnimationSpeed);

        ImVec4 color = defaultColor;
        color.w = alpha;
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        if (ChlenGui::TabButton(
            tab.label,
            tab.tab,
            iSelectedTab,
            0,
            ImVec2(buttonWidth, 30)))
        {
            iSelectedTab = tab.tab;
        }

        ImGui::PopStyleColor();

        if (i < tabs.size() - 1)
            ImGui::SameLine(0.0f, spacing);
    }

    ImGui::PopFont();
}

void RenderRagebotTabButtons()
{
    static float aimbotAlpha = 0.0f;
    static float antiaimAlpha = 0.0f;

    auto AnimateAlpha = [](float& alpha, bool active, float speed) {
        alpha = ImLerp(alpha, active ? 1.0f : 0.2f, ImGui::GetIO().DeltaTime * speed);
        };

    const float animationSpeed = 10.0f;
    const ImVec4 baseColor(1.0f, 1.0f, 1.0f, 1.0f);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    struct Tab {
        const char* label;
        ESubTabs_Ragebot id;
        float& alpha;
    } tabs[] = {
        {"Aimbot", ESubTabs_Ragebot::SUBTAB_AIMBOT, aimbotAlpha},
        {"Antiaim", ESubTabs_Ragebot::SUBTAB_ANTIAIM, antiaimAlpha}
    };

    const ImVec2 buttonSize(85, ImGui::CalcTextSize("Hi").y + 3);

    for (const auto& tab : tabs) {
        AnimateAlpha(tab.alpha, iSelectedSubTabRagebot == tab.id, animationSpeed);
        ImVec4 textColor = baseColor;
        textColor.w = tab.alpha;

        ImColor bgColor(50, 50, 50, 200);
        bgColor.Value.w = ImLerp(0.6f, 1.0f, 1.0f - tab.alpha);

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImVec2 buttonEnd = buttonPos + buttonSize;

        blurEffect.ApplyBlur(drawList, buttonPos, buttonSize, 5.0f, 4.0f);

        drawList->AddRectFilled(buttonPos, buttonEnd, bgColor, 3.2f);

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        if (ChlenGui::TabButton(tab.label, tab.id, iSelectedSubTabRagebot, -1, buttonSize)) {
            iSelectedSubTabRagebot = tab.id;
        }
        ImGui::PopStyleColor();
    }
}

void RenderVisualsTabButtons()
{
    static float flLabelAlphaLocal = 0.0f;
    static float flLabelAlphaEnemy = 0.0f;
    static float flLabelAlphaWorld = 0.0f;

    auto AnimateTab = [](float& currentAlpha, bool isSelected, float animationSpeed) {
        float targetAlpha = isSelected ? 1.0f : 0.2f;
        currentAlpha = ImLerp(currentAlpha, targetAlpha, ImGui::GetIO().DeltaTime * animationSpeed);
        };

    const float flAnimationSpeed = 10.0f;
    const ImVec4 baseColor(1.0f, 1.0f, 1.0f, 1.0f);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    struct Tab {
        const char* label;
        ESubTabs_Visuals id;
        float& alpha;
    } tabs[] = {
        {"Local", ESubTabs_Visuals::SUBTAB_LOCAL, flLabelAlphaLocal},
        {"Enemy", ESubTabs_Visuals::SUBTAB_ENEMY, flLabelAlphaEnemy},
        {"World", ESubTabs_Visuals::SUBTAB_WORLD, flLabelAlphaWorld}
    };

    const ImVec2 buttonSize(85, ImGui::CalcTextSize("Hi").y + 3);

    for (const auto& tab : tabs) {
        AnimateTab(tab.alpha, iSelectedSubTabVisuals == tab.id, flAnimationSpeed);
        ImVec4 textColor = baseColor;
        textColor.w = tab.alpha;

        ImColor bgColor(50, 50, 50, 200);
        bgColor.Value.w = ImLerp(0.6f, 1.0f, 1.0f - tab.alpha);

        ImVec2 buttonPos = ImGui::GetCursorScreenPos();
        ImVec2 buttonEnd = buttonPos + buttonSize;

        blurEffect.ApplyBlur(drawList, buttonPos, buttonSize, 5.0f, 4.0f);

        drawList->AddRectFilled(buttonPos, buttonEnd, bgColor, 3.2f);

        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        if (ChlenGui::TabButton(tab.label, tab.id, iSelectedSubTabVisuals, -1, buttonSize)) {
            iSelectedSubTabVisuals = tab.id;
        }
        ImGui::PopStyleColor();
    }
}

namespace fs = std::filesystem;

bool ContainsArms(const std::string& filename) {
    return filename.find("arms") != std::string::npos;
}

std::pair<std::vector<std::string>, std::vector<std::string>> FindVmdlFilesWithMaterials(const fs::path& folderPath) {
    std::vector<std::string> vmdlFilePaths;
    std::vector<std::string> vmdlNames;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vmdl_c") {
                fs::path parentDir = entry.path().parent_path();
                fs::path materialsDir = parentDir / "materials";
                fs::path stp = parentDir / "mat";

                if ((fs::exists(stp) && fs::is_directory(stp)) || (fs::exists(materialsDir) && fs::is_directory(materialsDir))) {
                    std::string fileName = entry.path().stem().string();

                    if (!ContainsArms(fileName)) {
                        vmdlFilePaths.push_back(fs::canonical(entry.path()).string());
                        vmdlNames.push_back(fileName);
                    }
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    }

    return { vmdlFilePaths, vmdlNames };
}

static void DrawHitLogs()
{
    if (!Interfaces::m_pGlobalVariables || !Interfaces::m_pEngine->IsInGame() || !Interfaces::m_pEngine->IsConnected() || !Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
    {
        if (!hitLogs.empty())
            hitLogs.clear();
        return;
    }

    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    float currentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
    float messageHeight = Fonts::VerdanaEsp->FontSize + 2.0f;
    float startY = 5.0f;
    float startX = 5.0f;

    for (size_t i = 0; i < hitLogs.size(); ++i)
    {
        const HitLogEntry& entry = hitLogs[i];

        if ( currentTime - entry.startTime > ( entry.fadeStartTime - entry.startTime + entry.fadeDuration ) )
        {
            hitLogs.erase( hitLogs.begin( ) + i );
            continue;
        }

        float alpha = 1.0f;

        if (currentTime >= entry.fadeStartTime) {
            alpha = 1.0f - ((currentTime - entry.fadeStartTime) / entry.fadeDuration);
            alpha = std::max(0.0f, std::min(1.0f, alpha));
        }

        ImColor color(255, 255, 255, static_cast<int>(255 * alpha));
        ImColor color2(0, 0, 0, static_cast<int>(100 * alpha));

        pDrawList->AddText(Fonts::VerdanaEsp, Fonts::VerdanaEsp->FontSize, ImVec2(startX + 1, startY + i * messageHeight + 1), color2, entry.message.c_str());
        pDrawList->AddText(Fonts::VerdanaEsp, Fonts::VerdanaEsp->FontSize, ImVec2(startX, startY + i * messageHeight), color, entry.message.c_str());
    }
}

std::string ConvertToRelativeModelPath(const std::string& fullPath, const std::string& basePath) {
    namespace fs = std::filesystem;
    fs::path fullFsPath = fs::path(fullPath);
    fs::path baseFsPath = fs::path(basePath);

    fs::path relativePath = fs::relative(fullFsPath, baseFsPath);

    relativePath.replace_extension(".vmdl");

    std::string modelPath = relativePath.string();
    std::replace(modelPath.begin(), modelPath.end(), '\\', '/');

    return modelPath;
}

static int currentWeaponType = 0;
static int currentLegitWeaponType = 0;

std::vector<MenuRageHitbox_t>& GetHitboxesForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LIGHT_PISTOL_vecHitBoxes);
    case DEAGLE:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_DEAGLE_vecHitBoxes);
    case REVOLVER:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_REVOLVER_vecHitBoxes);
    case SMG:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SMG_vecHitBoxes);
    case LMG:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LMG_vecHitBoxes);
    case AR:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AR_vecHitBoxes);
    case SHOTGUN:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SHOTGUN_vecHitBoxes);
    case SCOUT:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SCOUT_vecHitBoxes);
    case AUTOSNIPER:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AUTOSNIPER_vecHitBoxes);
    case AWP:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AWP_vecHitBoxes);
    default:
        return Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LIGHT_PISTOL_vecHitBoxes);
    }
}

int& GetPointscaleHeadForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscaleHead);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMaxPointscaleHead);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMaxPointscaleHead);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMaxPointscaleHead);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMaxPointscaleHead);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMaxPointscaleHead);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMaxPointscaleHead);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMaxPointscaleHead);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMaxPointscaleHead);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMaxPointscaleHead);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscaleHead);
    }
}

int& GetPointscaleBodyForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscaleBody);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMaxPointscaleBody);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMaxPointscaleBody);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMaxPointscaleBody);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMaxPointscaleBody);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMaxPointscaleBody);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMaxPointscaleBody);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMaxPointscaleBody);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMaxPointscaleBody);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMaxPointscaleBody);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscaleBody);
    }
}

int& GetTargetTypeForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iTargetPriority);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iTargetPriority);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iTargetPriority);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iTargetPriority);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iTargetPriority);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iTargetPriority);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iTargetPriority);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iTargetPriority);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iTargetPriority);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iTargetPriority);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iTargetPriority);
    }
}

int& GetHitchanceForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchance);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iHitchance);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iHitchance);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iHitchance);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iHitchance);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iHitchance);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iHitchance);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iHitchance);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iHitchance);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iHitchance);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchance);
    }
}

int& GetMinDamageForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamage);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamage);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamage);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamage);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamage);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMinDamage);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamage);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamage);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamage);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamage);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamage);
    }
}

int& GetMinDamageOverrideForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamageOverride);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamageOverride);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamageOverride);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamageOverride);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamageOverride);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMinDamageOverride);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamageOverride);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamageOverride);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamageOverride);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamageOverride);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamageOverride);
    }
}

int& GetPointscaleForCurrentWeapon()
{
    switch (currentWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscale);
    case DEAGLE:
        return Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMaxPointscale);
    case REVOLVER:
        return Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMaxPointscale);
    case SMG:
        return Config::i(g_Variables.m_Ragebot.m_SMG_iMaxPointscale);
    case LMG:
        return Config::i(g_Variables.m_Ragebot.m_LMG_iMaxPointscale);
    case AR:
        return Config::i(g_Variables.m_Ragebot.m_AR_iMaxPointscale);
    case SHOTGUN:
        return Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMaxPointscale);
    case SCOUT:
        return Config::i(g_Variables.m_Ragebot.m_SCOUT_iMaxPointscale);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMaxPointscale);
    case AWP:
        return Config::i(g_Variables.m_Ragebot.m_AWP_iMaxPointscale);
    default:
        return Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscale);
    }
}

int& GetFovForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iFov);
    case DEAGLE:
        return Config::i(g_Variables.m_Legitbot.m_DEAGLE_iFov);
    case REVOLVER:
        return Config::i(g_Variables.m_Legitbot.m_REVOLVER_iFov);
    case SMG:
        return Config::i(g_Variables.m_Legitbot.m_SMG_iFov);
    case LMG:
        return Config::i(g_Variables.m_Legitbot.m_LMG_iFov);
    case AR:
        return Config::i(g_Variables.m_Legitbot.m_AR_iFov);
    case SHOTGUN:
        return Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iFov);
    case SCOUT:
        return Config::i(g_Variables.m_Legitbot.m_SCOUT_iFov);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iFov);
    case AWP:
        return Config::i(g_Variables.m_Legitbot.m_AWP_iFov);
    default:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iFov);
    }
}

int& GetSmoothnessForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iSmoothness);
    case DEAGLE:
        return Config::i(g_Variables.m_Legitbot.m_DEAGLE_iSmoothness);
    case REVOLVER:
        return Config::i(g_Variables.m_Legitbot.m_REVOLVER_iSmoothness);
    case SMG:
        return Config::i(g_Variables.m_Legitbot.m_SMG_iSmoothness);
    case LMG:
        return Config::i(g_Variables.m_Legitbot.m_LMG_iSmoothness);
    case AR:
        return Config::i(g_Variables.m_Legitbot.m_AR_iSmoothness);
    case SHOTGUN:
        return Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iSmoothness);
    case SCOUT:
        return Config::i(g_Variables.m_Legitbot.m_SCOUT_iSmoothness);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iSmoothness);
    case AWP:
        return Config::i(g_Variables.m_Legitbot.m_AWP_iSmoothness);
    default:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iSmoothness);
    }
}

std::vector<bool>& GetHitboxesForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbHitboxes);
    case DEAGLE:
        return Config::vb(g_Variables.m_Legitbot.m_DEAGLE_vbHitboxes);
    case REVOLVER:
        return Config::vb(g_Variables.m_Legitbot.m_REVOLVER_vbHitboxes);
    case SMG:
        return Config::vb(g_Variables.m_Legitbot.m_SMG_vbHitboxes);
    case LMG:
        return Config::vb(g_Variables.m_Legitbot.m_LMG_vbHitboxes);
    case AR:
        return Config::vb(g_Variables.m_Legitbot.m_AR_vbHitboxes);
    case SHOTGUN:
        return Config::vb(g_Variables.m_Legitbot.m_SHOTGUN_vbHitboxes);
    case SCOUT:
        return Config::vb(g_Variables.m_Legitbot.m_SCOUT_vbHitboxes);
    case AUTOSNIPER:
        return Config::vb(g_Variables.m_Legitbot.m_AUTOSNIPER_vbHitboxes);
    case AWP:
        return Config::vb(g_Variables.m_Legitbot.m_AWP_vbHitboxes);
    default:
        return Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbHitboxes);
    }
}

bool& GetAutostopForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::b(g_Variables.m_Legitbot.m_LIGHT_PISTOL_bAutostop);
    case DEAGLE:
        return Config::b(g_Variables.m_Legitbot.m_DEAGLE_bAutostop);
    case REVOLVER:
        return Config::b(g_Variables.m_Legitbot.m_REVOLVER_bAutostop);
    case SMG:
        return Config::b(g_Variables.m_Legitbot.m_SMG_bAutostop);
    case LMG:
        return Config::b(g_Variables.m_Legitbot.m_LMG_bAutostop);
    case AR:
        return Config::b(g_Variables.m_Legitbot.m_AR_bAutostop);
    case SHOTGUN:
        return Config::b(g_Variables.m_Legitbot.m_SHOTGUN_bAutostop);
    case SCOUT:
        return Config::b(g_Variables.m_Legitbot.m_SCOUT_bAutostop);
    case AUTOSNIPER:
        return Config::b(g_Variables.m_Legitbot.m_AUTOSNIPER_bAutostop);
    case AWP:
        return Config::b(g_Variables.m_Legitbot.m_AWP_bAutostop);
    default:
        return Config::b(g_Variables.m_Legitbot.m_LIGHT_PISTOL_bAutostop);
    }
}

std::vector<bool>& GetAutostopModesForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbAutostop);
    case DEAGLE:
        return Config::vb(g_Variables.m_Legitbot.m_DEAGLE_vbAutostop);
    case REVOLVER:
        return Config::vb(g_Variables.m_Legitbot.m_REVOLVER_vbAutostop);
    case SMG:
        return Config::vb(g_Variables.m_Legitbot.m_SMG_vbAutostop);
    case LMG:
        return Config::vb(g_Variables.m_Legitbot.m_LMG_vbAutostop);
    case AR:
        return Config::vb(g_Variables.m_Legitbot.m_AR_vbAutostop);
    case SHOTGUN:
        return Config::vb(g_Variables.m_Legitbot.m_SHOTGUN_vbAutostop);
    case SCOUT:
        return Config::vb(g_Variables.m_Legitbot.m_SCOUT_vbAutostop);
    case AUTOSNIPER:
        return Config::vb(g_Variables.m_Legitbot.m_AUTOSNIPER_vbAutostop);
    case AWP:
        return Config::vb(g_Variables.m_Legitbot.m_AWP_vbAutostop);
    default:
        return Config::vb(g_Variables.m_Legitbot.m_LIGHT_PISTOL_vbAutostop);
    }
}

int& GetTriggerbotDelayForCurrentLegitWeapon()
{
    switch (currentLegitWeaponType)
    {
    case LIGHT_PISTOL:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iTriggerbotDelay);
    case DEAGLE:
        return Config::i(g_Variables.m_Legitbot.m_DEAGLE_iTriggerbotDelay);
    case REVOLVER:
        return Config::i(g_Variables.m_Legitbot.m_REVOLVER_iTriggerbotDelay);
    case SMG:
        return Config::i(g_Variables.m_Legitbot.m_SMG_iTriggerbotDelay);
    case LMG:
        return Config::i(g_Variables.m_Legitbot.m_LMG_iTriggerbotDelay);
    case AR:
        return Config::i(g_Variables.m_Legitbot.m_AR_iTriggerbotDelay);
    case SHOTGUN:
        return Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iTriggerbotDelay);
    case SCOUT:
        return Config::i(g_Variables.m_Legitbot.m_SCOUT_iTriggerbotDelay);
    case AUTOSNIPER:
        return Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iTriggerbotDelay);
    case AWP:
        return Config::i(g_Variables.m_Legitbot.m_AWP_iTriggerbotDelay);
    default:
        return Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iTriggerbotDelay);
    }
}

bool CustomButton_NoOutlineAndBg(const char* szLabel, int iTextAlign, ImVec2 imSize, int iCustomID)
{
    ImGuiWindow* imWindow = ImGui::GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = ImGui::CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = ImGui::CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ImGui::ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    float* flAnimatedAlpha = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);
    float flTargetAlpha = bHovered ? 0.8f : 0.5f;
    *flAnimatedAlpha = ImLerp(*flAnimatedAlpha, flTargetAlpha, ImGui::GetIO().DeltaTime * 5);

    ImColor imButtonColor = ImColor(255, 255, 255, 100);
    if (bHovered && !bPressed)
    {
        imButtonColor = ImColor(255, 255, 255, 180);
    }
    if (bHovered && bPressed)
    {
        imButtonColor = ImColor(255, 255, 255, 255);
    }

    ImVec4 imLabelColor = imButtonColor.Value;
    imLabelColor.w = ImClamp(*flAnimatedAlpha, 0.0f, 1.0f);

    float alignFactor = (iTextAlign == -1) ? 0.0f : (iTextAlign == 1) ? 1.0f : 0.5f;
    ImVec2 imLabelTextPos = ImVec2(
        ImLerp(imTotalBB.Min.x + style.FramePadding.x, imTotalBB.Max.x - style.FramePadding.x - imLabelSize.x, alignFactor),
        ImLerp(imTotalBB.Min.y + style.FramePadding.y, imTotalBB.Max.y - style.FramePadding.y - imLabelSize.y, 0.2f)
    );

    imWindow->DrawList->AddText(imLabelTextPos, ImColor(imLabelColor), szLabel);

    return bPressed;
}

void Gui::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* pForeGroundDrawList = ImGui::GetForegroundDrawList();

    Gui::Update(io);

    if (!m_bInitialized)
        return;

    blurEffect.BeginBlur();

    RenderKeybinds();
    DrawHitLogs();

    Gui::RenderSpectatorList();
    Gui::RenderWatermark();

    if (!Gui::m_bOpen)
        return;

    ImGui::SetNextWindowSize(MENU_SIZE);
    ChlenGui::Begin("Menu");
    {

        blurEffect.ApplyBlur(ImGui::GetCurrentWindow()->DrawList, ImGui::GetWindowPos(), MENU_SIZE, 5, 5);

        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + MENU_SIZE, ImColor(18, 18, 18, 235), ImGui::GetStyle().WindowRounding);

        ImRect imBarBackground = ImRect(
            ImGui::GetWindowPos(),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + 32.0f)
        );

        ImVec2 blurPos = imBarBackground.Min;
        ImVec2 blurSize = ImVec2(imBarBackground.Max.x - imBarBackground.Min.x, imBarBackground.Max.y - imBarBackground.Min.y);

        ImGui::GetCurrentWindow()->DrawList->AddRectFilled(imBarBackground.Min, imBarBackground.Max, ImColor(25, 25, 25, 150), ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop);

        float flOriginalCursorPosY = ImGui::GetCursorPosY( );

        const char* tabs[] = { "RAGE", "LEGIT", "VISUALS", "INVENTORY", "MISC", "CONFIGS"};

        ChlenGui::RenderMenuTabs(tabs, IM_ARRAYSIZE(tabs), &iSelectedTab);

        ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);

        ImVec2 imAllChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3, ImGui::GetContentRegionAvail().y);

        switch (iSelectedTab)
        {
        case ETabs::TABS_RAGEBOT:
        {
            const char* subtabs[] = { "AIMBOT", "ANTI-AIM" };
            ChlenGui::RenderSubTabs(subtabs, IM_ARRAYSIZE(subtabs), &iSelectedSubTabRagebot);

            switch (iSelectedSubTabRagebot)
            {
                case ESubTabs_Ragebot::SUBTAB_AIMBOT:
                {
                    ImVec2 imRagebotChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

                    ImGui::BeginChild("General", child_size, true);
                    ImGui::Text("General");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Enable", &Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled), 0);

                        ChlenGui::CheckBox("Auto Wall", &Config::b(g_Variables.m_Ragebot.m_bAutoWall), 0);

                        ChlenGui::CheckBox("Auto Fire", &Config::b(g_Variables.m_Ragebot.m_bAutoShoot), 0);

                        ChlenGui::CheckBox("Auto Scope", &Config::b(g_Variables.m_Ragebot.m_bAutoscope), 0);

                        ChlenGui::CheckBox("Silent Aim", &Config::b(g_Variables.m_Ragebot.m_bSilentAim), 0);

                        ChlenGui::CheckBox("Draw FOV", &Config::b(g_Variables.m_Ragebot.m_bDrawFov), 0);

                        ChlenGui::SliderFloat("Aimbot FOV", &Config::f(g_Variables.m_Ragebot.m_flFov), 0.f, 180.f, 0, false, "%.0f");

                        ChlenGui::CheckBox("Visualize Aimbot", &Config::b(g_Variables.m_Ragebot.m_bDrawTarget), 0);

                        ChlenGui::Combo("Weapon Type", &currentWeaponType, Tabs::WeaponNames, IM_ARRAYSIZE(Tabs::WeaponNames));

                        ChlenGui::Combo("Target Priority", &GetTargetTypeForCurrentWeapon(), Tabs::TargetTypes, IM_ARRAYSIZE(Tabs::TargetTypes));

                        ChlenGui::MinDamageSlider("Min Damage", &GetMinDamageForCurrentWeapon(), 1, 150);

                        ChlenGui::SliderInt("Min Dmg. Ovr.", &GetMinDamageOverrideForCurrentWeapon(), 1, 100);

                        std::vector<bool> hitboxSelections(HB_MAX, false);
                        std::vector<bool> multipointSelections(HB_MAX, false);

                        std::vector<MenuRageHitbox_t>& currentHitboxes = GetHitboxesForCurrentWeapon();

                        for (const auto& hitbox : currentHitboxes) {
                            hitboxSelections[hitbox.m_eDefinition] = true;
                            multipointSelections[hitbox.m_eDefinition] = hitbox.m_bMultipoint;
                        }

                        if (ChlenGui::MultiComboBox("Target Hitboxes", Tabs::HitboxNames, hitboxSelections, IM_ARRAYSIZE(Tabs::HitboxNames))) {
                            currentHitboxes.erase(
                                std::remove_if(currentHitboxes.begin(), currentHitboxes.end(),
                                    [&](const MenuRageHitbox_t& hitbox) {
                                        return !hitboxSelections[hitbox.m_eDefinition];
                                    }),
                                currentHitboxes.end()
                            );

                            for (int i = 0; i < HB_MAX; i++) {
                                if (hitboxSelections[i] &&
                                    std::none_of(currentHitboxes.begin(), currentHitboxes.end(),
                                        [&](const MenuRageHitbox_t& h) { return h.m_eDefinition == i; })) {
                                    currentHitboxes.push_back(MenuRageHitbox_t{ false, static_cast<EConfigHitBoxes>(i) });
                                }
                            }
                        }

                        bool anyHitboxSelected = std::any_of(hitboxSelections.begin(), hitboxSelections.end(), [](bool selected) { return selected; });

                        if (anyHitboxSelected) {
                            std::vector<const char*> selectedHitboxNames;
                            std::vector<bool> selectedMultipointValues;
                            std::vector<int> hitboxToIndexMap;

                            for (int i = 0; i < HB_MAX; i++) {
                                if (hitboxSelections[i]) {
                                    selectedHitboxNames.push_back(Tabs::HitboxNames[i]);
                                    selectedMultipointValues.push_back(multipointSelections[i]);
                                    hitboxToIndexMap.push_back(i);
                                }
                            }

                            if (ChlenGui::MultiComboBox("Multipoint Hitboxes", selectedHitboxNames.data(), selectedMultipointValues, selectedHitboxNames.size())) {
                                for (size_t i = 0; i < selectedMultipointValues.size(); i++) {
                                    int originalIndex = hitboxToIndexMap[i];

                                    for (auto& hitbox : currentHitboxes) {
                                        if (hitbox.m_eDefinition == originalIndex) {
                                            hitbox.m_bMultipoint = selectedMultipointValues[i];
                                            break;
                                        }
                                    }
                                }
                            }

                            if (std::any_of(selectedMultipointValues.begin(), selectedMultipointValues.end(), [](bool selected) { return selected; })) {
                                ChlenGui::SliderInt("Point Scale", &GetPointscaleForCurrentWeapon(), 1, 100);
                            }
                        }
                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    ImGui::BeginChild("Accuracy", child_size1, true);
                    ImGui::Text("Accuracy");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Auto Stop", &Config::b(g_Variables.m_Ragebot.m_bAutostop), 0);
                        ChlenGui::SameLine();
                        ChlenGui::MultiComboBox("Autostop Mode", Tabs::szAutostopModes, Config::vb(g_Variables.m_Ragebot.m_vbAutostop), IM_ARRAYSIZE(Tabs::szAutostopModes), false);

                        ChlenGui::SliderInt("Hit Chance", &GetHitchanceForCurrentWeapon(), 0, 100, 1);

                        ChlenGui::CheckBox("Auto Revolver", &Config::b(g_Variables.m_Ragebot.m_bAutoRevolver));

                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    ImGui::SetCursorPos(child_pos);
                    ImGui::BeginChild("Extra", child_size1, true);
                    ImGui::Text("Extra");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::Label("Min Damage Override Key");
                        ChlenGui::Keybind("Min Damage Override Key", &Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey));

                        ChlenGui::MultiComboBox("History Aim", Tabs::szHistoryAimModes, Config::vb(g_Variables.m_Ragebot.m_vbRageHistoryAim), IM_ARRAYSIZE(Tabs::szHistoryAimModes), false);

                        ChlenGui::CheckBox("Performance mode", &Config::b(g_Variables.m_Misc.m_bPerformanceMode));

                        ChlenGui::CheckBox("Duck Peek Assist", &Config::b(g_Variables.m_Ragebot.m_bDuckPeekAssist));
                        ChlenGui::Keybind("Duck Peek Assist Key", &Config::kb(g_Variables.m_Ragebot.m_kbDuckPeekAssist));

                    }
                    ChlenGui::ChildEnd();
                    break;
                }

                case ESubTabs_Ragebot::SUBTAB_ANTIAIM:
                {
                    ImVec2 imRagebotChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

                    ImGui::BeginChild("AA_CHILD", child_size, true);
                    ImGui::Text("Antiaim");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Enabled", &Config::b(g_Variables.m_Ragebot.m_bAntiaim), 0);
                        ChlenGui::CheckBox("Hide shots", &Config::b(g_Variables.m_Ragebot.m_bHideShots), 0);
                        ChlenGui::CheckBox("At target", &Config::b(g_Variables.m_Ragebot.m_bAtTarget), 0);

                        ChlenGui::SliderInt("Yaw", &Config::i(g_Variables.m_Ragebot.m_iYaw), -179, 179);
                        ChlenGui::SliderInt("Pitch", &Config::i(g_Variables.m_Ragebot.m_iPitch), -89, 89);

                        ChlenGui::CheckBox("Freestanding", &Config::b(g_Variables.m_Ragebot.m_bFreestanding));

                        if (ChlenGui::CheckBox("Yaw jitter", &Config::b(g_Variables.m_Ragebot.m_bYawJitter), 1))
                        {
                            ChlenGui::SliderInt("Yaw jitter", &Config::i(g_Variables.m_Ragebot.m_iYawJitter), -179, 179);
                        }

                        ChlenGui::Label("Left");
                        ChlenGui::SameLine();
                        ChlenGui::Keybind("Left key", &Config::kb(g_Variables.m_Ragebot.m_iLeftAntiaimKeybind));

                        ChlenGui::Label("Right");
                        ChlenGui::SameLine();
                        ChlenGui::Keybind("Right key", &Config::kb(g_Variables.m_Ragebot.m_iRightAntiaimKeybind));

                        if (ChlenGui::CheckBox("Spin", &Config::b(g_Variables.m_Ragebot.m_bSpinAA), 0))
                        {
                            ChlenGui::SliderInt("Spin speed", &Config::i(g_Variables.m_Ragebot.m_iSpinAASpeed), 0, 180);
                        }
                    }
                    ChlenGui::ChildEnd();
                    break;
                }
            }
            break;
        }
        case ETabs::TABS_LEGITBOT:
        {
            static bool bUserManuallyChanged = false;
            static int nLastWeaponID = -1;

            if (Config::b(g_Variables.m_Legitbot.m_bAutoWeapon) && Globals::m_pLocalPlayerPawn && Globals::m_pLocalPlayerPawn->m_pWeaponServices())
            {
                auto pWeapon = Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
                if (pWeapon)
                {
                    int nID = pWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex();

                    if (nID != nLastWeaponID) {
                        bUserManuallyChanged = false;
                        nLastWeaponID = nID;
                    }

                    if (!bUserManuallyChanged) {
                        int nType = -1;
                        switch (nID) {
                            case 1: nType = 1; break;
                            case 64: nType = 2; break;
                            case 9: nType = 9; break;
                            case 40: nType = 7; break;
                            case 11: case 38: nType = 8; break;
                            case 7: case 8: case 10: case 13: case 16: case 39: case 60: nType = 5; break;
                            case 17: case 19: case 23: case 24: case 26: case 33: case 34: nType = 3; break;
                            case 14: case 28: nType = 4; break;
                            case 25: case 27: case 29: case 35: nType = 6; break;
                            case 2: case 3: case 4: case 30: case 32: case 36: case 61: case 63: nType = 0; break;
                        }
                        if (nType != -1) currentLegitWeaponType = nType;
                    }
                }
            }

            ImVec2 imLegitbotChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

            ImGui::BeginChild("LEGIT_GENERAL", ImVec2(child_size.x, child_size.y + 16), true);
            ImGui::Text("General");

            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
            {
                ChlenGui::CheckBox(X("Aim"), &Config::b(g_Variables.m_Legitbot.m_bEnabled));
                ChlenGui::Keybind("Legit key", &Config::kb(g_Variables.m_Legitbot.m_bAimbot));
                ChlenGui::CheckBox(X("Auto weapon"), &Config::b(g_Variables.m_Legitbot.m_bAutoWeapon));
                ChlenGui::CheckBox(X("Triggerbot"), &Config::b(g_Variables.m_Legitbot.m_bTriggerbotEnabled));
                ChlenGui::Keybind("Trigger key", &Config::kb(g_Variables.m_Legitbot.m_kbTriggerbot));

                int prevWeaponType = currentLegitWeaponType;
                ChlenGui::Combo("Weapon group", &currentLegitWeaponType, Tabs::WeaponNames, IM_ARRAYSIZE(Tabs::WeaponNames));

                if (prevWeaponType != currentLegitWeaponType) {
                    bUserManuallyChanged = true;
                }

                ChlenGui::MultiComboBox("Aim hitboxes", Tabs::HitboxNames, GetHitboxesForCurrentLegitWeapon(), IM_ARRAYSIZE(Tabs::HitboxNames));
            }
            ChlenGui::ChildEnd();

            ChlenGui::SameLine();

            ImGui::BeginChild("LEGIT_WEAPON_SETTINGS", child_size1, true);
            ImGui::Text("Settings");

            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
            {
                ChlenGui::MultiComboBox("Conditions", Tabs::LegitbotConditions, Config::vb(g_Variables.m_Legitbot.m_vbConditions), IM_ARRAYSIZE(Tabs::LegitbotConditions));

                int& iFovValue = GetFovForCurrentLegitWeapon();
                const char* arrFovModes[] = { "Auto", "Manual" };
                int iFovMode = (iFovValue == -1) ? 0 : 1;

                if (ChlenGui::Combo(X("FOV mode"), &iFovMode, arrFovModes, 2))
                {
                    if (iFovMode == 0)
                        iFovValue = -1;
                    else
                        if (iFovValue == -1) iFovValue = 10;
                }

                if (iFovMode == 1)
                    ChlenGui::SliderInt(X("FOV"), &iFovValue, 10, 100);

                ChlenGui::SliderInt(X("Smoothness"), &GetSmoothnessForCurrentLegitWeapon(), 1, 100);

                ChlenGui::SliderInt(X("Trigger delay"), &GetTriggerbotDelayForCurrentLegitWeapon(), 0, 500);
            }
            ChlenGui::ChildEnd();

            ImGui::SetCursorPos(child_pos);
            ImGui::BeginChild("semirage", child_size1, true);
            ImGui::Text("Semirage");
            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
            {
                ChlenGui::CheckBox("Autostop", &GetAutostopForCurrentLegitWeapon(), 0);
                ChlenGui::MultiComboBox("Autostop mode", Tabs::szAutostopModes, GetAutostopModesForCurrentLegitWeapon(), IM_ARRAYSIZE(Tabs::szAutostopModes), false);
                ChlenGui::CheckBox(X("Trigger autowall"), &Config::b(g_Variables.m_Legitbot.m_bTriggerbotAutowall));
                ChlenGui::Keybind("Trigger autowall key", &Config::kb(g_Variables.m_Legitbot.m_kbTriggerbotAutowall));
                ChlenGui::CheckBox(X("RCS"), &Config::b(g_Variables.m_Legitbot.m_bEnableRCS));
                ChlenGui::CheckBox(X("Auto revolver"), &Config::b(g_Variables.m_Legitbot.m_bAutoRevolver));

                ChlenGui::CheckBox(X("Slow walk"), &Config::b(g_Variables.m_Legitbot.m_bSlowWalk));
                ChlenGui::Keybind("Slow walk key", &Config::kb(g_Variables.m_Legitbot.m_kbSlowWalk));
                if (Config::b(g_Variables.m_Legitbot.m_bSlowWalk))
                {
                    ChlenGui::SliderInt(X("Slow walk speed"), &Config::i(g_Variables.m_Legitbot.m_iSlowWalkSpeed), 0, 100);
                }
            }
            ChlenGui::ChildEnd();

            break;
        }
        case ETabs::TABS_VISUALS:
        {

            const char* subtabs[] = { "LOCAL", "ENEMY", "WORLD" };
            ChlenGui::RenderSubTabs(subtabs, IM_ARRAYSIZE(subtabs), &iSelectedSubTabVisuals);

            ImVec2 imVisualsChildSize = ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().FramePadding.x - 2) / 3.5, ImGui::GetContentRegionAvail().y);

            switch (iSelectedSubTabVisuals)
            {
                case ESubTabs_Visuals::SUBTAB_LOCAL:
                {
                    ImGui::BeginChild("Options_visuals", child_size, true);
                    ImGui::Text("Options");

                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Remove punch", &Config::b(g_Variables.m_Visuals.m_bRemovePunch), 1);
                        ChlenGui::CheckBox("Remove hud", &Config::b(g_Variables.m_Misc.m_bShowHud), 1);
                        ChlenGui::CheckBox("Third person", &Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson), 1);
                        ChlenGui::Keybind("Third person key", &Config::kb(g_Variables.m_Visuals.m_iThirdPersonKeybind));
                        if (Config::b(g_Variables.m_Visuals.m_bEnableThirdPerson))
                            ChlenGui::SliderInt("Third person distance", &Config::i(g_Variables.m_Visuals.m_iThirdPersonDistance), 60, 150, 1);

                        ChlenGui::NoneSlider("World FOV", &Config::i(g_Variables.m_Visuals.m_iWorldFov), 79, 200);

                        if (ChlenGui::CheckBox("Override world FOV", &Config::b(g_Variables.m_Visuals.m_bOverrideWorldFov)))
                            ChlenGui::SliderInt("Override world FOV", &Config::i(g_Variables.m_Visuals.m_iOverrideWorldFovMax), 1, 20, 1);

                        if (ChlenGui::CheckBox("Remove scope FOV", &Config::b(g_Variables.m_Visuals.m_bRemoveScopeFov)))
                            ChlenGui::SliderInt("World FOV (scoped)", &Config::i(g_Variables.m_Visuals.m_iScopedFov), 0, 100, 0, false, "%d%%" );

                        if (ChlenGui::CheckBox("Viewmodel changer", &Config::b(g_Variables.m_Visuals.m_bViewmodelChanger)))
                        {
                            ChlenGui::SliderInt("Viewmodel FOV", &Config::i(g_Variables.m_Visuals.iViewmodelFov), 40, 120);

                            ChlenGui::SliderInt("Viewmodel X", &Config::i(g_Variables.m_Visuals.iXOffset), -90, 90);
                            ChlenGui::SliderInt("Viewmodel Y", &Config::i(g_Variables.m_Visuals.iYOffset), -90, 90);
                            ChlenGui::SliderInt("Viewmodel Z", &Config::i(g_Variables.m_Visuals.iZOffset), -90, 90);

                        }
                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    ImGui::BeginChild("Locacacal_chams", child_size1, true);
                    ImGui::Text("Local chams");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox( "Body", &Config::b( g_Variables.m_Visuals.m_bEnableSelfChams ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Body color", &Config::c( g_Variables.m_Visuals.m_colSelfChams ) );
                        ChlenGui::Combo( "Body material", &Config::i( g_Variables.m_Visuals.m_iChamMaterialLocal ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox( "Arms", &Config::b( g_Variables.m_Visuals.m_bEnableSelfArmsChams ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Arms color", &Config::c( g_Variables.m_Visuals.m_colSelfArmsChams ) );
                        ChlenGui::Combo( "Arms material", &Config::i( g_Variables.m_Visuals.m_iChamMaterialLocalArms ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox( "Weapon", &Config::b( g_Variables.m_Visuals.m_bEnableSelfWeaponChams ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Weapon color", &Config::c( g_Variables.m_Visuals.m_colSelfWeaponChams ) );
                        ChlenGui::Combo( "Weapon material", &Config::i( g_Variables.m_Visuals.m_iChamMaterialLocalWeapon ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                    }
                    ChlenGui::ChildEnd();

                    ImGui::SetCursorPos(child_pos);
                    ImGui::BeginChild("zzzz", child_size1, true);
                    ImGui::Text("Other");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Speedometer", &Config::b(g_Variables.m_Visuals.m_bSpeedometer), 1);
                        if (Config::b(g_Variables.m_Visuals.m_bSpeedometer))
                        {
                            ChlenGui::SliderInt("Size", &Config::i(g_Variables.m_Visuals.m_iSpeedometerSize), 40, 150);
                            ChlenGui::SliderInt("Max speed", &Config::i(g_Variables.m_Visuals.m_iSpeedometerMaxSpeed), 100, 500);
                        }

                        ChlenGui::Combo("Remove scope", &Config::i(g_Variables.m_Visuals.m_iRemoveScope), arrScopeNames, IM_ARRAYSIZE(arrScopeNames));
                        if (Config::i(g_Variables.m_Visuals.m_iRemoveScope) == SCOPE_GRADIENT) {
                            ChlenGui::Label("Color");
                            ChlenGui::SameLine();
                            ChlenGui::SimpleColorPicker("Color in", &Config::c(g_Variables.m_Visuals.m_colScope), 2);
                            ChlenGui::SameLine();
                            ChlenGui::SimpleColorPicker("Color out", &Config::c(g_Variables.m_Visuals.m_colScopeOutSide));

                            ChlenGui::SliderInt("Scope length", &Config::i(g_Variables.m_Visuals.m_iScopeLength), 1, 500);
                            ChlenGui::SliderInt("Scope thickness", &Config::i(g_Variables.m_Visuals.m_iScopeThickness), 1, 50);
                            ChlenGui::SliderInt("Scope gap", &Config::i(g_Variables.m_Visuals.m_iScopeGap), 0, 100);
                        }

                        if (Config::b(g_Variables.m_Visuals.m_bCustomScope))
                        {
                            ChlenGui::SliderInt("Scope length", &Config::i(g_Variables.m_Visuals.m_iScopeLength), 1, 500);
                            ChlenGui::SliderInt("Scope thickness", &Config::i(g_Variables.m_Visuals.m_iScopeThickness), 1, 50);
                            ChlenGui::SliderInt("Scope gap", &Config::i(g_Variables.m_Visuals.m_iScopeGap), 0, 100);
                        }

                    }
                    ChlenGui::ChildEnd();

                    break;
                }

                case ESubTabs_Visuals::SUBTAB_ENEMY:
                {

                    ImGui::BeginChild("Esp_Child", child_size, true);
                    ImGui::Text("Esp");

                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Enabled", &Config::b(g_Variables.m_Visuals.m_bEnableESP), 0);

                        ChlenGui::CheckBox("Bounding box", &Config::b(g_Variables.m_Visuals.m_bEnableBoxESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Bounding box color", &Config::c(g_Variables.m_Visuals.m_colBoxESP));

                        ChlenGui::CheckBox("Name", &Config::b(g_Variables.m_Visuals.m_bEnableNameESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Name color", &Config::c(g_Variables.m_Visuals.m_colNameEsp));

                        ChlenGui::CheckBox("Health bar", &Config::b(g_Variables.m_Visuals.m_bEnableHealthESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Health bar color", &Config::c(g_Variables.m_Visuals.m_colHealthBarEsp));

                        ChlenGui::CheckBox("Weapon", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Weapon color", &Config::c(g_Variables.m_Visuals.m_colWeaponEsp));

                        ChlenGui::CheckBox("Weapon icon", &Config::b(g_Variables.m_Visuals.m_bEnableWeaponIconESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Weapon icon color", &Config::c(g_Variables.m_Visuals.m_colWeaponIcon));

                        ChlenGui::CheckBox("Armor bar", &Config::b(g_Variables.m_Visuals.m_bEnableArmorESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Armor bar color", &Config::c(g_Variables.m_Visuals.m_colArmorBar));

                        ChlenGui::CheckBox("Ammo bar", &Config::b(g_Variables.m_Visuals.m_bEnableAmmoESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Ammo bar color", &Config::c(g_Variables.m_Visuals.m_colAmmoBar));

                        ChlenGui::CheckBox("Skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableSkeletonESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Skeleton color", &Config::c(g_Variables.m_Visuals.m_colSkeletonEsp));

                        ChlenGui::CheckBox("On shot skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableOnShotSkeletonESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("On shot skeleton color", &Config::c(g_Variables.m_Visuals.m_colOnShotSkeletonBones));

                        ChlenGui::CheckBox("Backtrack skeleton", &Config::b(g_Variables.m_Visuals.m_bEnableBacktrackSkeletonESP), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Backtrack skeleton color", &Config::c(g_Variables.m_Visuals.m_colBacktrackSkeletonEsp));

                        ChlenGui::CheckBox("Hit shock effect", &Config::b(g_Variables.m_Visuals.m_bShockEffect), 0);
                        ChlenGui::SameLine( );
                        ChlenGui::SimpleColorPicker( "##hitshockeffectclr", &Config::c( g_Variables.m_Visuals.m_colShockEffect ) );

                        ChlenGui::CheckBox("OOF arrows", &Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("OOF arrows color", &Config::c(g_Variables.m_Visuals.m_colOOFArrows));

                        if (Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows))
                        {
                            bool bValuesTheSame = Config::i(g_Variables.m_Visuals.m_bOOFArrowsWidth) == Config::i(g_Variables.m_Visuals.m_bOOFArrowsHeight);
                            std::string strCircleOvalText = bValuesTheSame ? "Circle" : "Oval";
                            ChlenGui::SliderInt(std::string("OOF " + strCircleOvalText + " Width").c_str(), &Config::i(g_Variables.m_Visuals.m_bOOFArrowsWidth), 0, 200, ImGui::GetCurrentWindow()->GetID("OOF Height"), true);
                            ChlenGui::SliderInt(std::string("OOF " + strCircleOvalText + " Height").c_str(), &Config::i(g_Variables.m_Visuals.m_bOOFArrowsHeight), 0, 200, ImGui::GetCurrentWindow()->GetID("OOF Width"), true);
                        }

                        ChlenGui::MultiComboBox( "Flags", arrFlagsNames, Config::vb( g_Variables.m_Visuals.m_vecFlags ), IM_ARRAYSIZE( arrFlagsNames ) );
                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    ImGui::BeginChild("Locacacal_Chams", child_size, true);
                    ImGui::Text("Enemy chams");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);;
                    {
                        ChlenGui::CheckBox( "Visible", &Config::b( g_Variables.m_Visuals.m_bEnableEnemyChams ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Visible color", &Config::c( g_Variables.m_Visuals.m_colEnemyChams ) );
                        ChlenGui::Combo( "Visible material", &Config::i( g_Variables.m_Visuals.m_iChamMaterialEnemy ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox( "Occluded", &Config::b( g_Variables.m_Visuals.m_bEnableEnemyChamsIgnoreZ ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Occluded color", &Config::c( g_Variables.m_Visuals.m_colEnemyChamsIgnoreZ ) );
                        ChlenGui::Combo( "Occluded material", &Config::i( g_Variables.m_Visuals.m_iChamMaterialEnemyIgnoreZ ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox( "History", &Config::b( g_Variables.m_Visuals.m_bBacktrackChams ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "History color", &Config::c( g_Variables.m_Visuals.m_colBacktrackChams ) );
                        ChlenGui::Combo( "History material", &Config::i( g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox( "History occl.", &Config::b( g_Variables.m_Visuals.m_bBacktrackChamsIgnoreZ ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "History occl. color", &Config::c( g_Variables.m_Visuals.m_colBacktrackChamsIgnoreZ ) );
                        ChlenGui::Combo( "History occl. material", &Config::i( g_Variables.m_Visuals.m_iBacktrackChamMaterialEnemy ), arrChamMaterials, IM_ARRAYSIZE( arrChamMaterials ) );

                        ChlenGui::CheckBox("Ragdoll", &Config::b(g_Variables.m_Visuals.m_bRagdollChamsEnemy), 0);

                        ChlenGui::CheckBox("Glow", &Config::b(g_Variables.m_Visuals.m_bGlow), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Glow color", &Config::c(g_Variables.m_Visuals.m_colGlow));
                    }
                    ChlenGui::ChildEnd();

                    break;
                }

                case ESubTabs_Visuals::SUBTAB_WORLD:
                {

                    ImGui::BeginChild("World_Esp", child_size, true);
                    ImGui::Text("Esp");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Dropped weapons", &Config::b(g_Variables.m_WorldVisuals.m_bWeaponNames), 0);
                        ChlenGui::CheckBox("Grenade names", &Config::b(g_Variables.m_WorldVisuals.m_bDrawGrenadeNames));

                        ChlenGui::CheckBox("Grenade prediction", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadePrediction ));
                        ImGui::SameLine( );
                        ChlenGui::SimpleColorPicker( "##GrenadePredColor", &Config::c( g_Variables.m_WorldVisuals.m_cGrenadePrediction ) );

                        ChlenGui::CheckBox("Inferno", &Config::b(g_Variables.m_WorldVisuals.m_bDrawInferno));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Inferno color", &Config::c(g_Variables.m_WorldVisuals.m_colInferno));
                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    ImGui::BeginChild("123123", child_size1, true);
                    ImGui::Text("World");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        if (ChlenGui::CheckBox("Enable", &Config::b(g_Variables.m_WorldVisuals.m_bWorldModulation)))
                        {
                            ChlenGui::Label("Walls");
                            ChlenGui::SameLine();
                            ChlenGui::SimpleColorPicker("Walls color", &Config::c(g_Variables.m_WorldVisuals.m_colWorldColor));

                            ChlenGui::Label("Lighting");
                            ChlenGui::SameLine();
                            ChlenGui::SimpleColorPicker("Lighting color", &Config::c(g_Variables.m_WorldVisuals.m_colLighting));

                            ChlenGui::CheckBox("Light rotation", &Config::b(g_Variables.m_WorldVisuals.m_bModifyLight));
                            if (Config::b(g_Variables.m_WorldVisuals.m_bModifyLight))
                            {
                                ChlenGui::SliderInt("Light Pitch (X)", &Config::i(g_Variables.m_WorldVisuals.m_iLightPitch), -360, 360);
                                ChlenGui::SliderInt("Light Yaw (Y)", &Config::i(g_Variables.m_WorldVisuals.m_iLightYaw), -360, 360);
                                ChlenGui::SliderInt("Light Roll (Z)", &Config::i(g_Variables.m_WorldVisuals.m_iLightRoll), -360, 360);
                            }

                            ChlenGui::Label("Sun");
                            ChlenGui::SameLine();
                            ChlenGui::SimpleColorPicker("Sun color", &Config::c(g_Variables.m_WorldVisuals.m_colSunColor));
                        }

                        ChlenGui::CheckBox("Inferno modulation", &Config::b(g_Variables.m_WorldVisuals.m_bParticleModulation));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Inferno modulation color", &Config::c(g_Variables.m_WorldVisuals.m_colParticleColor));

                        ChlenGui::CheckBox("Grenade modulation", &Config::b(g_Variables.m_WorldVisuals.m_bGrenadeModulation));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Grenade modulation color", &Config::c(g_Variables.m_WorldVisuals.m_colGrenade));

                        ChlenGui::CheckBox("Taser stim modulation", &Config::b(g_Variables.m_WorldVisuals.m_bTaserStimModulation));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Taser stim modulation color", &Config::c(g_Variables.m_WorldVisuals.m_colTaserStim));

                        ChlenGui::CheckBox("Hit taser shock", &Config::b(g_Variables.m_Visuals.m_bShockEffect));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("Taser shock color", &Config::c(g_Variables.m_Visuals.m_colShockEffect));

                        ChlenGui::CheckBox( "Skybox", &Config::b( g_Variables.m_WorldEffects.m_bCustomSkybox ) );
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker( "Skybox color", &Config::c( g_Variables.m_WorldEffects.m_colSkybox ) );

                        ChlenGui::Combo(X("Weather"), &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticles), arrCustomWeatherEffects, IM_ARRAYSIZE(arrCustomWeatherEffects));
                        ChlenGui::SliderInt("Effect density", &Config::i(g_Variables.m_WorldEffects.m_iSelectedCustomWorldParticleDensity), 5, 200);

                        ChlenGui::NoneSlider(X("Exposure"), &Config::i(g_Variables.m_WorldVisuals.m_nWorldExposure), 0, 200);
                        ChlenGui::NoneSlider(X("Aspect ratio value"), &Config::i(g_Variables.m_WorldEffects.m_iAspectRatio), 0, 50);

                        ChlenGui::CheckBox("LSD mode", &Config::b(g_Variables.m_WorldVisuals.m_bLSDMode));

                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();
                    ImGui::SetCursorPos(child_pos);
                    ImGui::BeginChild("Other", child_size1, true);
                    ImGui::Text("Orher");
                    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                    {
                        ChlenGui::CheckBox("Spark Shot Effect", &Config::b(g_Variables.m_Visuals.m_SparkShotEffect));
                    	ChlenGui::CheckBox("Local tracers", &Config::b(g_Variables.m_WorldEffects.m_bLocalBulletTracers));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("localclr", &Config::c(g_Variables.m_WorldEffects.m_colLocalBulletTracers));

                        ChlenGui::CheckBox("Team tracers", &Config::b(g_Variables.m_WorldEffects.m_bTeamBulletTracers));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("teamclr", &Config::c(g_Variables.m_WorldEffects.m_colTeamBulletTracers));

                        ChlenGui::CheckBox("Enemy tracers", &Config::b(g_Variables.m_WorldEffects.m_bEnemyBulletTracers));
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("enemyclr", &Config::c(g_Variables.m_WorldEffects.m_colEnemyBulletTracers));

                        ChlenGui::CheckBox("World hitmarker", &Config::b(g_Variables.m_Visuals.m_bHitmarker), 0);
                        ChlenGui::SameLine();
                        ChlenGui::SimpleColorPicker("World hitmarker color", &Config::c(g_Variables.m_Visuals.m_colHitmarker));

                        ChlenGui::SliderInt("Duration", &Config::i(g_Variables.m_Visuals.m_nHitEffectsDurration), 1, 12, 1);

                        ChlenGui::MultiComboBox("Removals", Tabs::szRemovals, Config::vb(g_Variables.m_WorldVisuals.m_vbRemovals), IM_ARRAYSIZE(Tabs::szRemovals));

                        ChlenGui::ChildEnd();
                    }

                    break;
                }
            }

            break;
        }
        case ETabs::TABS_INVENTORY:
        {
            ImGui::BeginChild("Select type", ImVec2(child_size1.x, child_size1.y + 8), true);
            {
                ImGui::Text("Select type");
                ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                ChlenGui::Label("Search");
                ChlenGui::SameLine();

                static ImGuiTextFilter itemFilter = { };
                itemFilter.Draw(X("##Item-search"), ImGui::GetContentRegionAvail().x);
                if (ChlenGui::ListBox(X("##Items-list"), ImGui::GetContentRegionAvail(), InventoryChanger::m_vecDumpedItems.size()))
                {
                    for (DumpedItem_t& item : InventoryChanger::m_vecDumpedItems)
                    {
                        if (!itemFilter.PassFilter(item.m_strName.c_str()))
                            continue;

                        if (ChlenGui::Selectable(item.m_strName.c_str(), InventoryChanger::m_pSelectedItem == &item, ImVec2(0, 0), GetRarityColor(item.m_nRarity, 1)))
                        {
                            InventoryChanger::m_pSelectedItem = &item;
                            InventoryChanger::m_pSelectedAgent = nullptr;
                        }
                    }

                    for (DumpedAgent_t& agent : InventoryChanger::m_vecDumpedAgents)
                    {
                        if (!itemFilter.PassFilter(agent.m_strName.c_str()))
                            continue;

                        if (ChlenGui::Selectable(agent.m_strName.c_str(), InventoryChanger::m_pSelectedAgent == &agent, ImVec2(0, 0), GetRarityColor(agent.m_nRarity, 1)))
                        {
                            InventoryChanger::m_pSelectedItem = nullptr;
                            InventoryChanger::m_pSelectedAgent = &agent;
                        }
                    }

                    ChlenGui::ListBoxEnd();
                }

                ChlenGui::ChildEnd();
            }

            ImGui::BeginChild("Panitkit", ImVec2(child_size1.x , child_size1.y + 8), true);
            {
                ImGui::Text("Panitkit");
                ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                ChlenGui::Label("Search");
                ChlenGui::SameLine();
                static ImGuiTextFilter skinFilter = { };
                skinFilter.Draw(X("##Skin-search"), ImGui::GetContentRegionAvail().x);

                ChlenGui::ListBox(X("##Skinselection"), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 27), InventoryChanger::m_pSelectedItem ? InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.size() : 0);
                {
                    if (InventoryChanger::m_pSelectedItem)
                    {
                        if (InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.empty())
                        {
                            ImGui::PushFont(Fonts::Default);

                            ImVec2 vecTextSize = ImGui::CalcTextSize(X("Failed to dump skins"));
                            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                            ChlenGui::Label(X("Failed to dump skins"), ImColor( 255, 0, 0 ) );

                            ImGui::PopFont();
                        }
                        else
                        {
                            for (size_t uSize = 0U; uSize < InventoryChanger::m_pSelectedItem->m_vecDumpedSkins.size(); uSize++)
                            {
                                DumpedSkin_t& skin = InventoryChanger::m_pSelectedItem->m_vecDumpedSkins[uSize];
                                if (!&skin)
                                    continue;

                                std::string strLabel = X("");
                                if (skin.m_strName._Starts_with("Doppler"))
                                {
                                    switch (skin.m_nID)
                                    {
                                    case 415:
                                        strLabel = "Doppler (Ruby)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 416:
                                        strLabel = "Doppler (Sapphire)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 417:
                                        strLabel = "Doppler (Black pearl)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 418:
                                        strLabel = "Doppler (Phase 1)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 419:
                                        strLabel = "Doppler (Phase 2)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 420:
                                        strLabel = "Doppler (Phase 3)##" + std::to_string(skin.m_nID);
                                        break;
                                    case 421:
                                        strLabel = "Doppler (Phase 4)##" + std::to_string(skin.m_nID);
                                        break;
                                    default:
                                        strLabel = "Doppler (Other)##" + std::to_string(skin.m_nID);
                                        break;
                                    }
                                }
                                else
                                    strLabel = skin.m_strName + "##" + std::to_string(skin.m_nID);

                                if (!skinFilter.PassFilter(strLabel.c_str()))
                                    continue;

                                if (ChlenGui::Selectable(strLabel.c_str(), InventoryChanger::m_pSelectedItem->m_pSelectedSkin == &skin, ImVec2(0, 0), GetRarityColor(skin.m_nRarity, 1)))
                                    InventoryChanger::m_pSelectedItem->m_pSelectedSkin = &skin;
                            }
                        }
                    }
                    else
                    {
                        ImGui::PushFont(Fonts::Default);

                        ImVec2 vecTextSize = ImGui::CalcTextSize(X("Select an item first"));
                        ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                        ChlenGui::Label(X("Select an item first"));

                        ImGui::PopFont();
                    }

                }
                ChlenGui::ListBoxEnd();

                static std::string strWeaponName;
                static float flWear = 0.0001f;
                static int nSeed = 0;
                static int nStatTrack = 0;

                if (ChlenGui::Button("Add item", ImGui::GetContentRegionAvail()))
                {
                    if (InventoryChanger::m_pSelectedItem && InventoryChanger::m_pSelectedItem->m_pSelectedSkin)
                    {
                        CEconItem* pItem = CEconItem::CreateInstance();
                        if (pItem)
                        {
                            CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                            auto HighestIDs = pInventory->GetHighestIDs();
                            pItem->m_uID = HighestIDs.first + 1;
                            pItem->m_uInventory = HighestIDs.second + 1;
                            pItem->m_uAccountID = static_cast<std::uint32_t>(pInventory->GetOwner().m_uID);
                            pItem->m_uDefinitionIndex = InventoryChanger::m_pSelectedItem->m_uDefinitionIndex;
                            if (InventoryChanger::m_pSelectedItem->m_bUnusualItem)
                                pItem->m_nRarity = IQ_UNUSUAL;

                            pItem->m_nRarity = std::clamp(InventoryChanger::m_pSelectedItem->m_nRarity + InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nRarity - 1, 0, (InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nRarity == 7) ? 7 : 6);
                            pItem->SetPaintKit(static_cast<float>(InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nID));
                            pItem->SetPaintSeed(static_cast<float>(nSeed));
                            pItem->SetPaintWear(flWear);

                            if (nStatTrack > 0)
                            {
                                pItem->SetStatTrak(nStatTrack);
                                pItem->SetStatTrakType(0);

                                if (pItem->m_nQuality != IQ_UNUSUAL)
                                    pItem->m_nQuality = IQ_STRANGE;
                            }

                            if (!strWeaponName.empty())
                                pItem->SetCustomName(strWeaponName.c_str());

                            if (pInventory->AddEconItem(pItem))
                                InventoryChanger::AddEconItemToList
                                (
                                    InventoryItem_t(
                                        InventoryChanger::m_pSelectedItem->m_strName,
                                        InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_strName,
                                        InventoryChanger::m_pSelectedItem->m_pSelectedSkin->m_nID,
                                        pItem->m_uDefinitionIndex,
                                        pItem->m_nRarity,
                                        nStatTrack,
                                        nSeed,
                                        flWear,
                                        strWeaponName,
                                        pItem->m_uID,
                                        0,
                                        false,
                                        false
                                    )

                                );

                            nStatTrack = 0;
                            nSeed = 0;
                            flWear = 0.0001f;
                            strWeaponName.clear();

                            ImGui::CloseCurrentPopup();
                        }
                    }

                    else if (InventoryChanger::m_pSelectedAgent)
                    {
                        CEconItem* pItem = CEconItem::CreateInstance();
                        if (pItem)
                        {
                            CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                            auto HighestIDs = pInventory->GetHighestIDs();
                            pItem->m_uID = HighestIDs.first + 1;
                            pItem->m_uInventory = HighestIDs.second + 1;
                            pItem->m_uAccountID = static_cast<std::uint32_t>(pInventory->GetOwner().m_uID);
                            pItem->m_uDefinitionIndex = InventoryChanger::m_pSelectedAgent->m_nDefinitionIndex;

                            pItem->m_nRarity = std::clamp(InventoryChanger::m_pSelectedAgent->m_nRarity + InventoryChanger::m_pSelectedAgent->m_nRarity - 1, 0, (InventoryChanger::m_pSelectedAgent->m_nRarity == 7) ? 7 : 6);

                            if (pInventory->AddEconItem(pItem))
                                InventoryChanger::AddEconItemToList
                                (
                                    InventoryItem_t(
                                        InventoryChanger::m_pSelectedAgent->m_strName,
                                        InventoryChanger::m_pSelectedAgent->m_strModel,
                                        -1,
                                        pItem->m_uDefinitionIndex,
                                        pItem->m_nRarity,
                                        0,
                                        0,
                                        0,
                                        X("Agent"),
                                        pItem->m_uID,
                                        0,
                                        false,
                                        false
                                    )

                                );
                        }
                    }

                    InventoryChanger::m_bWantsHUDUpdate = true;
                }

                ChlenGui::ChildEnd();
            }

            ChlenGui::SameLine();
            ImGui::SetCursorPos(ImVec2(339, 41));
            ImGui::BeginChild("Added item", ImVec2(child_size.x, child_size.y + 16), true);
            ImGui::Text("Added item");
            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);

            {
                if (InventoryChanger::m_vecAddedItems.empty())
                {
                    ImGui::PushFont(Fonts::Default);
                    ImVec2 vecTextSize = ImGui::CalcTextSize(X("No items added to inventory"));
                    ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x * 0.5f) - (vecTextSize.x * 0.5f), (ImGui::GetWindowSize().y * 0.5f) - (vecTextSize.y * 0.5f)));
                    ChlenGui::Label(X("No items added to inventory"));
                    ImGui::PopFont();
                }
                else
                {
                    static InventoryItem_t* pSelectedItem = nullptr;
                    static size_t uEntry = 0U;
                    if (ChlenGui::ListBox(X("##InventoryList"), ImVec2(-1, ImGui::GetContentRegionAvail().y - 27.0f), InventoryChanger::m_vecAddedItems.size()))
                    {
                        for (size_t uSize = 0U; uSize < InventoryChanger::m_vecAddedItems.size(); uSize++)
                        {
                            InventoryItem_t& pItem = InventoryChanger::m_vecAddedItems[uSize];
                            if (!&pItem)
                                continue;

                            std::string strLabel = X("");
                            if (pItem.m_strCustomName == X("Agent"))
                            {
                                strLabel = pItem.m_strItemName + X(" (") + pItem.m_strCustomName + X(")") + X("##") + std::to_string(uSize);
                            }
                            else if (pItem.m_strCustomName.empty())
                            {
                                strLabel = pItem.m_strItemName + X(" | ") + pItem.m_strSkinName + X("##") + std::to_string(uSize);
                            }
                            else
                            {

                                strLabel = pItem.m_strItemName + X(" (") + pItem.m_strCustomName + X(")") + X(" | ") + pItem.m_strSkinName + X("##") + std::to_string(uSize);
                            }

                            if (ChlenGui::Selectable(strLabel.c_str(), pSelectedItem == &pItem, ImVec2(0, 0)))
                            {
                                pSelectedItem = &pItem;
                                uEntry = uSize;
                            }
                        }
                        ChlenGui::ListBoxEnd();
                    }

                    if (ChlenGui::Button(X("Remove item from inventory"), ImVec2(ImGui::GetContentRegionAvail().x, 25.f)))
                    {
                        CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
                        pInventory->RemoveEconItem(pInventory->GetSOCDataForItem(InventoryChanger::m_vecAddedItems[uEntry].m_uItemID));

                        if (!InventoryChanger::m_vecAddedItems.empty())
                            InventoryChanger::m_vecAddedItems.erase(InventoryChanger::m_vecAddedItems.begin() + uEntry);
                    }
                }
            }
            ChlenGui::ChildEnd();

            break;
        }
                case ETabs::TABS_CONFIG:
                {
                    static int nSelected = -1;
                    ImGui::BeginChild("Config", ImVec2(child_size.x, child_size.y + 16), true);
                    {
                        ImGui::Text("Configs");
                        ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                        if (ChlenGui::ListBox(X("##Config-List"), ImVec2(ImGui::GetContentRegionAvail().x + 5, ImGui::GetContentRegionAvail().y), Config::vecFileNames.size()))
                        {
                            for (size_t uIndex = 0U; uIndex < Config::vecFileNames.size(); uIndex++)
                            {
                                if (ChlenGui::Selectable(Config::vecFileNames.at(uIndex).c_str(), uIndex == nSelected, ImVec2(0, 0)))
                                    nSelected = uIndex;
                            }

                            ChlenGui::ListBoxEnd();
                        }
                    }
                    ChlenGui::ChildEnd();

                    ChlenGui::SameLine();

                    static std::string strConfigFile;

                    ImGui::BeginChild("Options_cfg", ImVec2(325, 406), true);
                    {
                        ImGui::Text("Options");
                        ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
                        ChlenGui::Label("ConfigName");

                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
                        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 20.0f / 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 20.0f / 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 10.0f / 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 20.0f / 255.0f));
                        ImGui::InputText(X("##config.file"), &strConfigFile);
                        ImGui::PopStyleColor(4);
                        ImGui::PopStyleVar(1);

                        if (ChlenGui::Button("Create", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                        {
                            Config::Save(strConfigFile, true);
                            strConfigFile.clear();
                            Config::Refresh();
                        }

                        if (ChlenGui::Button("Save", ImVec2(-1, 0)))
                        {
                            if (nSelected != -1)
                            {
                                Config::Save(Config::vecFileNames.at(nSelected), true);
                                InventoryChanger::m_bWantsHUDUpdate = true;
                                InventoryChanger::OnConfigSave(Config::vecFileNames.at(nSelected));
                            }
                        }

                        if (ChlenGui::Button("Load", ImVec2(-1, 0)))
                        {
                            if (nSelected != -1)
                            {
                                Config::Load(Config::vecFileNames.at(nSelected), true);
                                InventoryChanger::m_bWantsHUDUpdate = true;
                                InventoryChanger::OnConfigLoad(Config::vecFileNames.at(nSelected));
                            }
                        }

                        if (ChlenGui::Button("Refresh", ImVec2(-1, 0)))
                        {
                            Config::Refresh();
                        }

                        if (ChlenGui::Button("Unhook", ImVec2(-1, 0)))
                        {
                            Globals::m_bIsUnloading = true;
                        }
                    }
                    ChlenGui::ChildEnd();

                    break;
                }

        case ETabs::TABS_MISC:
        {
            ImGui::BeginChild("Misc_misc", ImVec2(child_size.x , child_size.y + 16), true);
            ImGui::Text("Misc");

            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
            {
                ChlenGui::CheckBox( "Spectator list", &Config::b( g_Variables.m_Misc.m_bSpectatorList ) );
                ChlenGui::CheckBox( "Keybind list", &Config::b( g_Variables.m_Misc.m_bKeybindList ) );
                ChlenGui::CheckBox( "Hit logs", &Config::b( g_Variables.m_Misc.m_bHitlogs ) );
                ChlenGui::CheckBox( "Miss logs", &Config::b( g_Variables.m_Misc.m_bMisslogs ) );
                ChlenGui::CheckBox( "Distant scout sound", &Config::b( g_Variables.m_Misc.m_bDistantScoutSound ) );
                ChlenGui::CheckBox("Auto pistol", &Config::b(g_Variables.m_Misc.m_bAutoPistol));
                ChlenGui::CheckBox("Free cam", &Config::b(g_Variables.m_Misc.kbFreeLook));
                ChlenGui::Keybind("Free cam bind", &Config::kb(g_Variables.m_Misc.m_kbFreeLook));
                ChlenGui::SliderFloat("Free cam speed", &Config::f(g_Variables.m_Misc.m_flFreeLookSpeed), 1.0f, 50.0f);
                ChlenGui::CheckBox("HitSound", &Config::b(g_Variables.m_Misc.m_bHitSound));
                ChlenGui::Combo("HitSound type", &Config::i(g_Variables.m_Misc.m_iHitSoundIndex), szHitsounds, IM_ARRAYSIZE(szHitsounds), 0, false);
                ChlenGui::CheckBox("Show impacts", &Config::b(g_Variables.m_Misc.m_bShowImpacts));

                ImGui::Text("Accent color");

                ImGui::SameLine();
                {
                    Color accentColor(
                        (uint8_t)(g_AccentColor.x * 255.0f),
                        (uint8_t)(g_AccentColor.y * 255.0f),
                        (uint8_t)(g_AccentColor.z * 255.0f),
                        (uint8_t)(g_AccentColor.w * 255.0f)
                    );
                    if (ChlenGui::SimpleColorPicker("Accent сolor", &accentColor))
                    {
                        g_AccentColor.x = ImClamp(accentColor.rBase(), 0.0f, 1.0f);
                        g_AccentColor.y = ImClamp(accentColor.gBase(), 0.0f, 1.0f);
                        g_AccentColor.z = ImClamp(accentColor.bBase(), 0.0f, 1.0f);
                        g_AccentColor.w = 1.0f;
                    }
                }

                ChlenGui::CheckBox( "Watermark", &Config::b( g_Variables.m_Misc.m_bWatermark ), 1 );
                ChlenGui::MultiComboBox( "Watermark", szWatermark, Config::vb( g_Variables.m_Misc.m_vbWatermark ), IM_ARRAYSIZE( szWatermark ), false );
                ChlenGui::CheckBox("Buybot", &Config::b(g_Variables.m_Misc.m_bAutoPurchase));

                const char* buy1[] = { "None", "Auto-sniper", "AWP", "SSG-08" };
                const char* buy2[] = { "None", "Elite", "Five-7/Tec-9", "Deagle", "R8 Revolver" };
                const char* buy3[] = { "Armor", "Taser", "Grenades", "Defuser" };

                ChlenGui::Combo("Primary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase1), buy1, IM_ARRAYSIZE(buy1));
                ChlenGui::Combo("Secondary", &Config::i(g_Variables.m_Misc.m_iAutoPurchase2), buy2, IM_ARRAYSIZE(buy2));
                ChlenGui::MultiComboBox("Additional", buy3, Config::vb(g_Variables.m_Misc.m_uAutoPurchase3), IM_ARRAYSIZE(buy3));

#ifdef _DEBUG
                ChlenGui::CheckBox( "Debug", &Config::b( g_Variables.m_Misc.m_bDebug ) );
                if ( ChlenGui::Button( "Debug mode", ImVec2( -1, 0 ) ) )
                {
                    Interfaces::m_pEngine->ExecuteClientCmdUnrestricted( std::string(
                        std::string( "sv_cheats 1;" ) +
                        std::string( "bot_stop 1;" ) +
                        std::string( "bot_add;" ) +
                        std::string( "sv_quantize_movement_input 0;" ) +
                        std::string( "sv_airaccelerate 200;" ) +
                        std::string( "sv_steamauth_enforce 0;" ) +
                        std::string( "mp_respawn_immunitytime -1;" )
                    ).c_str( ) );
                }
#endif
            }
            ChlenGui::ChildEnd( );

            ChlenGui::SameLine( );

            ImGui::BeginChild("Misc_movement", ImVec2(325, 406), true);
            ImGui::Text("Movement");

            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
            {
                ChlenGui::CheckBox( "Bunnyhop", &Config::b( g_Variables.m_Movement.m_bEnableBunnyHop ) );
                ChlenGui::CheckBox( "Jump bug", &Config::b( g_Variables.m_Movement.m_bEnableJumpBug ) );
                ChlenGui::CheckBox( "Quickstop", &Config::b( g_Variables.m_Movement.m_bEnableQuickStop ) );
                ChlenGui::CheckBox( "Autostrafe", &Config::b( g_Variables.m_Movement.m_bEnableAutoStrafe ) );
                ChlenGui::CheckBox( "Subtick autostrafe", &Config::b( g_Variables.m_Movement.m_bEnableAutoStrafeExploit ) );
                ChlenGui::CheckBox("Edge jump", &Config::b(g_Variables.m_Movement.kEdgeJump), 1);
                ChlenGui::Keybind("Edge jump key", &Config::kb(g_Variables.m_Movement.m_kEdgeJump));
            }
            ChlenGui::ChildEnd( );
            break;
        }

        }

    }
    ImGui::SetCursorPos(ImVec2(10, 446));
    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
    ImGui::Text("Chlenixx");
    ChlenGui::End();

    blurEffect.EndBlur();
}