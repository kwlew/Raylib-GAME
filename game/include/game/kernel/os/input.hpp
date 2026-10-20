#pragma once

#include <raylib.h>

namespace game::kernel::os {

    struct InputState {
        Vector2 move{};
        Vector2 look{};
    };

    [[nodiscard]] InputState pollInput() noexcept;

}