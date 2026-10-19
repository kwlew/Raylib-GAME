#pragma once

#include <raylib.h>

namespace game {

struct WindowSettings final {
    const char* title     = "My Game";
    int         width     = 800;
    int         height    = 600;
    int         targetFPS = 300;   // 0 = uncapped
    bool        resizable = true;
    bool        vsync     = false;
};

struct DebugSettings final {
    bool        visible              = true;     // overlay shown at startup
    int         toggleKey            = KEY_F3;   // shows/hides the overlay
    bool        showFPS              = true;
    bool        showMemory           = true;
    bool        logFPS               = false;
    float       memorySampleInterval = 0.5f;
    const char* fontPath             = "assets/fonts/VT323/VT323-Regular.ttf"; // relative to the executable
    int         fontSize             = 24;
};

struct LogSettings final {
    int appLevel    = LOG_INFO;
    int raylibLevel = LOG_WARNING;
};

struct GameConfig final {
    WindowSettings window;
    DebugSettings  debug;
    LogSettings    log;
    float          maxFrameTime = 0.1f;
};

} // namespace game
