#pragma once

#include "game/entities/def/entity.hpp"

namespace game::entities {

// Moves the player with WASD / arrow keys and keeps it inside the window.
void updatePlayer(def::Entity& player, float dt) noexcept;

} // namespace game::entities
