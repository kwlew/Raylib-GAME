#pragma once

#include "game/config.hpp"
#include "game/entities/def/entity.hpp"
#include "game/world/tile.hpp"

#include <span>

namespace game::entities::def {

// One physics step for any entity type:
// 1. Horizontal velocity chases `wantedHorizontal` (m/s on X/Z) at the type's
//    acceleration/deceleration, with reduced control in the air.
// 2. Gravity pulls it down.
// 3. It moves, colliding with `tiles`, the arena walls and the ground, and
//    `grounded` is updated.
// Set velocity.y beforehand to jump.
void stepPhysics(
    Entity& entity,
    Vector3 wantedHorizontal,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept;

} // namespace game::entities::def
