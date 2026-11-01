#pragma once

static constexpr ImColor LABEL_UNHOVERED_COLOR = ImColor( 255, 255, 255, 110 );
static constexpr ImColor LABEL_HOVERED_COLOR = ImColor( 255, 255, 255, 160 );
static constexpr ImColor LABEL_SELECTED_COLOR = ImColor( 255, 255, 255, 255 );

inline ImVec4 g_AccentColor = ImVec4(0.31f, 0.46f, 0.67f, 1.0f);

inline ImColor GetAccentImColor()
{
    return ImColor( g_AccentColor );
}

namespace ChlenGui
{
    inline void SetStyles(ImVec2 imWindowSize)
    {
        ImGui::SetNextWindowSize(imWindowSize);

        ImGuiStyle& imStyle = ImGui::GetStyle();

        imStyle.FrameRounding = 2.f;
        imStyle.WindowRounding = 4.f;
        imStyle.WindowBorderSize = 0.f;

        imStyle.Colors[ImGuiCol_WindowBg] = ImColor(20, 20, 20, 255);
    }

    inline void Begin(const char* szLabel)
    {
        ImGui::Begin(szLabel, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
    }

    inline void End()
    {
        ImGui::End();
    }

    void SameLine( float flOffsetFromStartX = 0.f, float flSpacingW = -1.f );

    bool CheckBox(const char* szLabel, bool* bBool, int iCustomID = 0);
    bool CheckBoxBind(const char* szLabel, bool* bBool, int* iKey, int* iKeyStyle, int iCustomID);
    bool CheckBoxStartPopup(const char* szLabel, bool* bBool, ImVec2 size, int iCustomID);
    void CheckBoxEndPopup();

    bool SliderInt(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID = 0, bool bForceCustomID = false, const char* szFormat = "%d", ImGuiSliderFlags imSliderFlags = 0);
    bool SliderFloat(const char* szLabel, float* flFloat, float flMin, float flMax, int iCustomID = 0, bool bForceCustomID = false, const char* szFormat = "%d", ImGuiSliderFlags imSliderFlags = 0);
    bool NoneSlider(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID = 0, const char* szFormat = "%d", ImGuiSliderFlags imSliderFlags = 0);
    bool MinDamageSlider(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID = 0, const char* szFormat = "%d", ImGuiSliderFlags imSliderFlags = 0);

    bool SliderScalar(const char* szLabel, ImGuiDataType imDataType, void* pData, const void* pMin, const void* pMax, const char* szFormat, ImGuiSliderFlags imSliderFlags, int iCustomID, bool bForceCustomID);

    static float CalcMaxPopupHeightFromItemCount(int iItemCount)
    {
        ImGuiContext& g = *GImGui;
        if (iItemCount <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * iItemCount - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    bool BeginCombo(const char* szLabel, const char* szCurrentValue, bool bDrawLabel = false, bool bColorPickerInline = false);

    bool MultiComboBox( const char* label, const char* items[ ], std::vector<bool>& v, int size, bool bDrawLabel = true );
    bool Combo(const char* szLabel, int* iSelectedItem, const char* szItems[], int iItemsCount, int iCustomID = 0, bool bDrawLabel = true, bool bColorPickerInline = false);
    bool BeginComboPopup(ImGuiID imPopupID, const ImRect& imFrameBB);

    void CheckboxComboColor(const char* strCheckboxLabel, bool* pbBool, int* piSelectedItem, const char* szItems[ ], int iNumItems, Color* pcColor );

    bool EndCombo();

    void Label( const char* szFmt, ImColor cColor = ImColor( 255, 255, 255, 255 ) );
    bool LabelStartPopup(const char* szLabel, ImVec2 size);
    void LabelEndPopup();

    void RenderArrow(ImGuiWindow* imWindow, ImVec2 imPos, ImColor imCol, float flAngle, float flScale);

    bool Button(const char* szLabel, ImVec2 imSize = ImVec2(0, 0), ImColor imLabelColor = ImColor(255, 255, 255, 255), int iCustomID = 0);
    bool ComboButton(const char* szLabelm, ImColor imLabelColor, bool bIsSelectedItem, int iCustomID = 0);
    bool TabButton(const char* szLabel, int iThisTab, int iCurrentTab, int iTextAlign, ImVec2 imSize = ImVec2(0, 0), int iCustomID = 0);

    bool Child(const char* szLabel, ImVec2* imSize = nullptr, bool bDrawLabel = true);
    bool ChildEnd();

    bool ListBox(const char* szLabel, ImVec2 imSize, int iItemsCount);
    bool ListBoxEnd();

    bool Selectable(const char* szLabel, bool bSelected, ImVec2 imSize, Color colForegroundColorOverride = Color(69, 69, 69));

    bool ColorPicker(const char* szLabel, ImColor* imColor, int iColorPickersOnLine = 1);
    bool ColorButton(const char* szDescId, const ImVec4& imCol, ImGuiColorEditFlags imColorEditFlags, const ImVec2& imSize);
    bool ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col);
    bool SimpleColorPicker( const std::string strName, Color* colColor, int iColorPickersOnLine = 1 );
    void CheckboxColor(const char* label, ImVec4* color);

    bool IsKeyDown(int user_key_index);

    bool IsKeyDown(ImGuiKey key);

    bool IsKeyDown(ImGuiKey key, ImGuiID owner_id);

    bool IsKeyPressed(ImGuiKey key, bool repeat);

    bool IsKeyPressed(ImGuiKey key, ImGuiID owner_id, ImGuiInputFlags flags);

    bool IsKeyReleased(ImGuiKey key);

    bool IsKeyReleased(ImGuiKey key, ImGuiID owner_id);

    bool Keybind( const char* str_id, KeyBind_t* keybind );

    bool PopupBegin(ImVec2 size);
    void PopupEnd();

    void CustomSeparator(float& thickness, ImVec4& color, float padding);

    void RenderMenuTabs(const char* tabs[], int tabs_count, int* selected_tab);
    void RenderSubTabs(const char* sub_tabs[], int sub_tabs_count, int* selected_sub_tab);
    void RenderExampleMenu();
};

const char* const KeyNames[ ] = {
	"None",
	"M1",
	"M2",
	"Esc",
	"M3",
	"M4",
	"M5",
	"None",
	"Back",
	"Tab",
	"None",
	"None",
	"BSpace",
	"Enter",
	"None",
	"None",
	"Shift",
	"Ctrl",
	"Alt",
	"PB",
	"CL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"Esc",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"Print Screen",
	"Ins",
	"Del",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Left Windows",
	"Right Windows",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"Multiply",
	"+",
	"Separator",
	"Subtract",
	"-",
	"/",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Num Lock",
	"Scroll lock",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"LShift",
	"RShift",
	"LCtrl",
	"RCtrl",
	"LAlt",
	"RAlt"
};

