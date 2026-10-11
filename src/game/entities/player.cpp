#include "game/entities/player.hpp"
#include "game/entities/def/combat.hpp"
#include "game/entities/def/physics.hpp"

#include <raymath.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <span>

namespace game::entities {

namespace {

constexpr float sprintMultiplier = 1.2f;

constexpr float coyoteTime = 0.1f;

constexpr float jumpBufferTime = 0.1f;

[[nodiscard]] float launchSpeed(float height, float gravity) noexcept {
    return std::sqrt(2.0f * gravity * height);
}

[[nodiscard]] Vector3 wantedVelocity(const def::EntityTypeInfo& info, const PlayerControls& controls) noexcept {
    const Vector3 forward = { std::sin(controls.yaw), 0.0f, -std::cos(controls.yaw) };
    const Vector3 right = { std::cos(controls.yaw), 0.0f, std::sin(controls.yaw) };
    const Vector3 direction = Vector3Add(Vector3Scale(right, controls.move.x), Vector3Scale(forward, controls.move.y));

    const float topSpeed = info.speed * (controls.sprint ? sprintMultiplier : 1.0f);
    return Vector3Scale(direction, topSpeed);
}

void updateJump(def::Entity& player, const def::EntityTypeInfo& info, const PlayerControls& controls, float gravity, float dt) noexcept {
    player.timeSinceGrounded = player.grounded ? 0.0f : player.timeSinceGrounded + dt;
    if (controls.jumpPressed || controls.jumpHeld) {
        player.jumpBufferTimer = jumpBufferTime;
    }
    else {
        player.jumpBufferTimer = std::max(0.0f, player.jumpBufferTimer - dt);
    }

    const bool canJump = info.jumpHeight > 0.0f && player.timeSinceGrounded <= coyoteTime;
    if (player.jumpBufferTimer > 0.0f && canJump) {
        player.velocity.y = launchSpeed(info.jumpHeight, gravity);
        player.jumpBufferTimer = 0.0f;
        player.timeSinceGrounded = coyoteTime;
    }
}

} // namespace

void updatePlayer(
    def::Entity& player,
    const PlayerControls& controls,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept {
    assert(player.type == def::EntityType::player);
    const def::EntityTypeInfo& info = infoFor(player.type);

    def::tickCombat(player, dt);
    updateJump(player, info, controls, world.gravity, dt);
    def::stepPhysics(player, wantedVelocity(info, controls), world, tiles, dt);
}

} // namespace game::entities
