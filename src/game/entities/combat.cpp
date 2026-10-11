#include "game/entities/def/combat.hpp"

#include <raymath.h>

#include <algorithm>

namespace game::entities::def {

namespace {

// Upward speed of the knockback hop, in m/s. Leaving the ground matters: in the
// air only air control fights the knockback, so the target actually flies back.
constexpr float knockbackLift = 3.0f;

} // namespace

void tickCombat(Entity& entity, float dt) noexcept {
    entity.hurtTimer = std::max(0.0f, entity.hurtTimer - dt);
    entity.attackCooldown = std::max(0.0f, entity.attackCooldown - dt);
}

bool hit(Entity& target, const Entity& attacker) noexcept {
    if (target.hurtTimer > 0.0f || !isAlive(target)) {
        return false;
    }
    const EntityTypeInfo& info = infoFor(attacker.type);

    target.health = std::max(0.0f, target.health - info.attackDamage);
    target.hurtTimer = invulnerabilityTime;

    // Straight away from the attacker on the ground; any direction if they overlap exactly.
    Vector3 away = { target.position.x - attacker.position.x, 0.0f, target.position.z - attacker.position.z };
    away = Vector3LengthSqr(away) > 0.0f ? Vector3Normalize(away) : Vector3{ 1.0f, 0.0f, 0.0f };

    target.velocity.x = away.x * info.knockback;
    target.velocity.z = away.z * info.knockback;
    target.velocity.y = std::max(target.velocity.y, knockbackLift);
    return true;
}

} // namespace game::entities::def
