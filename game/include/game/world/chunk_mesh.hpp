#pragma once

#include "game/world/chunk.hpp"
#include "game/world/world.hpp"

#include <raylib.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace game::world {

    // CPU-side geometry for one chunk, in world coordinates. Needs no GL context.
    struct ChunkMeshData {
        std::vector<float> vertices;        // 3 floats per vertex
        std::vector<float> normals;         // 3 floats per vertex
        std::vector<unsigned char> colors;  // 4 bytes (RGBA) per vertex
        std::vector<std::uint16_t> indices; // 6 per quad

        [[nodiscard]] std::size_t quadCount() const noexcept {
            return indices.size() / 6;
        }
    };

    [[nodiscard]] ChunkMeshData generateChunkMeshData(const World& world, ChunkPos pos);

    class ChunkMesh {
    public:
        ChunkMesh() noexcept = default;
        ~ChunkMesh();

        ChunkMesh(const ChunkMesh&) = delete;
        ChunkMesh& operator=(const ChunkMesh&) = delete;
        ChunkMesh(ChunkMesh&& other) noexcept;
        ChunkMesh& operator=(ChunkMesh&& other) noexcept;

        // Returns an empty mesh (no model) if the chunk has no visible faces.
        [[nodiscard]] static ChunkMesh build(const World& world, ChunkPos pos);

        void draw() const noexcept;

    private:
        void release() noexcept;

        Model model_{};
        bool hasModel_ = false;
    };

}
