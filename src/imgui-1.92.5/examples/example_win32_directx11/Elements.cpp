#include "Elements.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <unordered_map>
#include <string>

using namespace ImGui;

ImFont* g_font_segoe = nullptr;

void LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    g_font_segoe = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\Verdana.ttf",
        16.0f,
        nullptr,
        io.Fonts->GetGlyphRangesCyrillic()
    );

    IM_ASSERT(g_font_segoe);
}


bool c_elements::checkbox_t::base(const char* label, bool* v)
{
    ImGui::PushID(label);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float size = ImGui::GetFrameHeight() * 0.75f;
    ImVec2 pos = ImGui::GetCursorScreenPos();

    float text_width = ImGui::CalcTextSize(label).x;
    float total_width = size + ImGui::GetStyle().ItemInnerSpacing.x + text_width;

    ImGui::InvisibleButton(label, ImVec2(total_width, size));

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    if (clicked)
        *v = !(*v);
    ImU32 empty = ImGui::GetColorU32(IM_COL32(35, 35, 35, 255));
    ImU32 fill = ImGui::GetColorU32(ImVec4(0.32f, 0.47f, 0.69f, 1.0f));
    ImU32 border = ImGui::GetColorU32(hovered ? ImVec4(0.2f, 0.2f, 0.2f, 1.0f)
        : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

    if (*v)
        draw_list->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), fill);
    else
        draw_list->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), empty);
    draw_list->AddRect(pos, ImVec2(pos.x + size, pos.y + size), border, 0.0f, 0, 1.5f);

    ImGui::SetCursorScreenPos(ImVec2(
        pos.x + size + ImGui::GetStyle().ItemInnerSpacing.x,
        pos.y
    ));

    ImGui::PushFont(g_font_segoe);
    ImGui::TextUnformatted(label);
    ImGui::PopFont();

    ImGui::SetCursorScreenPos(ImVec2(
        pos.x,
        pos.y + size + ImGui::GetStyle().ItemSpacing.y
    ));

    ImGui::PopID();
    return clicked;
}


template<typename T>
void c_elements::slider_t::base(const char* label, T* value, T min, T max, T step)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    IM_ASSERT(value && min < max);

    ImGui::PushID(label);


    ImGui::PushFont(g_font_segoe);
    ImGui::TextUnformatted(label);
    ImGui::PopFont();

    ImGui::SameLine();

    char value_buf[32];
    if constexpr (std::is_floating_point<T>::value)
        snprintf(value_buf, sizeof(value_buf), "%.1f", *value);
    else
        snprintf(value_buf, sizeof(value_buf), "%d", (int)*value);

    ImGui::PushFont(g_font_segoe);
    float value_width = ImGui::CalcTextSize(value_buf).x;
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - value_width);
    ImGui::TextUnformatted(value_buf);
    ImGui::PopFont();

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
    ImGuiID id = window->GetID("##slider");

    float target = float(*value - min) / float(max - min);
    float& t = anim[id];
    t = ImLerp(t, target, ImClamp(99999.0f * g.IO.DeltaTime, 0.0f, 1.0f));

    float fill_width = full_width * t;
    if (fill_width > 1.0f)
    {
        window->DrawList->AddRectFilled(
            bar_pos,
            ImVec2(bar_pos.x + fill_width, bar_end.y),
            IM_COL32(79, 117, 172, 255), 0.0f
        );
    }

    ImGui::SetCursorScreenPos(bar_pos);
    ImGui::InvisibleButton("##slider", ImVec2(full_width, bar_height));

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

void c_elements::slider_t::int_value(const char* label, int* value, int min, int max, int step)
{
    base(label, value, min, max, step);
}

void c_elements::slider_t::float_value(const char* label, float* value, float min, float max, float step)
{
    base(label, value, min, max, step);
}
bool c_elements::combo_t::base(
    const char* label,
    int* current_item,
    const char* const items[],
    int count,
    int height_in_items)
{
    if (!current_item || !items || count <= 0)
        return false;

    bool value_changed = false;

    ImGui::TextUnformatted(label);

    const char* preview = (*current_item >= 0 && *current_item < count) ? items[*current_item] : "None";

    float combo_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(combo_width);
    ImVec4 gray(0.2f, 0.2f, 0.2f, 1.0f);

    // Цвета комбобокса и элементов
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // толщина рамки
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, gray); // цвет обводки
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(35, 35, 35, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(50, 50, 50, 255));

    std::string label_id = std::string("##") + label;

    // Используем BeginCombo без стрелки
    if (ImGui::BeginCombo(label_id.c_str(), preview, ImGuiComboFlags_NoArrowButton))
    {
        for (int i = 0; i < count; i++)
        {
            bool selected = (*current_item == i);
            if (ImGui::Selectable(items[i], selected))
            {
                *current_item = i;
                value_changed = true;
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar();

    return value_changed;
}



void c_elements::combo_t::multi_selection(
    const char* label,
    bool combos[],
    const char* items[],
    int count)
{
    if (!combos || !items || count <= 0)
        return;

    ImGui::TextUnformatted(label);

    std::string preview;
    for (int i = 0; i < count; i++)
    {
        if (combos[i])
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

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // толщина рамки
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_Border, gray); // цвет обводки
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(35, 35, 35, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(50, 50, 50, 255));

    std::string label_id = std::string("##") + label;

    // Используем BeginCombo без стрелки
    if (ImGui::BeginCombo(label_id.c_str(), preview.c_str(), ImGuiComboFlags_NoArrowButton))
    {
        for (int i = 0; i < count; i++)
        {
            ImGui::Selectable(items[i], &combos[i], ImGuiSelectableFlags_DontClosePopups);
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar();
}
void c_elements::misc::group_box(const char* name, ImVec2 size_arg) {
    auto window = ImGui::GetCurrentWindow();
    auto pos = window->DC.CursorPos;

    // Цвета
    ImU32 bg_color = IM_COL32(30, 30, 30, 255);
    ImU32 border_color = IM_COL32(255, 255, 255, 255);
    ImU32 accent_color = IM_COL32(0, 255, 0, 255);
    ImU32 text_color = IM_COL32(255, 255, 255, 255);

    // Промежуточные строки для ImGui ID
    std::string outer_name = std::string(name) + "_outer";
    std::string inner_name = std::string(name) + "_inner";

    // --- Внешнее окно ---
    ImGui::BeginChild(outer_name.c_str(), size_arg, false, ImGuiWindowFlags_NoScrollbar);

    ImVec2 rect_end = ImVec2(pos.x + size_arg.x, pos.y + size_arg.y);
    ImGui::GetWindowDrawList()->AddRectFilled(pos, rect_end, bg_color, 0.0f);
    ImGui::GetWindowDrawList()->AddRect(pos, rect_end, border_color, 0.0f);

    // Линия слева и заголовок сверху слева
    if (!strchr(name, '#')) {
        ImVec2 line_start = ImVec2(pos.x + 4, pos.y + 3);
        ImVec2 line_end = ImVec2(pos.x + 4, pos.y + 15);
        ImGui::GetWindowDrawList()->AddLine(line_start, line_end, accent_color, 1.0f);

        ImVec2 text_pos = ImVec2(pos.x + 8, pos.y + 2);
        ImGui::GetWindowDrawList()->AddText(text_pos, text_color, name);
    }

    // --- Внутреннее child окно ---
    ImGui::SetCursorPos(ImVec2(12, 19)); // отступ чтобы не перекрывать текст
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 10));
    ImGui::BeginChild(inner_name.c_str(), ImVec2(size_arg.x - 24, size_arg.y - 21), false, ImGuiWindowFlags_NoScrollbar);

    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

    // --- Здесь добавляешь элементы: кнопки, чекбоксы, слайдеры ---
    // Пример:
    // g_elements->checkbox.base("Checkbox", &some_bool);
    // g_elements->slider.int_value("Slider", &some_int, 0, 100, 1);

    ImGui::PopStyleVar(2);
    ImGui::EndGroup();
    ImGui::EndChild(); // inner
    ImGui::PopStyleVar(); // WindowPadding
    ImGui::EndChild();   // outer
}
void c_elements::misc::CustomSeparator(float& thickness, ImVec4& color, float padding)
{
    // небольшой отступ сверху
    ImGui::Dummy(ImVec2(0.0f, padding));

    // позиция линии
    ImVec2 p = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;

    // рисуем линию
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(ImVec2(p.x-5,p.y), ImVec2(p.x + width+5, p.y + thickness), ImColor(color));

    // чтобы ImGui учитывал размер линии
    ImGui::Dummy(ImVec2(width, thickness));

    // отступ снизу
    ImGui::Dummy(ImVec2(0.0f, padding));
}
void c_elements::checkbox_t::color(const char* label, ImVec4* color)
{
    // Сначала рисуется базовый элемент строки (ВАЖНО)

    // Смещение: влево и на строку выше (как в labels_t)
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 28);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 17);

    ImGuiStyle& style = ImGui::GetStyle();
    float old_rounding = style.FrameRounding;
    style.FrameRounding = 0.0f;

    ImVec2 size = ImVec2(26, 13);
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    // Цвет
    draw->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImColor(*color)
    );

    // Рамка
    draw->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(0.2f, 0.2f, 0.2f, 1.0f)
    );

    // Клик
    if (ImGui::InvisibleButton(
        ("##color_picker_" + std::string(label)).c_str(),
        size))
    {
        ImGui::OpenPopup(
            ("##color_picker_popup_" + std::string(label)).c_str());
    }

    if (ImGui::BeginPopup(
        ("##color_picker_popup_" + std::string(label)).c_str()))
    {
        ImGui::ColorPicker4("##picker", (float*)color);
        ImGui::EndPopup();
    }

    style.FrameRounding = old_rounding;
}







