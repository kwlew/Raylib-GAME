#pragma once

#include "game/entities/def/entity.hpp"

namespace game::entities::def {

// Seconds an entity can't be hurt again after taking a hit.
inline constexpr float invulnerabilityTime = 0.5f;

// Counts down the entity's combat timers; call once per update.
void tickCombat(Entity& entity, float dt) noexcept;

[[nodiscard]] constexpr bool isAlive(const Entity& entity) noexcept {
    return entity.health > 0.0f;
}

// Hurts `target` for `attacker`'s attackDamage and knocks it away from the
// attacker with a small hop. Does nothing while the target is still
// invulnerable from a previous hit. Returns true if the hit landed.
bool hit(Entity& target, const Entity& attacker) noexcept;

} // namespace game::entities::def
