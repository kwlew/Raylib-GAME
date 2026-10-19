#pragma once

#include "game/entities/def/entity.hpp"
#include "game/kernel/os/input.hpp"

namespace game::entities {

// Moves the player from input: accelerates to run or walk speed, jumps with gravity,
// with coyote time and a jump buffer. Flat floor at y = 0, no wall collision yet.
void updatePlayer(def::Entity& player, const kernel::os::InputState& input, float dt) noexcept;

} // namespace game::entities
