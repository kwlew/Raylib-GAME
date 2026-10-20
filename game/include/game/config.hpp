#pragma once

#include <raylib.h>

namespace game {
    
    struct WindowSettings final {
        const char* title     = "My Game";
        int         width     = 800;
        int         height    = 600;
        int         targetFPS = 300;
        bool        resizable = true;
        bool        vsync     = false;
    };

    struct LogSettings final {
        int appLevel    = LOG_INFO;
        int raylibLevel = LOG_WARNING;
    };

    struct GameConfig final {
        WindowSettings  window;
        LogSettings     log;

        float maxFrameTime = 0.1f;
    };

}