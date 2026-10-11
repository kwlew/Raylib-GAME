#include "game/entities/def/entity.hpp"

namespace game::entities::def {

void drawEntity(const Entity& entity) {
    const EntityTypeInfo& info = infoFor(entity.type);
    const Vector3 centre = { entity.position.x, entity.position.y + info.radius, entity.position.z };
    DrawSphere(centre, info.radius, info.color);
    DrawSphereWires(centre, info.radius, 8, 8, Fade(BLACK, 0.4f));
}

} // namespace game::entities
