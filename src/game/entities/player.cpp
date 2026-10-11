#include "game/entities/player.hpp"

#include <raylib.h>
#include <raymath.h>

#include <algorithm>
#include <cassert>

namespace game::entities {

namespace {

// WASD / arrow keys as a direction of length 0 or 1, so diagonals aren't faster.
[[nodiscard]] Vector2 inputDirection() noexcept {
    Vector2 direction{};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        direction.y -= 1.0f;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        direction.y += 1.0f;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        direction.x -= 1.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        direction.x += 1.0f;
    }
    return Vector2Normalize(direction);
}

} // namespace

void updatePlayer(def::Entity& player, float dt) noexcept {
    assert(player.type == def::EntityType::player);
    const def::EntityTypeInfo& info = infoFor(player.type);

    player.position = Vector2Add(player.position, Vector2Scale(inputDirection(), info.speed * dt));

    // Screen size is re-read every frame because the window is resizable.
    const float maxX = static_cast<float>(GetScreenWidth()) - info.radius;
    const float maxY = static_cast<float>(GetScreenHeight()) - info.radius;
    player.position.x = std::clamp(player.position.x, info.radius, std::max(info.radius, maxX));
    player.position.y = std::clamp(player.position.y, info.radius, std::max(info.radius, maxY));
}

} // namespace game::entities
