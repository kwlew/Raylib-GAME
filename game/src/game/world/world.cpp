#include "game/world/world.hpp"

namespace game::world {

    namespace {

        // Arithmetic shift floors for negative values; the mask yields a non-negative remainder.
        constexpr int chunkShift = 4;
        constexpr int chunkMask = chunkSize - 1;
        static_assert(chunkSize == (1 << chunkShift), "chunkShift must match chunkSize.");

        constexpr ChunkPos chunkPosOf(int x, int y, int z) noexcept {
            return { .x = x >> chunkShift, .y = y >> chunkShift, .z = z >> chunkShift };
        }

        constexpr int generatedSize = 64;

    }

    BlockType World::getBlock(int x, int y, int z) const noexcept {
        const auto it = chunks_.find(chunkPosOf(x, y, z));
        if (it == chunks_.end()) {
            return BlockType::air;
        }
        return it->second.get(x & chunkMask, y & chunkMask, z & chunkMask);
    }

    void World::setBlock(int x, int y, int z, BlockType type) {
        chunks_[chunkPosOf(x, y, z)].set(x & chunkMask, y & chunkMask, z & chunkMask, type);
    }

    void World::generateFlat() {
        chunks_.clear();
        for (int x = 0; x < generatedSize; ++x) {
            for (int z = 0; z < generatedSize; ++z) {
                setBlock(x, 0, z, BlockType::stone);
            }
        }
    }

}
