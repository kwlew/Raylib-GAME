#include "game/world/chunk.hpp"

#include <cassert>
#include <cstddef>

namespace game::world {

    Chunk::Chunk() noexcept {
        blocks_.fill(BlockType::air);
    }

    BlockType Chunk::get(int x, int y, int z) const noexcept {
        return blocks_[indexOf(x, y, z)];
    }

    void Chunk::set(int x, int y, int z, BlockType type) noexcept {
        blocks_[indexOf(x, y, z)] = type;
    }

    std::size_t Chunk::indexOf(int x, int y, int z) noexcept {
        assert(x >= 0 && x < chunkSize);
        assert(y >= 0 && y < chunkSize);
        assert(z >= 0 && z < chunkSize);
        return static_cast<std::size_t>((y * chunkSize + z) * chunkSize + x);
    }

}
