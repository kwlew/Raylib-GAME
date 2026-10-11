#pragma once

#include "game/entities/def/entity_type.hpp"
#include "game/entities/def/entity_type_info.hpp"

#include <raylib.h>

namespace game::entities::def {

struct Entity {
    EntityType type = EntityType::player;
    Vector2 position{};
    float health = 0.0f;
};

[[nodiscard]] constexpr Entity makeEntity(EntityType type, Vector2 position) noexcept {
    return { .type = type, .position = position, .health = infoFor(type).maxHealth };
}

// Draws the entity as a circle in its type's colour.
void drawEntity(const Entity& entity);

} // namespace game::entities
