#pragma once

#include "game/world/block.hpp"

#include <array>
#include <cstddef>
#include <functional>

namespace game::world {

    inline constexpr int chunkSize = 16;

    class Chunk {
    public:
        Chunk() noexcept;

        [[nodiscard]] BlockType get(int x, int y, int z) const noexcept;
        void set(int x, int y, int z, BlockType type) noexcept;

    private:
        static constexpr std::size_t blockCount_ = static_cast<std::size_t>(chunkSize) * chunkSize * chunkSize;

        [[nodiscard]] static std::size_t indexOf(int x, int y, int z) noexcept;

        std::array<BlockType, blockCount_> blocks_;
    };

    struct ChunkPos {
        int x;
        int y;
        int z;

        [[nodiscard]] friend constexpr bool operator==(const ChunkPos&, const ChunkPos&) noexcept = default;
    };

    struct ChunkPosHash {
        [[nodiscard]] std::size_t operator()(const ChunkPos& pos) const noexcept {
            std::size_t seed = std::hash<int> {}(pos.x);
            seed ^= std::hash<int> {}(pos.y) + 0x9e3779b9U + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int> {}(pos.z) + 0x9e3779b9U + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

}
