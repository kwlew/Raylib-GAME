#pragma once

#include "game/entities/def/entity_type.hpp"

#include <raylib.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <string_view>

namespace game::entities::def {

struct EntityTypeInfo {
    EntityType type;
    std::string_view name;
    bool hostile;
    float maxHealth;
    // Top walking speed, in metres per second.
    float speed;
    // How fast velocity grows towards the wanted speed while moving, in m/s².
    float acceleration;
    // How fast velocity shrinks to zero once input stops, in m/s².
    float deceleration;
    // Peak height of a full jump, in metres; 0 means the type can't jump.
    float jumpHeight;
    // Half the body's width, in metres.
    float radius;
    // Feet to top of head, in metres. The body collides as a box this tall.
    float height;
    Color color;
};

inline constexpr std::array<EntityTypeInfo, entityTypeCount> entityTypes {{
    { .type = EntityType::player, .name = "Player", .hostile = false, .maxHealth = 20.0f, .speed = 5.0f, .acceleration = 20.0f, .deceleration = 25.0f, .jumpHeight = 1.2f, .radius = 0.5f, .height = 1.8f, .color = BLUE },
    { .type = EntityType::zombie, .name = "Zombie", .hostile = true,  .maxHealth = 20.0f, .speed = 3.0f, .acceleration = 12.0f, .deceleration = 15.0f, .jumpHeight = 0.0f, .radius = 0.5f, .height = 1.8f, .color = GREEN },
}};

static_assert(
    [] {
        for (std::size_t i = 0; i < entityTypes.size(); ++i) {
            if (entityTypes[i].type != static_cast<EntityType>(i)) {
                return false;
            }
        }
        return true;
    }(),
    "entityTypes must have one row per EntityType, in enum order."
);

[[nodiscard]] constexpr const EntityTypeInfo& infoFor(EntityType type) noexcept {
    assert(type != EntityType::count);
    return entityTypes[static_cast<std::size_t>(type)];
}

[[nodiscard]] constexpr bool isHostile(EntityType type) noexcept {
    return infoFor(type).hostile;
}

} // namespace game::entities
