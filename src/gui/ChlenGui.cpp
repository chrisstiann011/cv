#include "../Precompiled.h"

#define IMGUI_DEFINE_MATH_OPERATORS

using namespace ImGui;

struct CheckBoxObj_t
{

    float               flFrameRounding = 2.f;
    float                        flSize = 16.f;
    float          flAnimationTimeLabel = 20.f;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);

    bool                     bDrawDebug = false;

    bool bRenderCheckMarkWhen_Unchecked = true;

    ImColor       imCheckMark_Unchecked = ImColor(0, 0, 0, 0);

    ImColor         imCheckMark_Hovered = ImColor(200, 200, 200, 120);

    bool   bRenderCheckMarkWhen_Hovered = true;
    float       flAnimationTime_Hovered = 20.f;

    ImColor         imCheckMark_Checked = ImColor(255, 20, 147, 255);

    bool   bRenderCheckMarkWhen_Checked = true;
    float       flAnimationTime_Checked = 20.f;
};

void ChlenGui::SameLine( float flOffsetFromStartX, float flSpacingW )
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* pWindow = g.CurrentWindow;

    if ( pWindow->SkipItems )
        return;

    if ( flOffsetFromStartX != 0.0f )
    {
        if ( flSpacingW < 0.0f )
            flSpacingW = 0.0f;

        pWindow->DC.CursorPos.x = pWindow->Pos.x - pWindow->Scroll.x + flOffsetFromStartX + flSpacingW + pWindow->DC.GroupOffset.x + pWindow->DC.ColumnsOffset.x;
        pWindow->DC.CursorPos.y = pWindow->DC.CursorPosPrevLine.y;
    }
    else
    {
        if ( flSpacingW < 0.0f )
            flSpacingW = g.Style.ItemSpacing.x;

        pWindow->DC.CursorPos.x = pWindow->DC.CursorPosPrevLine.x + flSpacingW;
        pWindow->DC.CursorPos.y = pWindow->DC.CursorPosPrevLine.y;
    }

    pWindow->DC.CurrLineSize = pWindow->DC.PrevLineSize;
    pWindow->DC.CurrLineTextBaseOffset = pWindow->DC.PrevLineTextBaseOffset;
    pWindow->DC.IsSameLine = true;
}

bool ChlenGui::CheckBox(const char* szLabel, bool* bBool, int iCustomID)
{
    static CheckBoxObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID * 8662;

    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);
    const float flSquare = obj.flSize;

    const ImVec2 imPos = imWindow->DC.CursorPos;
    const ImRect imTotalBB(
        imPos,
        imPos + ImVec2(
            flSquare + (imLabelSize.x > 0.0f ? style.ItemInnerSpacing.x + imLabelSize.x : 0.0f),
            flSquare
        )
    );

    const bool bIsVisible = ItemAdd(imTotalBB, imID);
    ItemSize(imTotalBB);

    bool bChecked = *bBool;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld);

    if (bPressed)
    {
        bChecked = !bChecked;
        *bBool = bChecked;
        MarkItemEdited(imID);
    }

    const ImRect imCheckBb(imPos, imPos + ImVec2(flSquare, flSquare));

    if (bIsVisible)
    {

        ImU32 empty = ImGui::GetColorU32(IM_COL32(35, 35, 35, 255));
        ImU32 fill = ImGui::GetColorU32(g_AccentColor);
        ImU32 border = ImGui::GetColorU32(bHovered ? ImVec4(0.2f, 0.2f, 0.2f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

        if (*bBool)
            imWindow->DrawList->AddRectFilled(imCheckBb.Min, imCheckBb.Max, fill, 0.0f);
        else
            imWindow->DrawList->AddRectFilled(imCheckBb.Min, imCheckBb.Max, empty, 0.0f);

        imWindow->DrawList->AddRect(imCheckBb.Min, imCheckBb.Max, border, 0.0f, 0, 1.5f);

        ImColor labelColor;
        if (*bBool)
            labelColor = LABEL_SELECTED_COLOR;
        else if (bHovered)
            labelColor = LABEL_HOVERED_COLOR;
        else
            labelColor = LABEL_UNHOVERED_COLOR;

        const float centerY = (imCheckBb.Min.y + imCheckBb.Max.y) * 0.5f;
        const float labelY = centerY - imLabelSize.y * 0.5f;
        const ImVec2 labelPos(imCheckBb.Max.x + 4.0f, labelY);

        imWindow->DrawList->AddText(labelPos, labelColor, szLabel);
    }

    return bChecked;
}

template<typename T>
static void SliderBase(const char* label, T* value, T min, T max, T step, int iCustomID)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    IM_ASSERT(value && min < max);

    std::string label_id = std::string("##") + label + std::to_string(iCustomID);
    ImGui::PushID(label_id.c_str());

    ImGui::TextUnformatted(label);

    ImGui::SameLine();

    char value_buf[32];
    if constexpr (std::is_floating_point<T>::value)
        _snprintf_s(value_buf, sizeof(value_buf), _TRUNCATE, "%.1f", *value);
    else
        _snprintf_s(value_buf, sizeof(value_buf), _TRUNCATE, "%d", (int)*value);

    float value_width = ImGui::CalcTextSize(value_buf).x;
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - value_width);
    ImGui::TextUnformatted(value_buf);

    float bar_height = 10.0f;
    float full_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4.0f);

    ImVec2 bar_pos = ImGui::GetCursorScreenPos();
    ImVec2 bar_end(bar_pos.x + full_width, bar_pos.y + bar_height);

    window->DrawList->AddRectFilled(
        ImVec2(bar_pos.x - 1, bar_pos.y - 1),
        ImVec2(bar_end.x + 1, bar_end.y + 1),
        IM_COL32(60, 60, 60, 255), 0.0f
    );

    window->DrawList->AddRectFilled(
        bar_pos, bar_end,
        IM_COL32(35, 35, 35, 255), 0.0f
    );

    static std::unordered_map<ImGuiID, float> anim;
    ImGuiID id = window->GetID(label_id.c_str());

    float target = float(*value - min) / float(max - min);
    float& t = anim[id];
    t = ImLerp(t, target, ImClamp(99999.0f * g.IO.DeltaTime, 0.0f, 1.0f));

    float fill_width = full_width * t;
    if (fill_width > 1.0f)
    {
        window->DrawList->AddRectFilled(
            bar_pos,
            ImVec2(bar_pos.x + fill_width, bar_end.y),
            ImGui::GetColorU32(g_AccentColor), 0.0f
        );
    }

    ImGui::SetCursorScreenPos(bar_pos);
    ImGui::InvisibleButton(label_id.c_str(), ImVec2(full_width, bar_height));

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        float rel_x = ImClamp(
            ImGui::GetIO().MousePos.x - bar_pos.x,
            0.0f, full_width
        );

        T raw = min + T((rel_x / full_width) * (max - min));
        T stepped = T(((raw + step * T(0.5)) / step)) * step;
        *value = ImClamp(stepped, min, max);
    }

    ImGui::PopID();
}

bool ChlenGui::SliderInt(const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, bool bForceCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    int step = 1;
    SliderBase(szLabel, iInt, iMin, iMax, step, iCustomID);
    return true;
}

struct SliderObj_t
{

    float               flFrameRounding = 0.f;
    float                    flWidthAdd = 0.f;
    float                    flWidthSub = 0.f;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);
    ImColor               imSliderColor = ImColor(255, 20, 147, 255);

    bool                     bDrawDebug = false;

    float         flAnimationTime_Value = 10.f;

    float        flAnimationTime_Hover = 10.f;

    ImColor        imSliderColor_Active = ImColor(255, 20, 147, 255);
    float        flAnimationTime_Active = 25.f;
};

bool ChlenGui::NoneSlider( const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags )
{
    int step = 1;
    SliderBase(szLabel, iInt, iMin, iMax, step, iCustomID);
    return true;
}

bool ChlenGui::MinDamageSlider( const char* szLabel, int* iInt, int iMin, int iMax, int iCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags )
{
    int step = 1;
    SliderBase(szLabel, iInt, iMin, iMax, step, iCustomID);
    return true;
}

bool ChlenGui::SliderFloat(const char* szLabel, float* flFloat, float flMin, float flMax, int iCustomID, bool bForceCustomID, const char* szFormat, ImGuiSliderFlags imSliderFlags)
{
    float step = 0.1f;
    SliderBase(szLabel, flFloat, flMin, flMax, step, iCustomID);
    return true;
}

bool ChlenGui::SliderScalar( const char* szLabel, ImGuiDataType imDataType, void* pData, const void* pMin, const void* pMax, const char* szFormat, ImGuiSliderFlags imSliderFlags, int iCustomID, bool bForceCustomID )
{
    static SliderObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow( );
    if ( imWindow->SkipItems )
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = bForceCustomID ? iCustomID : imWindow->GetID( szLabel ) + iCustomID;
    float flFullWidth = GetContentRegionAvail( ).x;

    const ImVec2 imLabelSize = CalcTextSize( szLabel, NULL, true );
    const float flSliderHeight = imLabelSize.y;

    ImVec2 imLabelPos = imWindow->DC.CursorPos;

    ImVec2 sliderPosMin( imLabelPos.x, imLabelPos.y + imLabelSize.y + style.ItemInnerSpacing.y * 0.5f );
    ImVec2 sliderPosMax( sliderPosMin.x + flFullWidth, sliderPosMin.y + flSliderHeight );

    ImRect imFrameBB( sliderPosMin, sliderPosMax );
    const ImRect imTotalBB( imLabelPos, sliderPosMax );

    const bool bTempInputAllowed = ( imSliderFlags & ImGuiSliderFlags_NoInput ) == 0;
    ItemSize( imTotalBB, style.FramePadding.y );
    if ( !ItemAdd( imTotalBB, imID, &imFrameBB, bTempInputAllowed ? ImGuiItemFlags_Inputable : 0 ) )
        return false;

    if ( szFormat == NULL )
        szFormat = DataTypeGetInfo( imDataType )->PrintFmt;

    const bool bHovered = ItemHoverable( imTotalBB, imID, g.LastItemData.ItemFlags );
    const bool bClicked = bHovered && IsMouseClicked( 0, ImGuiInputFlags_None, imID );
    const bool bItemActive = IsItemActive( );

    bool bTemmpInputActive = bTempInputAllowed && TempInputIsActive( imID );
    if ( !bTemmpInputActive )
    {
        const bool bMakeActive = ( bClicked || g.NavActivateId == imID );
        if ( bMakeActive && bClicked )
            SetKeyOwner( ImGuiKey_MouseLeft, imID );
        if ( bMakeActive )
        {
            SetActiveID( imID, imWindow );
            SetFocusID( imID, imWindow );
            FocusWindow( imWindow );
            g.ActiveIdUsingNavDirMask |= ( 1 << ImGuiDir_Left ) | ( 1 << ImGuiDir_Right );
        }
    }

    ImRect imGrabBB;
    const bool bValueChanged = SliderBehavior( imFrameBB, imID, imDataType, pData, pMin, pMax, szFormat, imSliderFlags, &imGrabBB );
    if ( bValueChanged )
        MarkItemEdited( imID );

    float* flAnimatedValue = ImGui::GetStateStorage( )->GetFloatRef( imID + 1, *(float*)pData );
    float targetValue = (float)( *(int*)pData );
    *flAnimatedValue = ImLerp( *flAnimatedValue, targetValue, ImGui::GetIO( ).DeltaTime * obj.flAnimationTime_Active );

    float targetAlpha = LABEL_UNHOVERED_COLOR.Value.w;
    if ( bItemActive )
        targetAlpha = LABEL_SELECTED_COLOR.Value.w;
    else if ( bHovered )
        targetAlpha = LABEL_HOVERED_COLOR.Value.w;

    float* labelAlpha = ImGui::GetStateStorage( )->GetFloatRef( imID + 2000, 0.3f );
    float labelAnimSpeed = ImGui::GetIO( ).DeltaTime * obj.flAnimationTime_Hover;
    *labelAlpha = ImLerp( *labelAlpha, targetAlpha, labelAnimSpeed );

    float* valueAlpha = ImGui::GetStateStorage( )->GetFloatRef( imID + 3000, 0.0f );
    float valueSpeed = ImGui::GetIO( ).DeltaTime * obj.flAnimationTime_Value;
    *valueAlpha = ImClamp( *valueAlpha + ( ( bHovered || bItemActive ) ? valueSpeed : -valueSpeed ), 0.0f, 1.0f );

    int min = *(int*)pMin;
    int max = *(int*)pMax;
    float fraction = ImClamp( ( *flAnimatedValue - min ) / (float)( max - min ), 0.0f, 1.0f );
    float animatedX = ImLerp( imFrameBB.Min.x, imFrameBB.Max.x, fraction );
    ImVec2 imEndSliderPos = ImVec2( animatedX, imFrameBB.Max.y );

    imWindow->DrawList->AddRectFilled(
        imFrameBB.Min + ImVec2( 0, 3 ),
        imFrameBB.Max - ImVec2( 0, 3 ),
        obj.imBackgroundColor,
        obj.flFrameRounding
    );

    {
        ImU32 sliderColor = ImGui::GetColorU32(g_AccentColor);
    imWindow->DrawList->AddRectFilled(
        ImVec2( imFrameBB.Min.x, imFrameBB.Min.y + 3 ),
        ImVec2( animatedX, imFrameBB.Max.y - 3 ),
        sliderColor,
        obj.flFrameRounding
    );
    }

    if ( *valueAlpha > 0.0f )
    {
        char szValueBuf[ 64 ];
        const char* szValueBufEnd = szValueBuf + DataTypeFormatString( szValueBuf, IM_ARRAYSIZE( szValueBuf ), imDataType, pData, szFormat );
        ImVec2 imValueTextSize = CalcTextSize( szValueBuf, szValueBufEnd );

        ImVec2 padding( 4.0f, 2.0f );
        ImVec2 boxSize = imValueTextSize + padding * 2.0f;

        ImVec2 imValueTextPos = ImVec2( animatedX - boxSize.x / 2.0f + padding.x, imFrameBB.Min.y - imValueTextSize.y - 6 );

        ImVec2 bgMin = imValueTextPos - padding;
        ImVec2 bgMax = imValueTextPos + imValueTextSize + padding;

        float boxLeft = bgMin.x;
        float boxRight = bgMax.x;
        float sliderLeft = imFrameBB.Min.x;
        float sliderRight = imFrameBB.Max.x;

        float shift = 0.0f;
        if ( boxLeft < sliderLeft )
            shift = sliderLeft - boxLeft;
        else if ( boxRight > sliderRight )
            shift = sliderRight - boxRight;

        imValueTextPos.x += shift;
        bgMin.x += shift;
        bgMax.x += shift;

        imWindow->DrawList->AddRectFilled( bgMin, bgMax, ImColor( 35, 35, 35, 255 ), 4.0f );
        imWindow->DrawList->AddText( imValueTextPos, ImColor( LABEL_SELECTED_COLOR.Value.x, LABEL_SELECTED_COLOR.Value.y, LABEL_SELECTED_COLOR.Value.z, *valueAlpha ), szValueBuf );

        float newCursorY = bgMax.y + style.ItemSpacing.y;
        if ( imWindow->DC.CursorPos.y < newCursorY )
            imWindow->DC.CursorPos.y = newCursorY;
    }

    imWindow->DrawList->AddText( imLabelPos, ImColor( LABEL_SELECTED_COLOR.Value.x, LABEL_SELECTED_COLOR.Value.y, LABEL_SELECTED_COLOR.Value.z, *labelAlpha ), szLabel );

    if ( obj.bDrawDebug )
        imWindow->DrawList->AddRect( imTotalBB.Min, imTotalBB.Max, IM_COL32( 255, 0, 0, 80 ) );

    return bValueChanged;
}

struct BeginComboObj_t
{

    float               flFrameRounding = 0.f;
    float                    flWidthAdd = 0.f;
    float                    flWidthSub = 300.f;
    float          flAnimationTimeArrow = 8.f;
    float              flPaddingForText = 2.f;
    float          flAnimationTimeLabel = 8.f;
    float          flAnimationTimePopup = 12.f;

    ImColor           imBackgroundColor = ImColor(40, 40, 40, 255);
    ImColor                imArrowColor = ImColor(255, 20, 147, 255);

    bool                     bDrawDebug = false;

};

bool ChlenGui::BeginCombo( const char* szLabel, const char* szCurrentValue, bool bDrawLabel, bool bColorPickerInline )
{
    static BeginComboObj_t obj;

    ImGuiContext& g = *GImGui;
    ImGuiWindow* imWindow = GetCurrentWindow( );

    g.NextWindowData.ClearFlags( );
    if ( imWindow->SkipItems )
        return false;

    const ImGuiStyle& imStyle = g.Style;
    const ImGuiID imID = imWindow->GetID( szLabel );

    ImVec2 label_size( 0.0f, 0.0f );
    const char* render_label_text_ptr = szLabel;

    if ( bDrawLabel ) {
        label_size = CalcTextSize( szLabel, NULL, true );
        if ( label_size.x == 0.0f ) {
            bDrawLabel = false;
            render_label_text_ptr = NULL;
        }
    }
    else {
        render_label_text_ptr = NULL;
    }

    const float flFrameHeight = GetFrameHeight( ) - 2.f;

    float flWidthForComboAndPicker = GetContentRegionAvail( ).x;
    if ( bDrawLabel ) {
        flWidthForComboAndPicker -= ( label_size.x + imStyle.ItemInnerSpacing.x );
        flWidthForComboAndPicker = ImMax( 1.0f, flWidthForComboAndPicker );
    }

    float flComboActualWidth = flWidthForComboAndPicker;
    if ( bColorPickerInline ) {
        flComboActualWidth -= ( 14.0f  + 10.0f  );
        flComboActualWidth = ImMax( 1.0f, flComboActualWidth );
    }

    ImVec2 combo_frame_pos = imWindow->DC.CursorPos;
    if ( bDrawLabel )
    {
        combo_frame_pos.x += label_size.x + imStyle.ItemInnerSpacing.x;
    }
    else
    {
        combo_frame_pos.y -= 1.5f;
    }

    const ImRect imFrameBB(
        combo_frame_pos,
        combo_frame_pos + ImVec2( flComboActualWidth, flFrameHeight )
    );

    const ImRect imTotalBB(
        imWindow->DC.CursorPos,
        imWindow->DC.CursorPos + ImVec2(
            ( bDrawLabel ? label_size.x + imStyle.ItemInnerSpacing.x : 0.0f ) + flWidthForComboAndPicker,
            flFrameHeight
        )
    );

    ItemSize( imTotalBB, imStyle.FramePadding.y );
    if ( !ItemAdd( imTotalBB, imID, &imFrameBB ) )
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior( imFrameBB, imID, &bHovered, &bHeld );
    const ImGuiID imPopupID = ImHashStr( "##ComboPopup", 0, imID );
    bool bPopupOpen = IsPopupOpen( imPopupID, ImGuiPopupFlags_None );

    if ( bPressed && !bPopupOpen ) {
        OpenPopupEx( imPopupID, ImGuiPopupFlags_None );
        bPopupOpen = true;
    }

    imWindow->DrawList->AddRectFilled( imFrameBB.Min, imFrameBB.Max, obj.imBackgroundColor, obj.flFrameRounding );

    const float flArrowSize = GetFrameHeight( );

    float* flArrowAngle = ImGui::GetStateStorage( )->GetFloatRef( imID, 0.0f );

    float flTargetArrowAngle = bPopupOpen ? IM_PI * 0.5f : -IM_PI * 0.5f;
    *flArrowAngle = ImLerp( *flArrowAngle, flTargetArrowAngle, ImGui::GetIO( ).DeltaTime * obj.flAnimationTimeArrow );

    *flArrowAngle = ImClamp( *flArrowAngle, -90.f, 90.f );

    const float flValue2x = ImMax( imFrameBB.Min.x, imFrameBB.Max.x - flArrowSize );

    if ( flValue2x + flArrowSize - imStyle.FramePadding.x <= imFrameBB.Max.x )
    {
        ImColor arrowColor = ImColor(82, 120, 176, 255);
        RenderArrow( imWindow, ImVec2( flValue2x + imStyle.FramePadding.y, imFrameBB.Min.y + imStyle.FramePadding.y ), arrowColor, *flArrowAngle, 0.7f );
    }

    RenderFrameBorder( imFrameBB.Min, imFrameBB.Max, 0.0f );

    float* flInterpolatedLabelAlpha = ImGui::GetStateStorage( )->GetFloatRef( imID + 1, 0.f );
    float flTargetAlpha_Selected = bPopupOpen ? 1.f : LABEL_UNHOVERED_COLOR.Value.w;
    *flInterpolatedLabelAlpha = ImLerp( *flInterpolatedLabelAlpha, flTargetAlpha_Selected, ImGui::GetIO( ).DeltaTime * obj.flAnimationTimeLabel );
    *flInterpolatedLabelAlpha = ImClamp( *flInterpolatedLabelAlpha, 0.0f, 1.0f );

    if ( bDrawLabel && render_label_text_ptr ) {
        ImVec4 base_color_rgb_values;
        if ( bPopupOpen )
            base_color_rgb_values = LABEL_HOVERED_COLOR.Value;
        else
            base_color_rgb_values = LABEL_UNHOVERED_COLOR.Value;

        ImVec4 final_label_color = ImVec4( base_color_rgb_values.x, base_color_rgb_values.y, base_color_rgb_values.z, *flInterpolatedLabelAlpha );

        ImVec2 label_render_pos = ImVec2(
            imWindow->DC.CursorPos.x,
            imFrameBB.Min.y + ( flFrameHeight - label_size.y ) * 0.5f
        );
        imWindow->DrawList->AddText( label_render_pos, ColorConvertFloat4ToU32( final_label_color ), render_label_text_ptr );
    }

    ImVec2 imSelectedTextPos = ImVec2(
        imFrameBB.Min.x + imStyle.FramePadding.x,
        ( imFrameBB.Min.y + imFrameBB.Max.y ) * 0.5f - CalcTextSize( szCurrentValue ).y * 0.5f
    );

    float selected_text_max_x = imFrameBB.Max.x - flArrowSize - imStyle.FramePadding.x;
    ImRect clip_rect_text( imFrameBB.Min.x + imStyle.FramePadding.x, imFrameBB.Min.y, selected_text_max_x, imFrameBB.Max.y );

    imWindow->DrawList->PushClipRect( clip_rect_text.Min, clip_rect_text.Max, true );
    imWindow->DrawList->AddText( imSelectedTextPos, LABEL_SELECTED_COLOR, szCurrentValue );
    imWindow->DrawList->PopClipRect( );

    if ( obj.bDrawDebug ) {
        imWindow->DrawList->AddRect( imTotalBB.Min, imTotalBB.Max, IM_COL32( 255, 0, 0, 120 ) );
        imWindow->DrawList->AddRect( imFrameBB.Min, imFrameBB.Max, IM_COL32( 0, 255, 0, 120 ) );
    }

    if ( bPopupOpen )
        if ( ChlenGui::BeginComboPopup( imPopupID, imFrameBB ) )
            return true;

    return false;
}

bool ChlenGui::BeginComboPopup(ImGuiID imPopupID, const ImRect& imFrameBB)
{
    static BeginComboObj_t obj;

    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(imPopupID, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    float w = imFrameBB.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {

        int popup_max_height_in_items = 8;

        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f)
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = CalcMaxPopupHeightFromItemCount(popup_max_height_in_items);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {

            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = ImGuiDir_Left;
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(imFrameBB.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, imFrameBB, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos + ImVec2(0, 4));
        }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

    PushStyleVar(ImGuiStyleVar_PopupRounding, obj.flFrameRounding);

    PushStyleColor(ImGuiCol_PopupBg, obj.imBackgroundColor.Value);
    PushStyleColor(ImGuiCol_Border, ImColor(25, 25, 25, 180).Value);

    bool ret = ImGui::Begin(name, NULL, window_flags);

    PopStyleColor(2);
    PopStyleVar(2);

    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);
        return false;
    }
    return true;
}

void ChlenGui::CheckboxComboColor( const char* strCheckboxLabel, bool* pbBool, int* piSelectedItem, const char* szItems[ ], int iNumItems, Color* pcColor )
{
    CheckBox( strCheckboxLabel, pbBool );
    SameLine( );
    Combo(
        std::string( "##" + std::string( strCheckboxLabel ) + "combo" ).c_str( ),
        piSelectedItem,
        szItems,
        iNumItems,
        0, false, true );
    SameLine( );
    SimpleColorPicker(
        std::string( "##" + std::string( strCheckboxLabel ) + "color" ).c_str( ),
        pcColor );
    SetCursorPosY( GetCursorPosY( ) - 3 );
}

struct ComboButtonObj_t
{

    bool                     bDrawDebug = false;

    ImColor       imCircleColor_Pressed = ImColor(82, 120, 176, 255);

    bool             bDrawIndentedLabel = true;
};

bool ChlenGui::ComboButton(const char* szLabel, ImColor imLabelColor, bool bIsSelectedItem, int iCustomID)
{
    static ComboButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 imSize = CalcItemSize(ImVec2(-1, 0), imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + 1.f * 0.5f);

    const ImRect imTotalBB(imPos, imPos + imSize);
    ItemSize(imSize);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    if (obj.bDrawDebug)
    {
        imWindow->DrawList->AddRect(imTotalBB.Min, imTotalBB.Max, ImColor(255, 0, 0, 70));
    }

    ImVec2 imLabelTextPos = imTotalBB.Min;

    if (bIsSelectedItem && obj.bDrawIndentedLabel)
    {
        imLabelTextPos.x += 4;
    }

    imWindow->DrawList->AddText(imLabelTextPos, imLabelColor, szLabel);

    return bPressed;
}

struct TabButtonObj_t
{

    ImColor imDefaultColor = ImColor(255, 255, 255, 100);
    float flAnimationTime = 15.f;

    ImColor imHoveredColor = ImColor(255, 255, 255, 180);

    ImColor imSelectedColor = ImColor(255, 255, 255, 255);
};

bool ChlenGui::TabButton(const char* szLabel, int iThisTab, int iCurrentTab, int iTextAlign, ImVec2 imSize, int iCustomID)
{
    static TabButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    float* flAnimatedAlpha = ImGui::GetStateStorage()->GetFloatRef(imID, 0.0f);
    float flTargetAlpha = (iCurrentTab == iThisTab) ? 1.0f : (bHovered ? 0.8f : 0.5f);
    *flAnimatedAlpha = ImLerp(*flAnimatedAlpha, flTargetAlpha, ImGui::GetIO().DeltaTime * obj.flAnimationTime);

    ImColor imButtonColor = obj.imDefaultColor;
    if (bHovered && !bPressed)
    {
        imButtonColor = obj.imHoveredColor;
    }
    if (iCurrentTab == iThisTab)
    {
        imButtonColor = obj.imSelectedColor;
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

struct ComboObj_t
{

    float  flAnimationTimeItem_Selected = 4.f;

};

bool ChlenGui::MultiComboBox( const char* label, const char* items[ ], std::vector<bool>& v, int size, bool bDrawLabel )
{
    if (!items || size <= 0 || v.size() < static_cast<size_t>(size))
        return false;

    bool value_changed = false;

    if (bDrawLabel)
        ImGui::TextUnformatted(label);

    std::string preview;
    for (int i = 0; i < size; i++)
    {
        if (v[i])
        {
            if (!preview.empty())
                preview += ", ";
            preview += items[i];
        }
    }
    if (preview.empty())
        preview = "None";

    float combo_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(combo_width);
    ImVec4 gray(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 20, 20, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, gray);
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(20, 20, 20, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(28, 28, 28, 255));

    std::string label_id = std::string("##") + label;

    if (ImGui::BeginCombo(label_id.c_str(), preview.c_str(), ImGuiComboFlags_NoArrowButton))
    {
        for (int i = 0; i < size; i++)
        {
            bool current_value = v[i];
            if (ImGui::Selectable(items[i], current_value, ImGuiSelectableFlags_DontClosePopups))
            {
                v[i] = !v[i];
                value_changed = true;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar(2);

    return value_changed;
}

bool ChlenGui::Combo( const char* szLabel, int* iSelectedItem, const char* szItems[ ], int iItemsCount, int iCustomID, bool bDrawLabel, bool bColorPickerInline )
{
    if (!iSelectedItem || !szItems || iItemsCount <= 0)
        return false;

    bool value_changed = false;

    if (bDrawLabel)
        ImGui::TextUnformatted(szLabel);

    const char* preview = (*iSelectedItem >= 0 && *iSelectedItem < iItemsCount) ? szItems[*iSelectedItem] : "None";

    float combo_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(combo_width);
    ImVec4 gray(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 20, 20, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, gray);
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(20, 20, 20, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(28, 28, 28, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(28, 28, 28, 255));

    std::string label_id = std::string("##") + szLabel + std::to_string(iCustomID);

    if (ImGui::BeginCombo(label_id.c_str(), preview, ImGuiComboFlags_NoArrowButton))
    {
        for (int i = 0; i < iItemsCount; i++)
        {
            bool selected = (*iSelectedItem == i);
            if (ImGui::Selectable(szItems[i], selected))
            {
                *iSelectedItem = i;
                value_changed = true;
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar(2);

    return value_changed;
}

bool ChlenGui::EndCombo()
{
    ImGuiContext& g = *GImGui;
    EndPopup();
    g.BeginComboDepth--;

    return true;
}

void ChlenGui::Label( const char* szFmt, ImColor cColor )
{
    ImGui::TextColored( cColor, szFmt );
}

void ChlenGui::RenderArrow(ImGuiWindow* imWindow, ImVec2 imPos, ImColor imCol, float flAngle, float flScale)
{
    const float h = imWindow->DrawList->_Data->FontSize * 1.00f;
    const float r = h * 0.40f * flScale;
    const ImVec2 center = imPos + ImVec2(h * 0.50f, h * 0.50f * flScale);

    ImVec2 a = ImVec2(cosf(flAngle), sinf(flAngle)) * r;
    ImVec2 b = ImVec2(cosf(flAngle + IM_PI * 2.0f / 3.0f), sinf(flAngle + IM_PI * 2.0f / 3.0f)) * r;
    ImVec2 c = ImVec2(cosf(flAngle - IM_PI * 2.0f / 3.0f), sinf(flAngle - IM_PI * 2.0f / 3.0f)) * r;

    imWindow->DrawList->AddTriangleFilled(center + a, center + b, center + c, imCol);
}

struct ButtonObj_t
{

    ImColor imBackgroundColor_Unhovered = ImColor(255, 255, 255, 20);

    ImColor   imBackgroundColor_Hovered = ImColor(255, 255, 255, 20);

    ImColor   imBackgroundColor_Pressed = ImColor(255, 255, 255, 10);
};

bool ChlenGui::Button(const char* szLabel, ImVec2 imSize, ImColor imLabelColor, int iCustomID)
{
    static ButtonObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID imID = imWindow->GetID(szLabel) + iCustomID;
    const ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);

    ImVec2 imPos = imWindow->DC.CursorPos;

    ImVec2 size = CalcItemSize(imSize, imLabelSize.x + style.FramePadding.x * 2.0f, imLabelSize.y + style.FramePadding.y * 2.0f);

    const ImRect imTotalBB(imPos, imPos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(imTotalBB, imID))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imTotalBB, imID, &bHovered, &bHeld, 0);

    ImColor imButtonColor = obj.imBackgroundColor_Unhovered;

    if (bHovered && !bPressed && !bHeld)
    {
        imButtonColor = obj.imBackgroundColor_Hovered;
    }

    if (bPressed || bHeld)
    {
        imButtonColor = obj.imBackgroundColor_Pressed;
    }

    float oldFrameRounding = style.FrameRounding;
    const float flNoRounding = 0.0f;

    GetWindowDrawList()->AddRect(imTotalBB.Min, imTotalBB.Max, ImColor(255, 255, 255, 20), flNoRounding);

    RenderNavCursor(imTotalBB, imID);
    RenderFrame(imTotalBB.Min, imTotalBB.Max, imButtonColor, true, flNoRounding);

    ImVec2 imLabelTextPos = ImLerp(imTotalBB.Min + style.FramePadding, imTotalBB.Max - style.FramePadding - imLabelSize, style.ButtonTextAlign);
    imWindow->DrawList->AddText(imLabelTextPos, imLabelColor, szLabel);

    return bPressed;
}

struct ChildObj_t
{
    int                       iRounding = 0.f;

    ImColor           imBackgroundColor = ImColor(0, 0, 0, 25);
    ImColor       imTextBackgroundColor = ImColor(0, 0, 0, 15);
    ImColor     imBackgroundBorderColor = ImColor(30, 30, 30, 80);
};

bool ChlenGui::Child(const char* szLabel, ImVec2* imSize, bool bDrawLabel)
{
    static ChildObj_t obj;

    ImGuiWindow* imWindow = GetCurrentWindow();

    ImVec2 imCursorPos = imWindow->DC.CursorPos;

    ImVec2 imActualSize;

    if (!imSize)
        imActualSize = ImGui::GetContentRegionAvail();
    else
        imActualSize = *imSize;

    ImRect imRectBackground = ImRect(imCursorPos, imCursorPos + imActualSize);

    imWindow->DrawList->AddRectFilled(imRectBackground.Min, imRectBackground.Max, obj.imBackgroundColor, obj.iRounding);
    imWindow->DrawList->AddRect(imRectBackground.Min, imRectBackground.Max, obj.imBackgroundBorderColor, obj.iRounding, 0, 1.f);

    bool bResult = ImGui::BeginChild(szLabel, imActualSize, ImGuiChildFlags_AlwaysUseWindowPadding);

    if (bDrawLabel)
    {
        ImRect imRectHeader(imCursorPos, ImVec2(imRectBackground.Max.x, imCursorPos.y + 28));

        imWindow->DrawList->AddRectFilled(imRectHeader.Min, imRectHeader.Max, obj.imTextBackgroundColor, obj.iRounding, ImDrawFlags_RoundCornersTop);

        ImGui::TextColored(ImColor(255, 255, 255, 200), szLabel);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
    }

    return bResult;
}

bool ChlenGui::ChildEnd()
{
    ImGui::EndChild();

    return true;
}

bool ChlenGui::ListBox(const char* szLabel, ImVec2 imSize, int iItemsCount)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = GetID(szLabel);
    ImVec2 size = ImTrunc(CalcItemSize(imSize, CalcItemWidth(), g.Style.FramePadding.y * 2.0f));
    size.x -= 5;

    ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemAdd(frame_bb, id);

    ImDrawList* draw_list = GetWindowDrawList();
    draw_list->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(45, 45, 45, 120), 5.f);

    if (!ChlenGui::Child(szLabel, &size, false))
        return false;

    return true;
}

bool ChlenGui::ListBoxEnd()
{
    ChlenGui::ChildEnd();

    return true;
}
bool ChlenGui::Selectable(const char* szLabel, bool bSelected, ImVec2 imSize, Color colForegroundColorOverride)
{
    ImGuiWindow* imWindow = ImGui::GetCurrentWindow();
    if (imWindow->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& imStyle = g.Style;

    ImVec2 imLabelSize = CalcTextSize(szLabel, NULL, true);
    ImVec2 imAvailRegion = ImGui::GetContentRegionAvail();

    ImVec2 imItemSize(
        imSize.x > 0.0f ? imSize.x : imAvailRegion.x,
        imSize.y > 0.0f ? imSize.y : imLabelSize.y
    );

    ImVec2 imPos = imWindow->DC.CursorPos;
    ItemSize(imItemSize, 0.0f);

    ImRect imBB(imPos, imPos + imItemSize);
    if (!ItemAdd(imBB, imWindow->GetID(szLabel)))
        return false;

    bool bHovered, bHeld;
    bool bPressed = ButtonBehavior(imBB, imWindow->GetID(szLabel), &bHovered, &bHeld);

    ImDrawList* imDrawList = imWindow->DrawList;
    ImVec2 imTextPos = imBB.Min + imStyle.FramePadding;

    std::string strNewLabel(szLabel);
    size_t pos = strNewLabel.find("##");

    if (pos != std::string::npos) {
        strNewLabel = strNewLabel.substr(0, pos);
    }

    ImGuiID id = imWindow->GetID(szLabel);

    float* selectProgress = ImGui::GetStateStorage()->GetFloatRef(id, 0.0f);
    float* hoverProgress = ImGui::GetStateStorage()->GetFloatRef(id + 1, 0.0f);
    float* colorTransition = ImGui::GetStateStorage()->GetFloatRef(id + 2, 0.0f);

    *selectProgress = ImSaturate(*selectProgress + g.IO.DeltaTime * (bSelected ? 15.0f : -15.0f));
    *hoverProgress = ImSaturate(*hoverProgress + g.IO.DeltaTime * (bHovered && !bSelected ? 15.0f : -15.0f));

    ImColor normalColor = LABEL_UNHOVERED_COLOR;
    ImColor hoverColor = LABEL_HOVERED_COLOR;
    ImColor selectedColor = ImColor(g_AccentColor);

    ImColor targetColor = normalColor;
    if (bSelected)
        targetColor = selectedColor;
    else if (bHovered)
        targetColor = hoverColor;

    *colorTransition = ImLerp(*colorTransition, targetColor.Value.w, g.IO.DeltaTime * 10.0f);
    targetColor.Value.w = *colorTransition;

    float xOffset = ImLerp(0.0f, 3.3f, *selectProgress) + ImLerp(0.0f, 1.5f, *hoverProgress);
    imTextPos.x += xOffset;

    imDrawList->AddText(imTextPos, targetColor, strNewLabel.c_str());

    return bPressed;
}

bool ChlenGui::ColorPicker(const char* szLabel, ImColor* imColor, int iColorPickersOnLine)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(szLabel);

    ImGuiStorage* storage = window->DC.StateStorage;
    float* flColor = (float*)storage->GetVoidPtr(id);
    if (!flColor)
    {
        flColor = new float[4] { imColor->Value.x, imColor->Value.y, imColor->Value.z, imColor->Value.w };
        storage->SetVoidPtr(id, flColor);
    }

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const char* label_display_end = FindRenderedTextEnd(szLabel);
    float w_full = CalcItemWidth();
    g.NextItemData.ClearFlags();

    BeginGroup();
    PushID(szLabel);
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();

    const ImGuiColorEditFlags flags_untouched = 0;

    ColorEditOptionsPopup(flColor, 0);

    const bool alpha = (0 & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (0 & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;
    const float w_button = 25;
    const float h_button = 14;
    const float w_inputs = ImMax(w_full - w_button, 1.0f);
    w_full = w_inputs + w_button;

    float f[4] = { flColor[0], flColor[1], flColor[2], alpha ? flColor[3] : 1.0f };

    bool value_changed = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = ImGui::GetContentRegionAvail().x - ((w_button + 5.0f) * iColorPickersOnLine);
    window->DC.CursorPos.x = pos.x + inputs_offset_x;

    const ImVec4 col_v4(flColor[0], flColor[1], flColor[2], flColor[3]);
    if (ChlenGui::ColorButton("##ColorButton", col_v4, 0, ImVec2(w_button, h_button)))
    {
        g.ColorPickerRef = col_v4;
        OpenPopup("picker");
        SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
    }

    ImGui::SetNextWindowBgAlpha(1.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(25, 25, 25).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

    if (BeginPopup("picker", ImGuiWindowFlags_NoMove))
    {
        if (g.CurrentWindow->BeginCount == 1)
        {
            ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
            ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
            SetNextItemWidth(square_sz * 12.0f);
            value_changed |= ChlenGui::ColorPicker4("##picker", flColor, 0, &g.ColorPickerRef.x);
        }
        EndPopup();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    if (value_changed)
    {
        imColor->Value = ImVec4(flColor[0], flColor[1], flColor[2], alpha ? flColor[3] : 1.0f);
    }

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;

    PopID();
    EndGroup();

    return value_changed;
}

bool ChlenGui::ColorButton(const char* szDescId, const ImVec4& imCol, ImGuiColorEditFlags imColorEditFlags, const ImVec2& imSize)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(szDescId);
    const float default_size = GetFrameHeight();
    const ImVec2 size(imSize.x == 0.0f ? default_size : imSize.x, imSize.y == 0.0f ? default_size : imSize.y);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    if (imColorEditFlags & ImGuiColorEditFlags_NoAlpha)
        imColorEditFlags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = imCol;
    if (imColorEditFlags & ImGuiColorEditFlags_InputHSV)
        ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    float grid_step = ImMin(size.x, size.y) / 2.99f;
    float rounding = 0;
    ImRect bb_inner = bb;
    float off = 0.0f;
    if ((imColorEditFlags & ImGuiColorEditFlags_NoBorder) == 0)
    {
        off = -0.75f;
        bb_inner.Expand(off);
    }

    RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min, bb_inner.Max, GetColorU32(imCol), grid_step, ImVec2(off, off), rounding);
    window->DrawList->AddRectFilled(bb_inner.Min, bb_inner.Max, GetColorU32(imCol), rounding);

    RenderNavCursor(bb, id);

    return pressed;
}

static void ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;
    if (g.ColorEditCurrentID == 0)
        return;
    if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        return;

    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    if (*V == 0.0f)
        *S = g.ColorEditSavedSat;
}

static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
{
    ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
    ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
}

bool ChlenGui::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;
    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    const bool is_readonly = ((g.NextItemData.ItemFlags | g.CurrentItemFlags) & ImGuiItemFlags_ReadOnly) != 0;
    g.NextItemData.ClearFlags();

    PushID(label);
    const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
    if (set_current_color_edit_id)
        g.ColorEditCurrentID = window->IDStack.back();
    BeginGroup();

    if (!(flags & ImGuiColorEditFlags_NoSidePreview))
        flags |= ImGuiColorEditFlags_NoSmallPreview;

    if (!(flags & ImGuiColorEditFlags_NoOptions))
        ColorPickerOptionsPopup(col, flags);

    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));
    if (!(flags & ImGuiColorEditFlags_NoOptions))
        flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = true;
    ImVec2 picker_pos = window->DC.CursorPos;
    float square_sz = GetFrameHeight();
    float bars_width = square_sz;
    float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x));
    float bar0_pos_x = picker_pos.x + sv_picker_size + style.ItemInnerSpacing.x;
    float bar1_pos_x = bar0_pos_x + bars_width + style.ItemInnerSpacing.x;
    float bars_triangles_half_sz = IM_TRUNC(bars_width * 0.20f);

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float wheel_thickness = sv_picker_size * 0.08f;
    float wheel_r_outer = sv_picker_size * 0.50f;
    float wheel_r_inner = wheel_r_outer - wheel_thickness;
    ImVec2 wheel_center(picker_pos.x + (sv_picker_size + bars_width) * 0.5f, picker_pos.y + sv_picker_size * 0.5f);

    float triangle_r = wheel_r_inner - (int)(sv_picker_size * 0.027f);
    ImVec2 triangle_pa = ImVec2(triangle_r, 0.0f);
    ImVec2 triangle_pb = ImVec2(triangle_r * -0.5f, triangle_r * -0.866025f);
    ImVec2 triangle_pc = ImVec2(triangle_r * -0.5f, triangle_r * +0.866025f);

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {

        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        ColorEditRestoreHS(col, &H, &S, &V);

        if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
        {
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {

        InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
        if (IsItemActive() && !is_readonly)
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));

            value_changed = value_changed_sv = true;
        }
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

        SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
        InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive() && !is_readonly)
        {
            H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = value_changed_h = true;
        }
    }

    if (alpha_bar)
    {
        SetCursorScreenPos(ImVec2(bar1_pos_x, picker_pos.y));
        InvisibleButton("alpha", ImVec2(bars_width, sv_picker_size));
        if (IsItemActive())
        {
            col[3] = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            value_changed = true;
        }
    }
    PopItemFlag();

    if (!(flags & ImGuiColorEditFlags_NoLabel))
    {
        const char* label_display_end = FindRenderedTextEnd(label);
        if (label != label_display_end)
        {
            if ((flags & ImGuiColorEditFlags_NoSidePreview))
                SameLine(0, style.ItemInnerSpacing.x);
            TextEx(label, label_display_end);
        }
    }

    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedID = g.ColorEditCurrentID;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    bool value_changed_fix_hue_wrap = false;
    if ((flags & ImGuiColorEditFlags_NoInputs) == 0)
    {
        PushItemWidth((alpha_bar ? bar1_pos_x : bar0_pos_x) + bars_width - picker_pos.x);
        ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
        ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;

        if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
            value_changed |= ColorEdit4("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);
        PopItemWidth();
    }

    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            ColorEditRestoreHS(col, &H, &S, &V);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(0,255,255,style_alpha8), IM_COL32(0,0,255,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha));

    ImVec2 sv_cursor_pos;

    if (flags & ImGuiColorEditFlags_PickerHueBar)
    {

        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), col_white, hue_color32, hue_color32, col_white);
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black);
        RenderFrameBorder(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0.0f);
        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x + 2, picker_pos.x + sv_picker_size - 2);
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size), picker_pos.y + 2, picker_pos.y + sv_picker_size - 2);

        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(bar0_pos_x, picker_pos.y + i * (sv_picker_size / 6)), ImVec2(bar0_pos_x + bars_width, picker_pos.y + (i + 1) * (sv_picker_size / 6)), col_hues[i], col_hues[i], col_hues[i + 1], col_hues[i + 1]);
        float bar0_line_y = IM_ROUND(picker_pos.y + H * sv_picker_size);
        RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    float sv_cursor_rad = value_changed_sv ? wheel_thickness * 0.55f : wheel_thickness * 0.40f;
    int sv_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(sv_cursor_rad);
    draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, sv_cursor_segments);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
    draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, sv_cursor_segments);

    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        ImRect bar1_bb(bar1_pos_x, picker_pos.y, bar1_pos_x + bars_width, picker_pos.y + sv_picker_size);
        RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, bar1_bb.GetWidth() / 2.0f, ImVec2(0.0f, 0.0f));
        draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);
        RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
        RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
    }

    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0)
        value_changed = false;
    if (value_changed && g.LastItemData.ID != 0)
        MarkItemEdited(g.LastItemData.ID);

    if (set_current_color_edit_id)
        g.ColorEditCurrentID = 0;
    PopID();

    return value_changed;
}

bool ChlenGui::SimpleColorPicker(const std::string strName, Color* colColor, int iColorPickersOnLine)
{
    ImVec4 colVec4 = ImVec4(colColor->rBase(), colColor->gBase(), colColor->bBase(), colColor->aBase());
    ImColor col = ImColor(colVec4);
    if (ChlenGui::ColorPicker(strName.c_str(), &col, iColorPickersOnLine))
    {
        colColor->Set(col.Value.x, col.Value.y, col.Value.z, col.Value.w);
        return true;
    }
    return false;
}

bool ChlenGui::IsKeyDown(int user_key_index)
{
    if (user_key_index < 0)
        return false;
    ImGuiContext& g = *GImGui;
    IM_ASSERT(user_key_index >= 0 && user_key_index < IM_ARRAYSIZE(g.IO.KeysDown));
    return g.IO.KeysDown[user_key_index];
}

bool ChlenGui::IsKeyDown(ImGuiKey key)
{
    return ChlenGui::IsKeyDown(key, ImGuiKeyOwner_Any);
}

bool ChlenGui::IsKeyDown(ImGuiKey key, ImGuiID owner_id)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (!key_data->Down)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

bool ChlenGui::IsKeyPressed(ImGuiKey key, bool repeat)
{
    return IsKeyPressed(key, ImGuiKeyOwner_Any, repeat ? ImGuiInputFlags_Repeat : ImGuiInputFlags_None);
}

bool ChlenGui::IsKeyPressed(ImGuiKey key, ImGuiID owner_id, ImGuiInputFlags flags)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (!key_data->Down)
        return false;
    const float t = key_data->DownDuration;
    if (t < 0.0f)
        return false;
    IM_ASSERT((flags & ~ImGuiInputFlags_SupportedByIsKeyPressed) == 0);

    bool pressed = (t == 0.0f);
    if (!pressed && ((flags & ImGuiInputFlags_Repeat) != 0))
    {
        float repeat_delay, repeat_rate;
        GetTypematicRepeatRate(flags, &repeat_delay, &repeat_rate);
        pressed = (t > repeat_delay) && GetKeyPressedAmount(key, repeat_delay, repeat_rate) > 0;
    }
    if (!pressed)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

bool ChlenGui::IsKeyReleased(ImGuiKey key)
{
    return IsKeyReleased(key, ImGuiKeyOwner_Any);
}

bool ChlenGui::IsKeyReleased(ImGuiKey key, ImGuiID owner_id)
{
    const ImGuiKeyData* key_data = GetKeyData(key);
    if (key_data->DownDurationPrev < 0.0f || key_data->Down)
        return false;
    if (!TestKeyOwner(key, owner_id))
        return false;
    return true;
}

ImGuiKey VirtualKeyToImGuiKey( int vk )
{
    switch ( vk )
    {
    case VK_TAB: return ImGuiKey_Tab;
    case VK_LEFT: return ImGuiKey_LeftArrow;
    case VK_RIGHT: return ImGuiKey_RightArrow;
    case VK_UP: return ImGuiKey_UpArrow;
    case VK_DOWN: return ImGuiKey_DownArrow;
    case VK_PRIOR: return ImGuiKey_PageUp;
    case VK_NEXT: return ImGuiKey_PageDown;
    case VK_HOME: return ImGuiKey_Home;
    case VK_END: return ImGuiKey_End;
    case VK_INSERT: return ImGuiKey_Insert;
    case VK_DELETE: return ImGuiKey_Delete;
    case VK_BACK: return ImGuiKey_Backspace;
    case VK_SPACE: return ImGuiKey_Space;
    case VK_RETURN: return ImGuiKey_Enter;
    case VK_ESCAPE: return ImGuiKey_Escape;
    case VK_LCONTROL: return ImGuiKey_LeftCtrl;
    case VK_RCONTROL: return ImGuiKey_RightCtrl;
    case VK_LSHIFT: return ImGuiKey_LeftShift;
    case VK_RSHIFT: return ImGuiKey_RightShift;
    case VK_LMENU: return ImGuiKey_LeftAlt;
    case VK_RMENU: return ImGuiKey_RightAlt;
    case VK_CAPITAL: return ImGuiKey_CapsLock;
    case VK_SCROLL: return ImGuiKey_ScrollLock;
    case VK_NUMLOCK: return ImGuiKey_NumLock;
    case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
    case VK_PAUSE: return ImGuiKey_Pause;
    case VK_NUMPAD0: return ImGuiKey_Keypad0;
    case VK_NUMPAD1: return ImGuiKey_Keypad1;
    case VK_NUMPAD2: return ImGuiKey_Keypad2;
    case VK_NUMPAD3: return ImGuiKey_Keypad3;
    case VK_NUMPAD4: return ImGuiKey_Keypad4;
    case VK_NUMPAD5: return ImGuiKey_Keypad5;
    case VK_NUMPAD6: return ImGuiKey_Keypad6;
    case VK_NUMPAD7: return ImGuiKey_Keypad7;
    case VK_NUMPAD8: return ImGuiKey_Keypad8;
    case VK_NUMPAD9: return ImGuiKey_Keypad9;
    case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
    case VK_DIVIDE: return ImGuiKey_KeypadDivide;
    case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
    case VK_ADD: return ImGuiKey_KeypadAdd;
    case VK_F1: return ImGuiKey_F1;
    case VK_F2: return ImGuiKey_F2;
    case VK_F3: return ImGuiKey_F3;
    case VK_F4: return ImGuiKey_F4;
    case VK_F5: return ImGuiKey_F5;
    case VK_F6: return ImGuiKey_F6;
    case VK_F7: return ImGuiKey_F7;
    case VK_F8: return ImGuiKey_F8;
    case VK_F9: return ImGuiKey_F9;
    case VK_F10: return ImGuiKey_F10;
    case VK_F11: return ImGuiKey_F11;
    case VK_F12: return ImGuiKey_F12;
    case 'A': return ImGuiKey_A;
    case 'B': return ImGuiKey_B;
    case 'C': return ImGuiKey_C;
    case 'D': return ImGuiKey_D;
    case 'E': return ImGuiKey_E;
    case 'F': return ImGuiKey_F;
    case 'G': return ImGuiKey_G;
    case 'H': return ImGuiKey_H;
    case 'I': return ImGuiKey_I;
    case 'J': return ImGuiKey_J;
    case 'K': return ImGuiKey_K;
    case 'L': return ImGuiKey_L;
    case 'M': return ImGuiKey_M;
    case 'N': return ImGuiKey_N;
    case 'O': return ImGuiKey_O;
    case 'P': return ImGuiKey_P;
    case 'Q': return ImGuiKey_Q;
    case 'R': return ImGuiKey_R;
    case 'S': return ImGuiKey_S;
    case 'T': return ImGuiKey_T;
    case 'U': return ImGuiKey_U;
    case 'V': return ImGuiKey_V;
    case 'W': return ImGuiKey_W;
    case 'X': return ImGuiKey_X;
    case 'Y': return ImGuiKey_Y;
    case 'Z': return ImGuiKey_Z;
    case '0': return ImGuiKey_0;
    case '1': return ImGuiKey_1;
    case '2': return ImGuiKey_2;
    case '3': return ImGuiKey_3;
    case '4': return ImGuiKey_4;
    case '5': return ImGuiKey_5;
    case '6': return ImGuiKey_6;
    case '7': return ImGuiKey_7;
    case '8': return ImGuiKey_8;
    case '9': return ImGuiKey_9;
    case VK_OEM_1: return ImGuiKey_Semicolon;
    case VK_OEM_PLUS: return ImGuiKey_Equal;
    case VK_OEM_COMMA: return ImGuiKey_Comma;
    case VK_OEM_MINUS: return ImGuiKey_Minus;
    case VK_OEM_PERIOD: return ImGuiKey_Period;
    case VK_OEM_2: return ImGuiKey_Slash;
    case VK_OEM_3: return ImGuiKey_GraveAccent;
    case VK_OEM_4: return ImGuiKey_LeftBracket;
    case VK_OEM_5: return ImGuiKey_Backslash;
    case VK_OEM_6: return ImGuiKey_RightBracket;
    case VK_OEM_7: return ImGuiKey_Apostrophe;
    default: return ImGuiKey_None;
    }
}

bool ChlenGui::Keybind( const char* str_id, KeyBind_t* keybind )
{
    ImGuiWindow* window = ImGui::GetCurrentWindow( );
    if ( window->SkipItems )
        return false;

    ImGui::SameLine( ImGui::GetCursorPosX( ) + ImGui::GetContentRegionAvail( ).x - ImGui::CalcTextSize( KeyNames[ keybind->m_iKey ] ).x - ImGui::GetStyle( ).FramePadding.x );

    ImGuiContext& g = *GImGui;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID( str_id );
    ImGuiIO* io = &ImGui::GetIO( );

    ImVec2 label_size = ImGui::CalcTextSize( KeyNames[ keybind->m_iKey ] );
    const ImRect frame_bb( window->DC.CursorPos, window->DC.CursorPos + label_size );
    const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( window->Pos.x + window->Size.x - window->DC.CursorPos.x, label_size.y ) );

    ImGui::ItemSize( total_bb, style.FramePadding.y );
    if ( !ImGui::ItemAdd( total_bb, id, &frame_bb ) )
        return false;

    const bool hovered = ImGui::IsItemHovered( );
    const bool edit_requested = hovered && io->MouseClicked[ 0 ];
    const bool style_requested = hovered && io->MouseClicked[ 1 ];

    if ( edit_requested ) {
        if ( g.ActiveId != id ) {
            memset( io->MouseDown, 0, sizeof( io->MouseDown ) );
            memset( io->KeysDown, 0, sizeof( io->KeysDown ) );
            keybind->m_iKey = 0;
        }

        ImGui::SetActiveID( id, window );
        ImGui::FocusWindow( window );
    }
    else if ( !hovered && io->MouseClicked[ 0 ] && g.ActiveId == id )
        ImGui::ClearActiveID( );

    bool value_changed = false;
    int key = keybind->m_iKey;

    if ( g.ActiveId == id ) {
        for ( auto i = 0; i < 5; i++ ) {
            if ( ImGui::IsMouseDown( i ) ) {
                switch ( i ) {
                case 0:
                    key = VK_LBUTTON;
                    break;
                case 1:
                    key = VK_RBUTTON;
                    break;
                case 2:
                    key = VK_MBUTTON;
                    break;
                case 3:
                    key = VK_XBUTTON1;
                    break;
                case 4:
                    key = VK_XBUTTON2;
                }
                value_changed = true;
                ImGui::ClearActiveID( );
            }
        }

        if ( !value_changed ) {
            for ( auto i = VK_BACK; i <= VK_RMENU; i++ ) {
                ImGuiKey key2 = VirtualKeyToImGuiKey( i );
                if ( !ImGui::IsNamedKey( key2 ) )
                    continue;
                if ( ImGui::IsKeyDown( key2 ) ) {
                    key = i;
                    value_changed = true;
                    ImGui::ClearActiveID( );
                }
            }
        }

        if ( ImGui::IsKeyPressed( ImGuiKey_Escape ) )
        {
            keybind->m_iKey = 0;
            ImGui::ClearActiveID( );
        }
        else
            keybind->m_iKey = key;
    }

    const bool bLeftClicked = hovered && io->MouseClicked[ 1 ];

    if ( bLeftClicked && !( g.ActiveId == id ) && keybind->m_iKey != 0 )
    {
        ImGui::OpenPopup( str_id );
    }

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 10, 10 ) );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 1.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 6.0f );

    ImGui::PushStyleColor( ImGuiCol_PopupBg, ImColor( 20, 20, 20, 255 ).Value );
    ImGui::PushStyleColor( ImGuiCol_Border, ImColor( 35, 35, 35, 120 ).Value );

    if ( ImGui::BeginPopup( str_id ) )
    {

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        if ( ChlenGui::Button( X( "Hold" ), ImVec2( 80, 20 ) ) )
            keybind->m_iMode = 0;

        if ( ChlenGui::Button( X( "Toggle" ), ImVec2( 80, 20 ) ) )
            keybind->m_iMode = 1;

        if ( ChlenGui::Button( X( "Always on" ), ImVec2( 80, 20 ) ) )
            keybind->m_iMode = 2;

        ImGui::PopStyleVar();

        ImGui::EndPopup( );
    }

    ImGui::PopStyleColor( 2 );
    ImGui::PopStyleVar( 3 );

    char buf_display[ 64 ] = "[-]";
    std::string active = std::vformat( X( "[{}]" ), std::make_format_args( KeyNames[ keybind->m_iKey ] ) );

    if ( keybind->m_iKey != 0 && g.ActiveId != id )
        strcpy_s( buf_display, active.c_str( ) );
    else if ( g.ActiveId == id )
        strcpy_s( buf_display, X( "[-]" ) );

    window->DrawList->AddText( frame_bb.Min, g.ActiveId == id ? ImColor( 82, 120, 176, 255 ) : ImColor( 255, 255, 255, 50 ), buf_display );

    return value_changed;
}

bool ChlenGui::CheckBoxBind(const char* szLabel, bool* bBool, int* iKey, int* iKeyStyle, int iCustomID)
{
    bool result = ChlenGui::CheckBox(szLabel, bBool, iCustomID);

    ImGui::SameLine(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(KeyNames[*iKey]).x - ImGui::GetStyle().FramePadding.x);

    KeyBind_t keybind;
    keybind.m_iKey = *iKey;
    keybind.m_iMode = *iKeyStyle;

    if (ChlenGui::Keybind((std::string("##") + szLabel + "_bind").c_str(), &keybind))
    {
        *iKey = keybind.m_iKey;
        *iKeyStyle = keybind.m_iMode;
    }

    return result;
}

bool ChlenGui::CheckBoxStartPopup(const char* szLabel, bool* bBool, ImVec2 size, int iCustomID)
{
    bool result = ChlenGui::CheckBox(szLabel, bBool, iCustomID);

    if (result && *bBool)
    {
        ImGui::OpenPopup((std::string("##") + szLabel + "_popup").c_str());
    }

    if (ImGui::BeginPopup((std::string("##") + szLabel + "_popup").c_str(), ImGuiWindowFlags_NoMove))
    {
        ImGui::SetWindowSize(size);
        return true;
    }

    return false;
}

void ChlenGui::CheckBoxEndPopup()
{
    ImGui::EndPopup();
}

bool ChlenGui::PopupBegin(ImVec2 size)
{
    static int popup_counter = 0;
    std::string popup_id = "##popup_" + std::to_string(popup_counter++);

    if (ImGui::BeginPopup(popup_id.c_str(), ImGuiWindowFlags_NoMove))
    {
        ImGui::SetWindowSize(size);
        return true;
    }

    return false;
}

void ChlenGui::PopupEnd()
{
    ImGui::EndPopup();
}

bool ChlenGui::LabelStartPopup(const char* szLabel, ImVec2 size)
{
    ImGui::TextUnformatted(szLabel);

    if (ImGui::IsItemClicked())
    {
        ImGui::OpenPopup((std::string("##") + szLabel + "_popup").c_str());
    }

    if (ImGui::BeginPopup((std::string("##") + szLabel + "_popup").c_str(), ImGuiWindowFlags_NoMove))
    {
        ImGui::SetWindowSize(size);
        return true;
    }

    return false;
}

void ChlenGui::LabelEndPopup()
{
    ImGui::EndPopup();
}

void ChlenGui::CustomSeparator(float& thickness, ImVec4& color, float padding)
{

    ImGui::Dummy(ImVec2(0.0f, padding));

    ImVec2 p = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(ImVec2(p.x - 5, p.y), ImVec2(p.x + width + 100, p.y + thickness), ImGui::GetColorU32(g_AccentColor));

    ImGui::Dummy(ImVec2(width, thickness));

    ImGui::Dummy(ImVec2(0.0f, padding));
}

void ChlenGui::CheckboxColor(const char* label, ImVec4* color)
{

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 20);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 17);

    ImGuiStyle& style = ImGui::GetStyle();
    float old_rounding = style.FrameRounding;
    style.FrameRounding = 0.0f;

    ImVec2 size = ImVec2(20, 20);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImColor(*color)
    );

    draw->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(30, 30, 30, 255)
    );

    std::string button_id = std::string("##color_picker_") + label;
    std::string popup_id = std::string("##color_picker_popup_") + label;

    if (ImGui::InvisibleButton(button_id.c_str(), size))
    {
        ImGui::OpenPopup(popup_id.c_str());
    }

    if (ImGui::BeginPopup(popup_id.c_str()))
    {
        ImGui::ColorPicker4("##picker", (float*)color);
        ImGui::EndPopup();
    }

    style.FrameRounding = old_rounding;
}

void ChlenGui::RenderMenuTabs(const char* tabs[], int tabs_count, int* selected_tab)
{
    for (int index = 0; index < tabs_count; ++index) {
        if (index == *selected_tab)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

        ImGui::Text(tabs[index]);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            *selected_tab = index;

        if (index != tabs_count - 1)
            ImGui::SameLine();
    }
}

void ChlenGui::RenderSubTabs(const char* sub_tabs[], int sub_tabs_count, int* selected_sub_tab)
{
    for (int index = 0; index < sub_tabs_count; ++index) {
        if (index == *selected_sub_tab)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 100, 100, 255));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

        ImGui::Text(sub_tabs[index]);
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            *selected_sub_tab = index;

        if (index != sub_tabs_count - 1)
            ImGui::SameLine();
    }
}

void ChlenGui::RenderExampleMenu()
{
    const char* items[] = { "RAPIRA", "TOHA", "REDDER" };
    const char* subs[] = { "RAGE", "LEGIT", "VISUALS", "INVENTORY", "CONFIGS" };
    const char* subtub[] = { "AIMBOT", "ANTI-AIM" };
    static int subs_selected = 0;
    static int subtub_selected = 0;
    static float separator_thickness = 1.5f;
    static ImVec4 separator_color = ImVec4(0.32f, 0.47f, 0.69f, 1.0f);

    static bool show_demo_window = false;
    static bool test = false;
    static int rapira = 1;
    static float rapira1 = 1.f;
    static ImVec4 accent_color = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);
    static bool multi[] = { false, true, false };
    static int cur = 1;

    bool opened = true;
    ImGui::SetNextWindowSize(ImVec2(680, 480));
    ImGui::Begin("Example Menu", &opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ChlenGui::RenderMenuTabs(subs, IM_ARRAYSIZE(subs), &subs_selected);

    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);

    if (subs_selected == 0) {
        ChlenGui::RenderSubTabs(subtub, IM_ARRAYSIZE(subtub), &subtub_selected);

        if (subtub_selected == 0) {
            ImVec2 child_size = ImVec2(330, 390);
            ImGui::BeginChild("my_child_window", child_size, true);
            ImGui::Text("Content");
            ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);

            ChlenGui::CheckBox("checkbox off", &show_demo_window);
            ChlenGui::CheckboxColor("2", &accent_color);
            ChlenGui::CheckboxColor("3", &accent_color);
            ChlenGui::CheckBox("checkbox on", &test);
            ChlenGui::CheckboxColor("1", &accent_color);
            ChlenGui::SliderInt("Rapira", &rapira, 1, 100);
            ChlenGui::SliderFloat("PROCENT SOSNIA", &rapira1, 1.f, 100.f);

            ChlenGui::Combo("Combo", &cur, items, 3);

            std::vector<bool> multi_vec;
            for (int i = 0; i < 3; i++)
                multi_vec.push_back(multi[i]);
            ChlenGui::MultiComboBox("MultiCombo", items, multi_vec, 3);
            for (int i = 0; i < 3; i++)
                multi[i] = multi_vec[i];

            ImGui::EndChild();
            ImGui::SameLine();

            ImVec2 child_size1 = ImVec2(324, 193);
            ImGui::BeginChild("zzzz", child_size1, true);
            ImGui::EndChild();

            ImGui::SetCursorPos(ImVec2(346, 251));
            ImGui::BeginChild("sd", child_size1, true);
            ImGui::EndChild();
        }
    }

    ImGui::SetCursorPos(ImVec2(10, 446));
    ChlenGui::CustomSeparator(separator_thickness, separator_color, 0.5f);
    ImGui::Text("Example Menu");

    ImGui::End();
}

