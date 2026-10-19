#include "game/kernel/os/input.hpp"

#include <cmath>

namespace game::kernel::os {

namespace {

[[nodiscard]] bool eitherDown(KeyboardKey primary, KeyboardKey secondary) noexcept {
    return IsKeyDown(primary) || IsKeyDown(secondary);
}

// 1 if only the positive side is down, -1 if only the negative side is, 0 if neither or both.
[[nodiscard]] float axis(bool positive, bool negative) noexcept {
    return static_cast<float>(positive) - static_cast<float>(negative);
}

} // namespace

InputState pollInput() noexcept {
    InputState state{};

    // Movement
    state.move.x = axis(eitherDown(KEY_D, KEY_RIGHT), eitherDown(KEY_A, KEY_LEFT));
    state.move.y = axis(eitherDown(KEY_W, KEY_UP), eitherDown(KEY_S, KEY_DOWN));
    const float length = std::hypot(state.move.x, state.move.y);
    if (length > 1.0f) {
        state.move.x /= length;
        state.move.y /= length;
    }

    // Look
    state.look = GetMouseDelta();

    // Jump
    state.jump     = IsKeyPressed(KEY_SPACE);
    state.jumpHeld = IsKeyDown(KEY_SPACE);

    // Walk
    state.walk = eitherDown(KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT);

    return state;
}

} // namespace game::kernel::os
