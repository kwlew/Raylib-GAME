#pragma once

#include <raylib.h>

namespace game {

struct WindowState {
    bool fullscreen, minimized, maximized, focused, resized;
    int width, height;
    Vector2 position, dpiScale;

    static WindowState Poll() {
        return {
            .fullscreen = IsWindowFullscreen(),
            .minimized  = IsWindowMinimized(),
            .maximized  = IsWindowMaximized(),
            .focused    = IsWindowFocused(),
            .resized    = IsWindowResized(),
            .width      = GetScreenWidth(),
            .height     = GetScreenHeight(),
            .position   = GetWindowPosition(),
            .dpiScale   = GetWindowScaleDPI(),
        };
    }
};

}