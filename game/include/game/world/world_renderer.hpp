#pragma once

#include "game/world/chunk_mesh.hpp"
#include "game/world/world.hpp"

#include <vector>

namespace game::world {

    // Owns one GPU mesh per chunk. Construct and destroy it while the GL context is alive.
    class WorldRenderer {
    public:
        explicit WorldRenderer(const World& world);

        void draw() const noexcept;

    private:
        std::vector<ChunkMesh> meshes_;
    };

}
