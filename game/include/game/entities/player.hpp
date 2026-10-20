#pragma once

#include "game/entities/def/entity.hpp"
#include "game/kernel/os/input.hpp"

namespace game::entities {

    void updatePlayer(def::Entity& player, const kernel::os::InputState& input, float dt) noexcept;

}