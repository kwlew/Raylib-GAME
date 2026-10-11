#include "game/entities/def/combat.hpp"
#include "game/entities/def/entity.hpp"
#include "game/entities/def/physics.hpp"
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
#include <cstddef>
#include <random>
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

    constexpr Vector3 playerSpawn = { 0.0f, 0.0f, 0.0f };

    auto player = game::entities::def::makeEntity(EntityType::player, playerSpawn);
    game::kernel::FirstPersonCamera camera(player.position);

    constexpr std::size_t zombieCount = 100;
    // No zombie spawns closer than this to the player, in metres, so none attack instantly.
    constexpr float zombieSafeRadius = 15.0f;

    std::mt19937 rng{ std::random_device{}() };
    std::vector<game::entities::def::Entity> zombies;
    zombies.reserve(zombieCount);

    // Scatters the zombies at random across the arena, away from the player spawn.
    const auto spawnZombies = [&] {
        const float edge = config.world.arenaHalfSize - game::entities::def::infoFor(EntityType::zombie).radius;
        std::uniform_real_distribution<float> coordinate(-edge, edge);
        zombies.clear();
        while (zombies.size() < zombieCount) {
            const Vector3 position = { coordinate(rng), 0.0f, coordinate(rng) };
            if (Vector3Distance(position, playerSpawn) >= zombieSafeRadius) {
                zombies.push_back(game::entities::def::makeEntity(EntityType::zombie, position));
            }
        }
    };
    spawnZombies();

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
        // Soft pushes first, between every pair of bodies, so zombies crowd
        // around the player instead of merging into one; the updates apply them.
        for (std::size_t i = 0; i < zombies.size(); ++i) {
            game::entities::def::pushApart(player, zombies[i], dt);
            for (std::size_t j = i + 1; j < zombies.size(); ++j) {
                game::entities::def::pushApart(zombies[i], zombies[j], dt);
            }
        }

        game::entities::updatePlayer(player, controls, config.world, tiles, dt);
        for (auto& zombie : zombies) {
            game::entities::updateZombie(zombie, player, config.world, tiles, dt);
        }

        if (!game::entities::def::isAlive(player)) {
            Logger::log(LOG_INFO, "Player died; respawning.");
            player = game::entities::def::makeEntity(EntityType::player, playerSpawn);
            spawnZombies();
        }
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
        for (const auto& zombie : zombies) {
            game::entities::def::drawEntity(zombie);
        }
        EndMode3D();

        // Red flash that fades over the invulnerability window after a hit.
        if (player.hurtTimer > 0.0f) {
            const float strength = player.hurtTimer / game::entities::def::invulnerabilityTime;
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, 0.35f * strength));
        }

        // Crosshair at the centre of the screen.
        const int centreX = GetScreenWidth() / 2;
        const int centreY = GetScreenHeight() / 2;
        DrawLine(centreX - 8, centreY, centreX + 8, centreY, RAYWHITE);
        DrawLine(centreX, centreY - 8, centreX, centreY + 8, RAYWHITE);

        // Health bar, bottom-left.
        {
            constexpr int barWidth = 200;
            constexpr int barHeight = 16;
            const int barX = 10;
            const int barY = GetScreenHeight() - barHeight - 10;
            const float maxHealth = game::entities::def::infoFor(player.type).maxHealth;
            const float fraction = player.health / maxHealth;
            DrawRectangle(barX, barY, barWidth, barHeight, Fade(BLACK, 0.6f));
            DrawRectangle(barX, barY, static_cast<int>(static_cast<float>(barWidth) * fraction), barHeight, RED);
            DrawRectangleLines(barX, barY, barWidth, barHeight, RAYWHITE);
            DrawText(TextFormat("%.0f / %.0f", player.health, maxHealth), barX + barWidth + 8, barY - 2, 20, RAYWHITE);
        }

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
