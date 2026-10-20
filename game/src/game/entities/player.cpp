#include "game/entities/player.hpp"

#include <cmath>

namespace game::entities {

    void updatePlayer(def::Entity& player, const kernel::os::InputState& input, float dt) noexcept {
        const float speed = def::infoFor(player.type).speed;

        const Vector3 forward = def::forwardOf(player);
        const Vector3 right = { .x = -std::cos(player.yaw), .y = 0.0f, .z = std::sin(player.yaw) };

        player.velocity.x = (forward.x * input.move.y + right.x * input.move.x) * speed;
        player.velocity.z = (forward.z * input.move.y + right.z * input.move.x) * speed;

        player.position.x += player.velocity.x * dt;
        player.position.z += player.velocity.z * dt;
    }

}
