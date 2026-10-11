#pragma once

#include <raylib.h>

namespace game::world {

// Side length of one tile, in metres. Every distance in the game is in metres,
// so one grid square on the ground is one tile is one metre.
inline constexpr float tileSize = 1.0f;

// A solid cube one tile on each side. (x, y, z) is in tile units and names the
// cube's minimum corner, so tile (0, 0, 0) fills [0, 1] metres on every axis
// and sits on the ground.
struct Tile {
    int x = 0;
    int y = 0;
    int z = 0;
    Color color = GRAY;
};

[[nodiscard]] BoundingBox tileBounds(const Tile& tile) noexcept;

// Must be called between BeginMode3D() and EndMode3D().
void drawTile(const Tile& tile);

} // namespace game::world
