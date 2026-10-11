#include "game/world/tile.hpp"

#include <raymath.h>

namespace game::world {

BoundingBox tileBounds(const Tile& tile) noexcept {
    const Vector3 min = {
        static_cast<float>(tile.x) * tileSize,
        static_cast<float>(tile.y) * tileSize,
        static_cast<float>(tile.z) * tileSize,
    };
    return { .min = min, .max = Vector3AddValue(min, tileSize) };
}

void drawTile(const Tile& tile) {
    const BoundingBox bounds = tileBounds(tile);
    const Vector3 centre = Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f);
    DrawCube(centre, tileSize, tileSize, tileSize, tile.color);
    DrawCubeWires(centre, tileSize, tileSize, tileSize, BLACK);
}

} // namespace game::world
