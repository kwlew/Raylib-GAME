#pragma once

#include "game/config.hpp"
#include "game/entities/def/entity.hpp"
#include "game/world/tile.hpp"

#include <span>

namespace game::entities {

// What the player is trying to do this update, already decoupled from devices.
struct PlayerControls {
    // x = right, y = forward, relative to `yaw`. Length 0 or 1.
    Vector2 move{};
    // Radians the player faces; 0 = -Z, positive turns right.
    float yaw = 0.0f;
    bool sprint = false;
    // Jump was pressed this frame.
    bool jumpPressed = false;
};

// Accelerates, jumps and applies gravity to the player, keeping it on or above
// the ground, out of solid `tiles`, and inside the square arena
// [-arenaHalfSize, arenaHalfSize] on X and Z.
void updatePlayer(
    def::Entity& player,
    const PlayerControls& controls,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept;

} // namespace game::entities
