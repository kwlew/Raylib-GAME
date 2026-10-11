#pragma once

#include "game/entities/def/entity_type.hpp"
#include "game/entities/def/entity_type_info.hpp"

#include <raylib.h>

namespace game::entities::def {

struct Entity {
    EntityType type = EntityType::player;
    // Feet position, in metres; y = 0 is the ground.
    Vector3 position{};
    // Metres per second. Carried between updates, so speed builds up and dies down.
    Vector3 velocity{};
    // Metres of soft push from overlapping bodies, applied (through tile
    // collision) and cleared by the next stepPhysics().
    Vector3 push{};
    bool grounded = true;
    // Seconds since the entity was last on the ground (coyote time).
    float timeSinceGrounded = 0.0f;
    // Seconds a recent jump press stays valid while waiting to land (jump buffer).
    float jumpBufferTimer = 0.0f;
    float health = 0.0f;
    // Seconds of invulnerability left after being hit.
    float hurtTimer = 0.0f;
    // Seconds until this entity can attack again.
    float attackCooldown = 0.0f;
};

[[nodiscard]] constexpr Entity makeEntity(EntityType type, Vector3 position) noexcept {
    return { .type = type, .position = position, .health = infoFor(type).maxHealth };
}

// Draws the entity as a capsule standing on its feet position, in its type's colour.
// Must be called between BeginMode3D() and EndMode3D().
void drawEntity(const Entity& entity);

} // namespace game::entities
