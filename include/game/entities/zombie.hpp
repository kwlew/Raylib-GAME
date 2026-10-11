#pragma once

#include "game/config.hpp"
#include "game/entities/def/entity.hpp"
#include "game/world/tile.hpp"

#include <span>

namespace game::entities {

// Walks the zombie straight towards `target` on the ground, stopping just
// short of it, and hits it whenever it's within reach.
void updateZombie(
    def::Entity& zombie,
    def::Entity& target,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept;

} // namespace game::entities
