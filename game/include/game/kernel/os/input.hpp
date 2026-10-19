#pragma once

#include <raylib.h>

namespace game::kernel::os {

struct InputState {
    // Wanted movement: x = right, y = forward. Length is at most 1.
    Vector2 move{};
    // Mouse movement this frame, in pixels.
    Vector2 look{};
    // True only on the frame jump is pressed.
    bool jump{};
    // True while jump is held.
    bool jumpHeld{};
    // True while walk is held (slower, CS-style).
    bool walk{};
};

[[nodiscard]] InputState pollInput() noexcept;

} // namespace game::kernel::os
