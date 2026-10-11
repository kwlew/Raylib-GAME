#include "game/kernel/input.hpp"

#include <raymath.h>

namespace game::kernel {

namespace {

[[nodiscard]] bool eitherDown(KeyboardKey primary, KeyboardKey alternate) noexcept {
    return IsKeyDown(primary) || IsKeyDown(alternate);
}

} // namespace

InputState pollInput() noexcept {
    Vector2 move{};
    bool sprint = false;
    if (eitherDown(KEY_W, KEY_UP)) {
        move.y += 1.0f;
    }
    if (eitherDown(KEY_S, KEY_DOWN)) {
        move.y -= 1.0f;
    }
    if (eitherDown(KEY_A, KEY_LEFT)) {
        move.x -= 1.0f;
    }
    if (eitherDown(KEY_D, KEY_RIGHT)) {
        move.x += 1.0f;
    }
    if (eitherDown(KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT)) {
        sprint = true;
    }

    // Normalized so diagonals aren't faster.
    return { 
        .move = Vector2Normalize(move), 
        .look = GetMouseDelta(), 
        .jump = IsKeyPressed(KEY_SPACE),
        .jumpHeld = IsKeyDown(KEY_SPACE),
        .sprint = sprint,
     };
}

} // namespace game::kernel
