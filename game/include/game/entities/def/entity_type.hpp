#pragma once

#include <cstddef>

namespace game::entities::def {

    enum class EntityType {
        player,
        zombie,
        count
    };

    inline constexpr std::size_t entityTypeCount = static_cast<std::size_t>(EntityType::count);

}