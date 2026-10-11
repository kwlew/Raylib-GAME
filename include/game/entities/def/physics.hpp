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
// Set velocity.y beforehand to jump. Any pending `push` is applied too.
void stepPhysics(
    Entity& entity,
    Vector3 wantedHorizontal,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept;

// Soft collision between two bodies, like Minecraft mobs: if they overlap,
// each is nudged apart a little this step instead of being blocked. The nudge
// is stronger the deeper the overlap but always slower than walking, so you
// can still walk through, and standing inside another body slides you out.
// Call before both entities' stepPhysics() for the frame.
void pushApart(Entity& a, Entity& b, float dt) noexcept;

} // namespace game::entities::def
