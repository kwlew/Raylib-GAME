#pragma once

#include <raylib.h>

namespace game::kernel {

// What the player is asking for this frame, independent of which keys or
// device produced it. Gameplay code reads this instead of calling IsKeyDown().
struct InputState {
    // x = right, y = forward, relative to where the player faces. Length 0 or 1.
    Vector2 move{};
    // How far to turn the view this frame, in mouse pixels; +x right, +y down.
    Vector2 look{};
    // Player wants to jump.
    bool jump{};
    // Player is holding the jump button down.
    bool jumpHeld{};
    // Whether the player is sprinting.
    bool sprint{};
};

// Reads the keyboard and mouse once; call at the start of each update.
[[nodiscard]] InputState pollInput() noexcept;

} // namespace game::kernel
