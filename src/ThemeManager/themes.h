#pragma once

#include "imgui.h"

enum class ThemeType {
    Dark,
    Light,
    Custom,
    Default = Dark
};

class ThemeManager {
    public:
        static void ApplyTheme(ThemeType theme);
        static ThemeType GetCurrentTheme() { return currentTheme; }
        
    private:
        static ThemeType currentTheme;
};
