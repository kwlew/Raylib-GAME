#include "game/world/world_renderer.hpp"

namespace game::world {

    WorldRenderer::WorldRenderer(const World& world) {
        meshes_.reserve(world.chunks().size());
        for (const auto& entry : world.chunks()) {
            meshes_.push_back(ChunkMesh::build(world, entry.first));
        }
    }

    void WorldRenderer::draw() const noexcept {
        for (const auto& mesh : meshes_) {
            mesh.draw();
        }
    }

}
