#pragma once
#include "raylib.h"
#include <stdbool.h>

struct WindowState {
    bool fullscreen, minimized, maximized, focused, resized;
    int width, height;
    Vector2 position, dpiScale;

    static WindowState Pool() {
        return {
            IsWindowFullscreen(), IsWindowMinimized(), IsWindowMaximized(),
            IsWindowFocused(), IsWindowResized(),
            GetScreenWidth(), GetScreenHeight(),
            GetWindowPosition(), GetWindowScaleDPI()
        };
    }
};