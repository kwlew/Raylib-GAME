#include "game/entities/player.hpp"

#include <raymath.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <span>

namespace game::entities {

namespace {

constexpr float sprintMultiplier = 1.2f;

constexpr float airControl = 0.4f;

constexpr float fallGravityMultiplier = 1.6f;

constexpr float coyoteTime = 0.1f;

constexpr float jumpBufferTime = 0.1f;

[[nodiscard]] float launchSpeed(float height, float gravity) noexcept {
    return std::sqrt(2.0f * gravity * height);
}

void updateHorizontal(def::Entity& player, const def::EntityTypeInfo& info, const PlayerControls& controls, float dt) noexcept {
    const Vector3 forward = { std::sin(controls.yaw), 0.0f, -std::cos(controls.yaw) };
    const Vector3 right = { std::cos(controls.yaw), 0.0f, std::sin(controls.yaw) };
    const Vector3 direction = Vector3Add(Vector3Scale(right, controls.move.x), Vector3Scale(forward, controls.move.y));

    const float topSpeed = info.speed * (controls.sprint ? sprintMultiplier : 1.0f);
    const Vector3 wanted = Vector3Scale(direction, topSpeed);
    const bool moving = Vector2LengthSqr(controls.move) > 0.0f;
    const float rate = (moving ? info.acceleration : info.deceleration) * (player.grounded ? 1.0f : airControl);

    const Vector3 horizontal = Vector3MoveTowards({ player.velocity.x, 0.0f, player.velocity.z }, wanted, rate * dt);
    player.velocity.x = horizontal.x;
    player.velocity.z = horizontal.z;
}

[[nodiscard]] float updateVertical(def::Entity& player, const def::EntityTypeInfo& info, const PlayerControls& controls, float gravity, float dt) noexcept {
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
        player.grounded = false;
        player.jumpBufferTimer = 0.0f;
        player.timeSinceGrounded = coyoteTime;
    }

    const float startVelocity = player.velocity.y;
    const float gravityMultiplier = startVelocity < 0.0f ? fallGravityMultiplier : 1.0f;
    player.velocity.y -= gravity * gravityMultiplier * dt;
    return (startVelocity + player.velocity.y) * 0.5f;
}

constexpr float contactSlop = 1e-4f;

[[nodiscard]] BoundingBox bodyBounds(Vector3 feet, const def::EntityTypeInfo& info) noexcept {
    return {
        .min = { feet.x - info.radius, feet.y, feet.z - info.radius },
        .max = { feet.x + info.radius, feet.y + info.height, feet.z + info.radius },
    };
}

[[nodiscard]] bool penetrates(const BoundingBox& a, const BoundingBox& b, float Vector3::* moving) noexcept {
    for (float Vector3::* axis : { &Vector3::x, &Vector3::y, &Vector3::z }) {
        const float slop = axis == moving ? 0.0f : contactSlop;
        if (a.min.*axis >= b.max.*axis - slop || a.max.*axis <= b.min.*axis + slop) {
            return false;
        }
    }
    return true;
}

bool moveAxis(
    def::Entity& entity,
    const def::EntityTypeInfo& info,
    float Vector3::* axis,
    float delta,
    const BoundingBox& feetRange,
    std::span<const world::Tile> tiles
) noexcept {
    if (delta == 0.0f) {
        return false;
    }
    entity.position.*axis += delta;

    bool blocked = false;
    for (const world::Tile& tile : tiles) {
        const BoundingBox solid = world::tileBounds(tile);
        const BoundingBox body = bodyBounds(entity.position, info);
        if (!penetrates(body, solid, axis)) {
            continue;
        }
        entity.position.*axis -= delta > 0.0f ? body.max.*axis - solid.min.*axis : body.min.*axis - solid.max.*axis;
        blocked = true;
    }

    const float bounded = std::clamp(entity.position.*axis, feetRange.min.*axis, feetRange.max.*axis);
    if (bounded != entity.position.*axis) {
        entity.position.*axis = bounded;
        blocked = true;
    }

    if (blocked) {
        entity.velocity.*axis = 0.0f;
    }
    return blocked;
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

    updateHorizontal(player, info, controls, dt);
    const float verticalVelocity = updateVertical(player, info, controls, world.gravity, dt);

    // Where the feet may go: inside the arena walls and on or above the ground (y = 0).
    const float limit = std::max(0.0f, world.arenaHalfSize - info.radius);
    const BoundingBox feetRange = {
        .min = { -limit, 0.0f, -limit },
        .max = { limit, std::numeric_limits<float>::infinity(), limit },
    };

    moveAxis(player, info, &Vector3::x, player.velocity.x * dt, feetRange, tiles);
    moveAxis(player, info, &Vector3::z, player.velocity.z * dt, feetRange, tiles);
    // Being blocked while falling means landing; while rising, bumping a ceiling.
    const bool falling = player.velocity.y < 0.0f;
    player.grounded = moveAxis(player, info, &Vector3::y, verticalVelocity * dt, feetRange, tiles) && falling;
}

} // namespace game::entities
