#include "game/entities/def/physics.hpp"

#include <raymath.h>

#include <algorithm>
#include <limits>

namespace game::entities::def {

namespace {

// Fraction of ground acceleration available in the air.
constexpr float airControl = 0.4f;

// Extra gravity while falling, so jumps come down faster than they go up.
constexpr float fallGravityMultiplier = 1.6f;

// Speed, in m/s, at which two bodies are pushed apart when fully overlapping
// (centres together). Kept below walking speed so bodies stay walk-through.
constexpr float maxPushSpeed = 3.0f;

// Overlap ignored on the axes not being moved along, so touching faces don't collide.
constexpr float contactSlop = 1e-4f;

void accelerate(Entity& entity, const EntityTypeInfo& info, Vector3 wanted, float dt) noexcept {
    const bool moving = Vector3LengthSqr(wanted) > 0.0f;
    const float rate = (moving ? info.acceleration : info.deceleration) * (entity.grounded ? 1.0f : airControl);

    const Vector3 horizontal = Vector3MoveTowards({ entity.velocity.x, 0.0f, entity.velocity.z }, wanted, rate * dt);
    entity.velocity.x = horizontal.x;
    entity.velocity.z = horizontal.z;
}

// Returns the average vertical velocity over the step, which makes the arc
// exact under constant gravity whatever the frame rate.
[[nodiscard]] float applyGravity(Entity& entity, float gravity, float dt) noexcept {
    const float startVelocity = entity.velocity.y;
    const float multiplier = startVelocity < 0.0f ? fallGravityMultiplier : 1.0f;
    entity.velocity.y -= gravity * multiplier * dt;
    return (startVelocity + entity.velocity.y) * 0.5f;
}

[[nodiscard]] BoundingBox bodyBounds(Vector3 feet, const EntityTypeInfo& info) noexcept {
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

// Moves along one axis, pushes out of tiles, keeps the feet in `feetRange`,
// and stops velocity on that axis if blocked. Returns true if blocked.
bool moveAxis(
    Entity& entity,
    const EntityTypeInfo& info,
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

void stepPhysics(
    Entity& entity,
    Vector3 wantedHorizontal,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept {
    const EntityTypeInfo& info = infoFor(entity.type);

    accelerate(entity, info, wantedHorizontal, dt);
    const float verticalVelocity = applyGravity(entity, world.gravity, dt);

    // Where the feet may go: inside the arena walls and on or above the ground (y = 0).
    const float limit = std::max(0.0f, world.arenaHalfSize - info.radius);
    const BoundingBox feetRange = {
        .min = { -limit, 0.0f, -limit },
        .max = { limit, std::numeric_limits<float>::infinity(), limit },
    };

    moveAxis(entity, info, &Vector3::x, (entity.velocity.x * dt) + entity.push.x, feetRange, tiles);
    moveAxis(entity, info, &Vector3::z, (entity.velocity.z * dt) + entity.push.z, feetRange, tiles);
    entity.push = {};
    // Being blocked while falling means landing; while rising, bumping a ceiling.
    const bool falling = entity.velocity.y < 0.0f;
    entity.grounded = moveAxis(entity, info, &Vector3::y, verticalVelocity * dt, feetRange, tiles) && falling;
}

void pushApart(Entity& a, Entity& b, float dt) noexcept {
    const EntityTypeInfo& infoA = infoFor(a.type);
    const EntityTypeInfo& infoB = infoFor(b.type);

    // Bodies only touch if their heights overlap (e.g. not when jumping over one).
    const bool verticalOverlap = a.position.y < b.position.y + infoB.height && b.position.y < a.position.y + infoA.height;
    if (!verticalOverlap) {
        return;
    }

    // Horizontally the bodies are treated as circles, so the push direction
    // turns smoothly around a body instead of snapping between box faces.
    const Vector2 apart = { a.position.x - b.position.x, a.position.z - b.position.z };
    const float distance = Vector2Length(apart);
    const float reach = infoA.radius + infoB.radius;
    if (distance >= reach) {
        return;
    }

    // Exactly on top of each other has no direction; pick one so they still separate.
    const Vector2 direction = distance > 0.0f ? Vector2Scale(apart, 1.0f / distance) : Vector2{ 1.0f, 0.0f };
    const float depth = (reach - distance) / reach;
    const float step = maxPushSpeed * depth * dt * 0.5f;

    a.push.x += direction.x * step;
    a.push.z += direction.y * step;
    b.push.x -= direction.x * step;
    b.push.z -= direction.y * step;
}

} // namespace game::entities::def
