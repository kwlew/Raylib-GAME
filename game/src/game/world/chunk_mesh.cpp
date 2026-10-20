#include "game/world/chunk_mesh.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <utility>

namespace game::world {

    namespace {

        struct Face {
            int dx;
            int dy;
            int dz;
            float shade;
            // Corner offsets within the block, counter-clockwise seen from outside.
            std::array<std::array<int, 3>, 4> corners;
        };

        constexpr std::array<Face, 6> faces {{
            { .dx = 0, .dy = 1, .dz = 0, .shade = 1.0f, .corners = {{ { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 }, { 0, 1, 0 } }} },
            { .dx = 0, .dy = -1, .dz = 0, .shade = 0.5f, .corners = {{ { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 } }} },
            { .dx = 1, .dy = 0, .dz = 0, .shade = 0.8f, .corners = {{ { 1, 0, 1 }, { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 } }} },
            { .dx = -1, .dy = 0, .dz = 0, .shade = 0.8f, .corners = {{ { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 } }} },
            { .dx = 0, .dy = 0, .dz = 1, .shade = 0.65f, .corners = {{ { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } }} },
            { .dx = 0, .dy = 0, .dz = -1, .shade = 0.65f, .corners = {{ { 1, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 } }} },
        }};

        constexpr std::size_t maxVertices = std::numeric_limits<std::uint16_t>::max();

        unsigned char shadeChannel(unsigned char channel, float shade) noexcept {
            return static_cast<unsigned char>(static_cast<float>(channel) * shade + 0.5f);
        }

        void emitQuad(ChunkMeshData& data, int x, int y, int z, const Face& face, Color color) {
            const auto base = static_cast<std::uint16_t>(data.vertices.size() / 3);
            const Color shaded {
                shadeChannel(color.r, face.shade),
                shadeChannel(color.g, face.shade),
                shadeChannel(color.b, face.shade),
                color.a,
            };

            for (const auto& corner : face.corners) {
                data.vertices.push_back(static_cast<float>(x + corner[0]));
                data.vertices.push_back(static_cast<float>(y + corner[1]));
                data.vertices.push_back(static_cast<float>(z + corner[2]));
                data.normals.push_back(static_cast<float>(face.dx));
                data.normals.push_back(static_cast<float>(face.dy));
                data.normals.push_back(static_cast<float>(face.dz));
                data.colors.push_back(shaded.r);
                data.colors.push_back(shaded.g);
                data.colors.push_back(shaded.b);
                data.colors.push_back(shaded.a);
            }

            const std::array<std::uint16_t, 6> quadIndices {
                base,
                static_cast<std::uint16_t>(base + 1),
                static_cast<std::uint16_t>(base + 2),
                base,
                static_cast<std::uint16_t>(base + 2),
                static_cast<std::uint16_t>(base + 3),
            };
            data.indices.insert(data.indices.end(), quadIndices.begin(), quadIndices.end());
        }

        // Copies a vector into a MemAlloc'd array, which UnloadModel later frees.
        template <typename T>
        T* copyToRaylib(const std::vector<T>& source) {
            auto* destination = static_cast<T*>(MemAlloc(static_cast<unsigned int>(source.size() * sizeof(T))));
            std::memcpy(destination, source.data(), source.size() * sizeof(T));
            return destination;
        }

    }

    ChunkMeshData generateChunkMeshData(const World& world, ChunkPos pos) {
        ChunkMeshData data;
        const int originX = pos.x * chunkSize;
        const int originY = pos.y * chunkSize;
        const int originZ = pos.z * chunkSize;

        for (int ly = 0; ly < chunkSize; ++ly) {
            for (int lz = 0; lz < chunkSize; ++lz) {
                for (int lx = 0; lx < chunkSize; ++lx) {
                    const int x = originX + lx;
                    const int y = originY + ly;
                    const int z = originZ + lz;

                    const BlockInfo& info = blockInfo(world.getBlock(x, y, z));
                    if (!info.solid) {
                        continue;
                    }

                    for (const Face& face : faces) {
                        if (blockInfo(world.getBlock(x + face.dx, y + face.dy, z + face.dz)).solid) {
                            continue;
                        }
                        emitQuad(data, x, y, z, face, info.color);
                    }
                }
            }
        }

        assert(data.vertices.size() / 3 <= maxVertices);
        return data;
    }

    ChunkMesh::~ChunkMesh() {
        release();
    }

    ChunkMesh::ChunkMesh(ChunkMesh&& other) noexcept
        : model_(other.model_)
        , hasModel_(other.hasModel_) {
        other.model_ = Model{};
        other.hasModel_ = false;
    }

    ChunkMesh& ChunkMesh::operator=(ChunkMesh&& other) noexcept {
        if (this != &other) {
            release();
            model_ = other.model_;
            hasModel_ = other.hasModel_;
            other.model_ = Model{};
            other.hasModel_ = false;
        }
        return *this;
    }

    ChunkMesh ChunkMesh::build(const World& world, ChunkPos pos) {
        ChunkMesh result;
        const ChunkMeshData data = generateChunkMeshData(world, pos);
        if (data.indices.empty()) {
            return result;
        }

        Mesh mesh{};
        mesh.vertexCount = static_cast<int>(data.vertices.size() / 3);
        mesh.triangleCount = static_cast<int>(data.indices.size() / 3);
        mesh.vertices = copyToRaylib(data.vertices);
        mesh.normals = copyToRaylib(data.normals);
        mesh.colors = copyToRaylib(data.colors);
        mesh.indices = copyToRaylib(data.indices);

        UploadMesh(&mesh, false);
        result.model_ = LoadModelFromMesh(mesh);
        result.hasModel_ = true;
        return result;
    }

    void ChunkMesh::draw() const noexcept {
        if (hasModel_) {
            DrawModel(model_, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
        }
    }

    void ChunkMesh::release() noexcept {
        if (hasModel_) {
            UnloadModel(model_);
            model_ = Model{};
            hasModel_ = false;
        }
    }

}
