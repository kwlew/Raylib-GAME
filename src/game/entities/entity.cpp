#include "game/entities/def/entity.hpp"

namespace game::entities::def {

void drawEntity(const Entity& entity) {
    const EntityTypeInfo& info = infoFor(entity.type);
    // A capsule filling the collision box: `radius` wide, `height` tall.
    const Vector3 bottom = { entity.position.x, entity.position.y + info.radius, entity.position.z };
    const Vector3 top = { entity.position.x, entity.position.y + info.height - info.radius, entity.position.z };
    DrawCapsule(bottom, top, info.radius, 8, 8, info.color);
    DrawCapsuleWires(bottom, top, info.radius, 8, 8, Fade(BLACK, 0.4f));
}

} // namespace game::entities
