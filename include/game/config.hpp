#pragma once

namespace game {

    struct WindowSettings final {
        int width;
        int height;
        int targetFPS;
        bool showFPS;
        bool showMemory;
        bool resizable;
    };

    struct GameConfig final {
        WindowSettings window;
    };

    inline constexpr GameConfig defaultConfig {
        .window = {
            .width = 800,
            .height = 600,
            .targetFPS = 144,
            .showFPS = true,
            .showMemory = true,
            .resizable = true
        }
    };
}