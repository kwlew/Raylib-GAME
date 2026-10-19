#include "game/entities/player.hpp"

#include <algorithm>
#include <cmath>

namespace game::entities {

namespace {

// Downward acceleration, in m/s². Stronger than real so jumps feel snappy.
constexpr float gravity = 20.0f;
// Fraction of ground acceleration available in the air.
constexpr float airControl = 0.3f;
// Seconds a jump is still allowed after walking off an edge.
constexpr float coyoteTime = 0.1f;
// Seconds a jump pressed just before landing still fires.
constexpr float jumpBufferTime = 0.1f;

// Moves current towards target by at most maxStep, without overshooting.
[[nodiscard]] Vector2 moveTowards(Vector2 current, Vector2 target, float maxStep) noexcept {
    const Vector2 delta = { .x = target.x - current.x, .y = target.y - current.y };
    const float distance = std::hypot(delta.x, delta.y);
    if (distance <= maxStep) {
        return target;
    }
    const float scale = maxStep / distance;
    return { .x = current.x + delta.x * scale, .y = current.y + delta.y * scale };
}

} // namespace

void updatePlayer(def::Entity& player, const kernel::os::InputState& input, float dt) noexcept {
    const auto& info = def::infoFor(player.type);

    // Horizontal: accelerate towards the wanted velocity.
    const Vector3 forward = { .x = std::sin(player.yaw), .y = 0.0f, .z = std::cos(player.yaw) };
    const Vector3 right = { .x = -std::cos(player.yaw), .y = 0.0f, .z = std::sin(player.yaw) };
    const float targetSpeed = input.walk ? info.walkSpeed : info.speed;
    const Vector2 target = {
        .x = (forward.x * input.move.y + right.x * input.move.x) * targetSpeed,
        .y = (forward.z * input.move.y + right.z * input.move.x) * targetSpeed,
    };
    const bool hasInput = std::hypot(input.move.x, input.move.y) > 0.0f;

    // In the air with no input, keep the current velocity (no air friction).
    if (player.grounded || hasInput) {
        float rate = hasInput ? info.acceleration : info.deceleration;
        if (!player.grounded) {
            rate *= airControl;
        }
        const Vector2 horizontal = moveTowards({ player.velocity.x, player.velocity.z }, target, rate * dt);
        player.velocity.x = horizontal.x;
        player.velocity.z = horizontal.y;
    }

    // Jump
    player.jumpBufferTimer = input.jump ? jumpBufferTime : std::max(0.0f, player.jumpBufferTimer - dt);
    if (player.jumpBufferTimer > 0.0f && info.jumpHeight > 0.0f && (player.grounded || player.timeSinceGrounded < coyoteTime)) {
        player.velocity.y = std::sqrt(2.0f * gravity * info.jumpHeight);
        player.grounded = false;
        player.jumpBufferTimer = 0.0f;
        player.timeSinceGrounded = coyoteTime;
    }

    // Gravity
    if (!player.grounded) {
        player.velocity.y -= gravity * dt;
    }

    player.position.x += player.velocity.x * dt;
    player.position.y += player.velocity.y * dt;
    player.position.z += player.velocity.z * dt;

    // Flat floor at y = 0.
    if (player.position.y <= 0.0f && player.velocity.y <= 0.0f) {
        player.position.y = 0.0f;
        player.velocity.y = 0.0f;
        player.grounded = true;
        player.timeSinceGrounded = 0.0f;
    } else {
        player.grounded = false;
        player.timeSinceGrounded += dt;
    }
}

} // namespace game::entities
