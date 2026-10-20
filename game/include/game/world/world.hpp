#pragma once

#include "game/world/block.hpp"
#include "game/world/chunk.hpp"

#include <unordered_map>

namespace game::world {

    class World {
    public:
        [[nodiscard]] BlockType getBlock(int x, int y, int z) const noexcept;
        void setBlock(int x, int y, int z, BlockType type);

        void generateFlat();

        [[nodiscard]] const std::unordered_map<ChunkPos, Chunk, ChunkPosHash>& chunks() const noexcept {
            return chunks_;
        }

    private:
        std::unordered_map<ChunkPos, Chunk, ChunkPosHash> chunks_;
    };

}
