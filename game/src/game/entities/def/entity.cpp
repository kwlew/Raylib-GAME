#include "game/entities/def/entity.hpp"

#include <raylib.h>

#include <cmath>

namespace game::entities::def {

    namespace {

        Vector3 offset(Vector3 v, float dx, float dy, float dz) noexcept {
            return { .x = v.x + dx, .y = v.y + dy, .z = v.z + dz };
        }

    }

    Vector3 forwardOf(const Entity& entity) noexcept {
        return { .x = std::sin(entity.yaw), .y = 0.0f, .z = std::cos(entity.yaw) };
    }

    Capsule capsuleOf(const Entity& entity) noexcept {
        const EntityTypeInfo& info = infoFor(entity.type);
        
        return {
            .bottom = offset(entity.position, 0.0f, info.radius, 0.0f),
            .top = offset(entity.position, 0.0f, info.height - info.radius, 0.0f),
            .radius = info.radius
        };
    }

    void drawEntity(const Entity& entity) {
        const EntityTypeInfo& info = infoFor(entity.type);
        const Capsule c = capsuleOf(entity);

        DrawCapsule(c.bottom, c.top, c.radius, 16, 8, info.color);
        DrawCapsuleWires(c.bottom, c.top, c.radius, 16, 8, BLACK);

        const Vector3 forward = forwardOf(entity);
        const Vector3 marker = offset(entity.position, forward.x * info.radius, info.height * 0.75f, forward.z * info.radius);

        DrawSphere(marker, 0.12f, WHITE);
    }

}