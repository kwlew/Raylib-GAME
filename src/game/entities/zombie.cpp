#include "game/entities/zombie.hpp"
#include "game/entities/def/combat.hpp"
#include "game/entities/def/physics.hpp"

#include <raymath.h>

#include <cassert>

namespace game::entities {

namespace {

// Distance between centres, in metres, at which the zombie stops walking.
// Bodies touch at 1 m; the extra covers the distance it takes to slow down.
constexpr float stopDistance = 1.5f;

// Distance between centres, in metres, within which the zombie can hit.
constexpr float attackRange = 1.7f;

// Seconds between the zombie's hits.
constexpr float attackInterval = 1.0f;

} // namespace

void updateZombie(
    def::Entity& zombie,
    def::Entity& target,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept {
    assert(zombie.type == def::EntityType::zombie);
    const def::EntityTypeInfo& info = infoFor(zombie.type);

    def::tickCombat(zombie, dt);

    const Vector3 toTarget = { target.position.x - zombie.position.x, 0.0f, target.position.z - zombie.position.z };
    const float distance = Vector3Length(toTarget);
    const Vector3 wanted = distance > stopDistance
        ? Vector3Scale(Vector3Normalize(toTarget), info.speed)
        : Vector3{};

    // Can't reach a target standing on its head or jumping well over it.
    const bool heightsOverlap = target.position.y < zombie.position.y + info.height
        && zombie.position.y < target.position.y + infoFor(target.type).height;
    if (distance <= attackRange && heightsOverlap && zombie.attackCooldown <= 0.0f) {
        if (def::hit(target, zombie)) {
            zombie.attackCooldown = attackInterval;
        }
    }

    def::stepPhysics(zombie, wanted, world, tiles, dt);
}

} // namespace game::entities
