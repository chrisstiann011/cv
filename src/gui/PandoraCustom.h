#pragma once

#include "../Precompiled.h"

#define  IMGUI_DEFINE_MATH_OPERATORS

using namespace ImGui;
using namespace std;

#define to_vec4(r, g, b, a) ImColor(r / 255.f, g / 255.f, b / 255.f, a)

class c_custom
{
public:
    float m_anim = 0.f;

    int m_tab = 0;
    vector<const char*> tabs       = { "Rage", "Visuals", "Misc", "Profile", "Scripts" };
    vector<const char*> tabs_icons = { "B", "C", "D", "E", "F" };

    int m_rage_subtab    = 0;
    vector<const char*> rage_subtabs = { "Aimbot", "Anti-aim", "Other" };

    int m_visuals_subtab = 0;
    vector<const char*> visuals_subtabs = { "Players", "World" };

    float col_buf[4] = { 1.f, 1.f, 1.f, 1.f };

    void render_arrows_for_horizontal_bar(ImVec2 pos, float alpha, float width, float height);

    bool expand_button(const char* label, bool selected, float rounding = 0.f, ImDrawFlags flags = 0);
    bool tab(const char* icon, const char* label, bool selected);
    bool sub_tab(const char* label, bool selected);

    void tab_area(const char* str_id, ImVec2 size, std::function<void()> content);

    void begin_child(const char* name, ImVec2 size);
    void end_child();

    int accent_color[4] = { 140, 131, 214, 255 };
    ImColor get_accent_color(float a = 1.f)
    {
        return to_vec4(accent_color[0], accent_color[1], accent_color[2], a);
    }
};

inline void c_custom::render_arrows_for_horizontal_bar(ImVec2 pos, float alpha, float width, float height)
{
    auto draw = GetWindowDrawList();
    draw->AddRect(pos, pos + ImVec2(width, height), ImColor(0.f, 0.f, 0.f, alpha * GetStyle().Alpha), 4);
}

inline bool c_custom::expand_button(const char* label, bool selected, float rounding, ImDrawFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiID id          = window->GetID(label);

    ImVec2 label_size = CalcTextSize(label, nullptr, true);
    ImVec2 icon_size  = GetIO().Fonts->Fonts[1]->CalcTextSizeA(GetIO().Fonts->Fonts[1]->FontSize - 2, FLT_MAX, 0, "A");
    ImVec2 pos        = window->DC.CursorPos;

    auto   draw = window->DrawList;
    ImRect rect(pos, pos + ImVec2(GetWindowWidth(), 45));
    ItemAdd(rect, id);
    ItemSize(rect, GetStyle().FramePadding.y);

    bool hovered{}, held{};
    bool pressed = ButtonBehavior(rect, id, &hovered, &held, ImGuiButtonFlags_None);

    static std::unordered_map<ImGuiID, float> values;
    auto                                      it = values.find(id);
    if (it == values.end())
    {
        values.insert({ id, 0.f });
        it = values.find(id);
    }

    it->second = ImLerp(it->second, (selected ? 1.f : 0.f), 0.035f);

    draw->AddRectFilled(rect.Min, rect.Max, ImColor(1.f, 1.f, 1.f, 0.025f * it->second), rounding, flags);
    draw->AddText(GetIO().Fonts->Fonts[1], GetIO().Fonts->Fonts[1]->FontSize - 2,
                  ImVec2(rect.Min.x + 50 / 2 - icon_size.x / 2, rect.GetCenter().y - icon_size.y / 2),
                  GetColorU32(ImGuiCol_Text, (it->second > 0.3f ? it->second : 0.3f)), "A");
    draw->AddText(ImVec2(rect.Min.x + 50, rect.GetCenter().y - label_size.y / 2),
                  GetColorU32(ImGuiCol_Text, (it->second > 0.3f ? it->second : 0.3f)), label);

    return pressed;
}

inline bool c_custom::tab(const char* icon, const char* label, bool selected)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiID      id     = window->GetID(label);

    ImVec2 label_size = CalcTextSize(label, nullptr, true);
    ImVec2 icon_size  = GetIO().Fonts->Fonts[1]->CalcTextSizeA(GetIO().Fonts->Fonts[1]->FontSize, FLT_MAX, 0, icon);
    ImVec2 pos        = window->DC.CursorPos;

    auto   draw = window->DrawList;
    ImRect rect(pos, pos + ImVec2(GetWindowWidth(), 40));
    ItemAdd(rect, id);
    ItemSize(rect, GetStyle().FramePadding.y);

    bool hovered{}, held{};
    bool pressed = ButtonBehavior(rect, id, &hovered, &held, ImGuiButtonFlags_None);

    static std::unordered_map<ImGuiID, float> values;
    auto                                      it = values.find(id);
    if (it == values.end())
    {
        values.insert({ id, 0.f });
        it = values.find(id);
    }

    it->second = ImLerp(it->second, (selected || hovered ? 1.f : 0.f), 0.035f);

    draw->AddText(GetIO().Fonts->Fonts[1], GetIO().Fonts->Fonts[1]->FontSize,
                  ImVec2(rect.Min.x + 50 / 2 - icon_size.x / 2, rect.GetCenter().y - icon_size.y / 2),
                  GetColorU32(ImGuiCol_Text, (it->second > 0.3f ? it->second : 0.3f)), icon);
    draw->AddText(ImVec2(rect.Min.x + 50, rect.GetCenter().y - label_size.y / 2),
                  GetColorU32(ImGuiCol_Text, (it->second > 0.3f ? it->second : 0.3f)), label);

    return pressed;
}

inline bool c_custom::sub_tab(const char* label, bool selected)
{
    ImGuiWindow* window = GetCurrentWindow();
    ImGuiID      id     = window->GetID(label);

    ImVec2 label_size = CalcTextSize(label, nullptr, true);
    ImVec2 pos        = window->DC.CursorPos;

    auto   draw = window->DrawList;
    ImRect rect(pos, pos + label_size);
    ItemAdd(rect, id);
    ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x + 15, rect.Max.y), GetStyle().FramePadding.y);

    bool hovered{}, held{};
    bool pressed = ButtonBehavior(rect, id, &hovered, &held, ImGuiButtonFlags_None);

    static std::unordered_map<ImGuiID, float> values;
    auto                                      it = values.find(id);
    if (it == values.end())
    {
        values.insert({ id, 0.f });
        it = values.find(id);
    }

    it->second = ImLerp(it->second, (selected ? 1.f : 0.f), 0.035f);

    draw->AddText(rect.Min, GetColorU32(ImGuiCol_Text, (it->second > 0.3f ? it->second : 0.3f)), label);
    draw->AddCircleFilled(ImVec2(rect.GetCenter().x, rect.Max.y + 10),
                          3 * it->second, get_accent_color(it->second * GetStyle().Alpha));

    return pressed;
}

struct tab_area_structure
{
    bool  hovered{};
    bool  should_expand{};
    float width{};
};

inline void c_custom::tab_area(const char* str_id, ImVec2 size, std::function<void()> content)
{
    static std::unordered_map<std::string, tab_area_structure> values;
    auto                                                        it = values.find(std::string(str_id));
    if (it == values.end())
    {
        values.insert({ std::string(str_id), { false, false, size.x } });
        it = values.find(std::string(str_id));
    }

    it->second.width = ImLerp(it->second.width,
                              (it->second.should_expand && it->second.hovered) ? size.x + 60.f : size.x,
                              0.08f);

    if (it->second.should_expand && it->second.hovered)
        GetForegroundDrawList()->AddRectFilled(GetWindowPos() + ImVec2(it->second.width, 0),
                                               GetWindowPos() + GetWindowSize(),
                                               ImColor(0.f, 0.f, 0.f, 0.2f),
                                               GetStyle().WindowRounding);

    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    BeginChild((std::string(str_id) + ".child").c_str(), ImVec2(it->second.width, size.y));

    it->second.hovered = IsMouseHoveringRect(GetCurrentWindow()->Pos,
                                             GetCurrentWindow()->Pos + ImVec2(it->second.width, size.y));

    GetWindowDrawList()->AddRectFilled(GetCurrentWindow()->Pos,
                                       GetCurrentWindow()->Pos + GetCurrentWindow()->Size,
                                       ImColor(28, 30, 36),
                                       GetStyle().WindowRounding,
                                       ImDrawFlags_RoundCornersTopLeft);

    if (expand_button("Expand", it->second.should_expand, GetStyle().WindowRounding,
                      ImDrawFlags_RoundCornersTopLeft))
        it->second.should_expand = !it->second.should_expand;

    content();

    EndChild();

    PopStyleVar();
}

inline void c_custom::begin_child(const char* name, ImVec2 size)
{
    ImGuiWindow* window = GetCurrentWindow();

    ImVec2 pos = window->DC.CursorPos;

    BeginChild((std::string(name) + ".main").c_str(), size, false, ImGuiWindowFlags_NoScrollbar);

    GetWindowDrawList()->AddRectFilled(pos, pos + size, to_vec4(28, 30, 36, m_anim), 4);
    GetWindowDrawList()->AddText(pos + ImVec2(10, 8), get_accent_color(m_anim), name,
                                 FindRenderedTextEnd(name));

    SetCursorPosY(30);
    BeginChild(name, ImVec2(size.x, size.y - 30));
    SetCursorPosX(10);

    BeginGroup();

    PushStyleVar(ImGuiStyleVar_Alpha, m_anim);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
}

inline void c_custom::end_child()
{
    PopStyleVar(2);
    EndGroup();
    EndChild();
    EndChild();
}

inline c_custom custom;

