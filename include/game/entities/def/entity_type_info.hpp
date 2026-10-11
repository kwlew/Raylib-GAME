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
    // Pixels per second.
    float speed;
    float radius;
    Color color;
};

inline constexpr std::array<EntityTypeInfo, entityTypeCount> entityTypes {{
    { .type = EntityType::player, .name = "Player", .hostile = false, .maxHealth = 20.0f, .speed = 120.0f, .radius = 12.0f, .color = BLUE },
    { .type = EntityType::zombie, .name = "Zombie", .hostile = true,  .maxHealth = 20.0f, .speed = 70.0f,  .radius = 12.0f, .color = GREEN },
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
