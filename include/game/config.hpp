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

// The world is a flat square arena centred on the origin, in metres.
struct WorldSettings final {
    float arenaHalfSize;
    // Downward acceleration applied to every entity, in m/s².
    float gravity;
};

struct GameConfig final {
    WindowSettings window;
    WorldSettings  world;
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
    .world = {
        .arenaHalfSize = 150.0f,
        .gravity = 9.80665f
    }
};

} // namespace game
