#pragma once

#include "game/entities/def/entity_type.hpp"
#include "game/entities/def/entity_type_info.hpp"

#include <raylib.h>

namespace game::entities::def {

    struct Entity {
        EntityType type = EntityType::player;
        Vector3 position{};
        Vector3 velocity{};
        float yaw = 0.0f;
        float health = 0.0f;
    };

    [[nodiscard]] constexpr Entity makeEntity(EntityType type, Vector3 position) noexcept {
        return {
            .type = type,
            .position = position,
            .health = infoFor(type).maxHealth
        };
    }

    [[nodiscard]] Vector3 forwardOf(const Entity& entity) noexcept;

    struct Capsule {
        Vector3 bottom;
        Vector3 top;
        float radius;
    };

    [[nodiscard]] Capsule capsuleOf(const Entity& entity) noexcept;

    void drawEntity(const Entity& entity);

}