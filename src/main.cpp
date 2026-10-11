#include "game/entities/def/entity.hpp"
#include "game/entities/player.hpp"
#include "game/entities/zombie.hpp"
#include "game/kernel/camera.hpp"
#include "game/kernel/input.hpp"
#include "game/kernel/logger.hpp"
#include "game/kernel/engine.hpp"
#include "game/world/tile.hpp"
#include "game/config.hpp"
#include "game/version.hpp"

#include <raylib.h>
#include <raymath.h>

#include <cstdlib>
#include <climits>
#include <vector>

int main() {
    using game::kernel::Engine;
    using game::kernel::Logger;

    Engine engine;

    Logger::initialize(LOG_INFO, LOG_WARNING);

    auto config = game::defaultConfig;

    config.window.targetFPS = 240;

    Logger::log(LOG_INFO, "Starting game with configuration: %dx%d, %d FPS", config.window.width, config.window.height, config.window.targetFPS);
    Logger::log(LOG_INFO, "Game Version: v%s", game::version::string);

    if (!engine.initialize(config.window, "My Game")) {
        Logger::log(LOG_ERROR, "Failed to initialize the game engine.");
        return EXIT_FAILURE;
    }

    using game::entities::def::EntityType;

    auto player = game::entities::def::makeEntity(EntityType::player, { 0.0f, 0.0f, 0.0f });
    game::kernel::FirstPersonCamera camera(player.position);

    // Spawns ahead and to the right, past the test block, and walks towards the player.
    auto zombie = game::entities::def::makeEntity(EntityType::zombie, { 6.0f, 0.0f, -12.0f });

    // Test block: one tile tall, 3 m ahead of the spawn point (forward is -Z).
    const std::vector<game::world::Tile> tiles {
        { .x = 0, .y = 0, .z = -4, .color = GRAY },
    };

    // Hides the cursor and locks it to the window so the mouse can turn freely.
    DisableCursor();

    const float arenaHalfSize = config.world.arenaHalfSize;
    const float arenaSize = arenaHalfSize * 2.0f;

    auto update = [&](float dt) {
        const game::kernel::InputState input = game::kernel::pollInput();
        camera.look(input.look);
        const game::entities::PlayerControls controls {
            .move = input.move,
            .yaw = camera.yaw(),
            .sprint = input.sprint,
            .jumpPressed = input.jump,
            .jumpHeld = input.jumpHeld,
        };
        game::entities::updatePlayer(player, controls, config.world, tiles, dt);
        game::entities::updateZombie(zombie, player.position, config.world, tiles, dt);
        camera.follow(player.position);
    };

    auto draw = [&] {
        ClearBackground({ 20, 20, 28, 255 });

        BeginMode3D(camera.camera());
        DrawPlane({ 0.0f, 0.0f, 0.0f }, { arenaSize, arenaSize }, { 40, 44, 52, 255 });
        DrawGrid(static_cast<int>(arenaSize / game::world::tileSize), game::world::tileSize);
        DrawCubeWires({ 0.0f, 0.5f, 0.0f }, arenaSize, 1.0f, arenaSize, RED);
        for (const game::world::Tile& tile : tiles) {
            game::world::drawTile(tile);
        }
        game::entities::def::drawEntity(zombie);
        EndMode3D();

        // Crosshair at the centre of the screen.
        const int centreX = GetScreenWidth() / 2;
        const int centreY = GetScreenHeight() / 2;
        DrawLine(centreX - 8, centreY, centreX + 8, centreY, RAYWHITE);
        DrawLine(centreX, centreY - 8, centreX, centreY + 8, RAYWHITE);

        // 2D overlay: drawn after EndMode3D() so it sits on top of the scene.
        if (config.window.showFPS) {
            engine.debugFPS();
        }
        if (config.window.showMemory) {
            engine.debugMemory();
        }
        if (config.window.showVelocity) {
            const Vector3 v = player.velocity;
            DrawText(
                TextFormat("Velocity: (%.2f, %.2f, %.2f)  %.2f m/s", v.x, v.y, v.z, Vector3Length(v)),
                10, 50, 20, LIME
            );
        }
    };

    while (engine.running()) {
        engine.frame(update, draw);
    }

    engine.shutdown();

    return EXIT_SUCCESS;
}
