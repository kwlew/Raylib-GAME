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
    // Facing, in radians around +Y. 0 faces +Z; positive turns towards +X.
    float yaw = 0.0f;
    bool grounded = true;
    // Seconds since the entity was last on the ground (coyote time).
    float timeSinceGrounded = 0.0f;
    // Seconds a recent jump press stays valid while waiting to land (jump buffer).
    float jumpBufferTimer = 0.0f;
    float health = 0.0f;
};

[[nodiscard]] constexpr Entity makeEntity(EntityType type, Vector3 position) noexcept {
    return { .type = type, .position = position, .health = infoFor(type).maxHealth };
}

// Unit vector the entity faces, on the ground plane.
[[nodiscard]] Vector3 forwardOf(const Entity& entity) noexcept;

// Vertical capsule: the segment between the two sphere centres, plus a radius.
struct Capsule {
    Vector3 bottom;
    Vector3 top;
    float radius;
};

[[nodiscard]] Capsule capsuleOf(const Entity& entity) noexcept;

// Draws the entity as a capsule resting on its feet position, in its type's colour,
// with a marker showing where it faces.
// Must be called between BeginMode3D() and EndMode3D().
void drawEntity(const Entity& entity);

} // namespace game::entities::def
