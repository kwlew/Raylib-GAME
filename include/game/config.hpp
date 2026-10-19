#pragma once

namespace game {

struct WindowSettings final {
    const char* name;
    int         width;
    int         height;
    int         targetFPS;
    bool        showFPS;
    bool        showMemory;
    bool        showVelocity;
    bool        resizable;
};

struct GameConfig final {
    WindowSettings window;
};

inline constexpr GameConfig defaultConfig {
    .window = {
        .name = "My Game",
        .width = 800,
        .height = 600,
        .targetFPS = 144,
        .showFPS = true,
        .showMemory = true,
        .showVelocity = true,
        .resizable = true
    },
};

} // namespace game
