#include "game/entities/player.hpp"

#include <raymath.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <span>

namespace game::entities {

namespace {

constexpr float sprintMultiplier = 1.2f;

// Fraction of ground acceleration/deceleration available in the air, so a
// jump commits you to its direction instead of letting you turn on a dime.
constexpr float airControl = 0.4f;

// Extra gravity while falling, so jumps come down faster than they go up.
constexpr float fallGravityMultiplier = 1.6f;

// Seconds a jump still works after leaving the ground without jumping.
constexpr float coyoteTime = 0.1f;

// Seconds a jump press is remembered, so pressing just before landing still jumps.
constexpr float jumpBufferTime = 0.1f;

// Moves `current` towards `target` by at most `maxDelta`, without overshooting.
[[nodiscard]] Vector3 moveTowards(Vector3 current, Vector3 target, float maxDelta) noexcept {
    const Vector3 difference = Vector3Subtract(target, current);
    const float distance = Vector3Length(difference);
    if (distance <= maxDelta) {
        return target;
    }
    return Vector3Add(current, Vector3Scale(difference, maxDelta / distance));
}

// Speed that, under `gravity`, peaks exactly `height` metres up: v = sqrt(2gh).
[[nodiscard]] float launchSpeed(float height, float gravity) noexcept {
    return std::sqrt(2.0f * gravity * height);
}

void updateHorizontal(def::Entity& player, const def::EntityTypeInfo& info, const PlayerControls& controls, float dt) noexcept {
    // Turn the facing-relative move into a world direction on the ground (XZ) plane.
    const Vector3 forward = { std::sin(controls.yaw), 0.0f, -std::cos(controls.yaw) };
    const Vector3 right = { std::cos(controls.yaw), 0.0f, std::sin(controls.yaw) };
    const Vector3 direction = Vector3Add(Vector3Scale(right, controls.move.x), Vector3Scale(forward, controls.move.y));

    // Velocity chases the wanted velocity instead of jumping to it. Only X/Z
    // are touched here; Y belongs to jumping and gravity.
    const float topSpeed = info.speed * (controls.sprint ? sprintMultiplier : 1.0f);
    const Vector3 wanted = Vector3Scale(direction, topSpeed);
    const bool moving = Vector2LengthSqr(controls.move) > 0.0f;
    const float rate = (moving ? info.acceleration : info.deceleration) * (player.grounded ? 1.0f : airControl);

    const Vector3 horizontal = moveTowards({ player.velocity.x, 0.0f, player.velocity.z }, wanted, rate * dt);
    player.velocity.x = horizontal.x;
    player.velocity.z = horizontal.z;
}

// Returns the average vertical velocity over this step. Moving by the average
// rather than the end velocity makes the arc exact under constant gravity, so
// a jump peaks at precisely `jumpHeight` whatever the frame rate.
[[nodiscard]] float updateVertical(def::Entity& player, const def::EntityTypeInfo& info, const PlayerControls& controls, float gravity, float dt) noexcept {
    player.timeSinceGrounded = player.grounded ? 0.0f : player.timeSinceGrounded + dt;
    player.jumpBufferTimer = controls.jumpPressed ? jumpBufferTime : std::max(0.0f, player.jumpBufferTimer - dt);

    const bool canJump = info.jumpHeight > 0.0f && player.timeSinceGrounded <= coyoteTime;
    if (player.jumpBufferTimer > 0.0f && canJump) {
        player.velocity.y = launchSpeed(info.jumpHeight, gravity);
        player.grounded = false;
        player.jumpBufferTimer = 0.0f;
        // Uses up the coyote window so the same airtime can't give a second jump.
        player.timeSinceGrounded = coyoteTime;
    }

    // Every jump has the same arc: how long jump is held doesn't matter.
    const float startVelocity = player.velocity.y;
    const float gravityMultiplier = startVelocity < 0.0f ? fallGravityMultiplier : 1.0f;
    player.velocity.y -= gravity * gravityMultiplier * dt;
    return (startVelocity + player.velocity.y) * 0.5f;
}

// Overlap smaller than this (metres) is ignored on the axes not being moved
// along, so a body flush against a tile (e.g. standing on it) isn't treated as
// inside it after float rounding when it moves sideways.
constexpr float contactSlop = 1e-4f;

// The entity's collision box: `radius` out from the feet on X/Z, `height` up.
[[nodiscard]] BoundingBox bodyBounds(Vector3 feet, const def::EntityTypeInfo& info) noexcept {
    return {
        .min = { feet.x - info.radius, feet.y, feet.z - info.radius },
        .max = { feet.x + info.radius, feet.y + info.height, feet.z + info.radius },
    };
}

// True when the boxes overlap. Along `moving` any overlap counts, however
// small, so slow motion (high frame rates) still lands; the other axes need
// more than `contactSlop`, so touching faces don't count as a collision.
[[nodiscard]] bool penetrates(const BoundingBox& a, const BoundingBox& b, float Vector3::* moving) noexcept {
    for (float Vector3::* axis : { &Vector3::x, &Vector3::y, &Vector3::z }) {
        const float slop = axis == moving ? 0.0f : contactSlop;
        if (a.min.*axis >= b.max.*axis - slop || a.max.*axis <= b.min.*axis + slop) {
            return false;
        }
    }
    return true;
}

// Moves the entity `delta` metres along one axis, then pushes it back out of
// any tile it ended up inside. Returns true if a tile blocked the move.
// Moving one axis at a time is what lets the body slide along walls and land on tops.
bool moveAxis(def::Entity& entity, const def::EntityTypeInfo& info, float Vector3::* axis, float delta, std::span<const world::Tile> tiles) noexcept {
    entity.position.*axis += delta;
    if (delta == 0.0f) {
        return false;
    }

    bool blocked = false;
    for (const world::Tile& tile : tiles) {
        const BoundingBox solid = world::tileBounds(tile);
        const BoundingBox body = bodyBounds(entity.position, info);
        if (!penetrates(body, solid, axis)) {
            continue;
        }
        // Back out by exactly the overlap, against the direction of travel.
        entity.position.*axis -= delta > 0.0f ? body.max.*axis - solid.min.*axis : body.min.*axis - solid.max.*axis;
        blocked = true;
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

    if (moveAxis(player, info, &Vector3::x, player.velocity.x * dt, tiles)) {
        player.velocity.x = 0.0f;
    }
    if (moveAxis(player, info, &Vector3::z, player.velocity.z * dt, tiles)) {
        player.velocity.z = 0.0f;
    }
    // Hitting a tile while falling means landing on it; while rising, bumping a ceiling.
    const bool falling = player.velocity.y < 0.0f;
    const bool hitVertically = moveAxis(player, info, &Vector3::y, verticalVelocity * dt, tiles);
    if (hitVertically) {
        player.velocity.y = 0.0f;
    }
    player.grounded = hitVertically && falling;

    // Hitting a wall stops motion into it, so velocity doesn't keep building there.
    const float limit = std::max(0.0f, world.arenaHalfSize - info.radius);
    if (std::abs(player.position.x) > limit) {
        player.position.x = std::clamp(player.position.x, -limit, limit);
        player.velocity.x = 0.0f;
    }
    if (std::abs(player.position.z) > limit) {
        player.position.z = std::clamp(player.position.z, -limit, limit);
        player.velocity.z = 0.0f;
    }

    // The ground is the plane y = 0.
    if (player.position.y <= 0.0f) {
        player.position.y = 0.0f;
        player.velocity.y = std::max(0.0f, player.velocity.y);
        player.grounded = true;
    }
}

} // namespace game::entities
