#include "game/entities/zombie.hpp"
#include "game/entities/def/physics.hpp"

#include <raymath.h>

#include <cassert>

namespace game::entities {

namespace {

// Distance between centres, in metres, at which the zombie stops walking.
// Bodies touch at 1 m; the extra covers the distance it takes to slow down.
constexpr float stopDistance = 1.5f;

} // namespace

void updateZombie(
    def::Entity& zombie,
    Vector3 target,
    const WorldSettings& world,
    std::span<const world::Tile> tiles,
    float dt
) noexcept {
    assert(zombie.type == def::EntityType::zombie);
    const def::EntityTypeInfo& info = infoFor(zombie.type);

    const Vector3 toTarget = { target.x - zombie.position.x, 0.0f, target.z - zombie.position.z };
    const Vector3 wanted = Vector3Length(toTarget) > stopDistance
        ? Vector3Scale(Vector3Normalize(toTarget), info.speed)
        : Vector3{};

    def::stepPhysics(zombie, wanted, world, tiles, dt);
}

} // namespace game::entities
