#pragma once

#include <raylib.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace game::world {

    enum class BlockType : std::uint8_t {
        air,
        stone,
        count,
    };

    inline constexpr std::size_t blockTypeCount = static_cast<std::size_t>(BlockType::count);

    struct BlockInfo {
        Color color;
        bool solid;
    };

    inline constexpr std::array<BlockInfo, blockTypeCount> blockInfos {{
        { .color = { 0, 0, 0, 0 }, .solid = false },
        { .color = { 128, 128, 128, 255 }, .solid = true },
    }};

    [[nodiscard]] constexpr const BlockInfo& blockInfo(BlockType type) noexcept {
        assert(type != BlockType::count);
        return blockInfos[static_cast<std::size_t>(type)];
    }

}
