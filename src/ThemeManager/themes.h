#pragma once

enum class ThemeType {
    Dark,
    Light,
    Custom,
    Default = Dark
};

class ThemeManager {
    public:
        static void ApplyTheme(ThemeType theme);
        static void LoadDefaultTheme();
        static ThemeType GetCurrentTheme() { return currentTheme; }
        
    private:
        static ThemeType currentTheme;
        
        static void SetDarkTheme();
        static void SetLightTheme(); 
        static void SetCustomTheme();
};