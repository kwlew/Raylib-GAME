#include "game/entities/def/entity.hpp"

namespace game::entities::def {

void drawEntity(const Entity& entity) {
    const EntityTypeInfo& info = infoFor(entity.type);
    DrawCircleV(entity.position, info.radius, info.color);
}

} // namespace game::entities
