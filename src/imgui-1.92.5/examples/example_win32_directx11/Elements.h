#pragma once
#include <imgui.h>

class c_elements
{
public:
    struct checkbox_t
    {
        bool base(const char* label, bool* v);
        void color(const char* label, ImVec4* color);
        bool bind(const char* label, bool* v, int* key, int* key_style);
        bool start_popup(const char* label, bool* v, ImVec2 size = ImVec2(200, 200));
        void end_popup();
    } checkbox;

    struct separator_t
    {
        void base(float thickness = 1.0f, float padding = 0.0f);
    } separator;

    struct popup_t
    {
        bool begin(ImVec2 size = ImVec2(200, 200));
        void end();
    } popup;

    struct combo_t
    {
        bool base(const char* label, int* current_item, const char* const items[], int count, int height_in_items = -1);
        void multi_selection(const char* label, bool combos[], const char* items[], int items_count);
    } combo;

    struct slider_t
    {
        template<typename T>
        void base(const char* label, T* v, T v_min, T v_max, T step = T(1));
        void int_value(const char* label, int* value, int min, int max, int step = 1);
        void float_value(const char* label, float* value, float min, float max, float step = 0.1f);
    } slider;

    struct labels_t
    {
        void base(const char* label);
        bool start_popup(const char* label, ImVec2 size = ImVec2(200, 200));
        void end_popup();
    } label;
    struct misc
    {
        void CustomSeparator(float& thickness, ImVec4& color, float padding);
        void group_box(const char* name, ImVec2 size_arg);
    } misc;
};

// ќбъ€вление глобального объекта
extern c_elements* g_elements;
