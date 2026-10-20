#include <raylib.h>

#include <cmath>
#include <cstdlib>

#include "game/kernel/os/logger.hpp"
#include "game/camera/first_person.hpp"
#include "game/config.hpp"
#include "game/entities/def/entity.hpp"
#include "game/entities/player.hpp"
#include "game/kernel/engine.hpp"
#include "game/kernel/os/input.hpp"
#include "game/world/world.hpp"
#include "game/world/world_renderer.hpp"


int close();

int main() {
    using game::kernel::os::Logger;
    using game::kernel::Engine;
    using game::GameConfig;

    GameConfig config;
    Engine engine;

    if (!engine.initialize(config)) {
        return EXIT_FAILURE;
    }

    DisableCursor();

    // Declared after `engine` so it is destroyed first, while the GL context is alive.
    game::world::World world;
    world.generateFlat();
    const game::world::WorldRenderer renderer(world);

    Logger::log(LOG_INFO, "Application started successfully");

    using game::entities::def::EntityType;
    using game::entities::def::drawEntity;
    using game::entities::def::makeEntity;

    auto player = makeEntity(EntityType::player, { 32.0f, 1.0f, 32.0f });

    auto view = game::camera::makeFirstPersonCamera(player.position, player.yaw);

    auto update = [&](float dt) {
        const auto input = game::kernel::os::pollInput();
        game::camera::mouseLook(view, player.yaw, input.look);
        game::entities::updatePlayer(player, input, dt);
        game::camera::updateFirstPersonCamera(view, player.position, player.yaw);
    };

    auto draw = [&]() {
        ClearBackground({ 20, 20, 28, 255 });
        BeginMode3D(view.camera);
        renderer.draw();
        EndMode3D();

        const float cx = static_cast<float>(GetScreenWidth()) * 0.5f;
        const float cy = static_cast<float>(GetScreenHeight()) * 0.5f;
        constexpr Color crosshair{ 255, 255, 255, 200 };
        DrawLineEx({ cx - 8.0f, cy }, { cx + 8.0f, cy }, 2.0f, crosshair);
        DrawLineEx({ cx, cy - 8.0f }, { cx, cy + 8.0f }, 2.0f, crosshair);

        DrawFPS(10, 10);
    };

    while(engine.isRunning()) {
        engine.frame(update, draw);
    }

    return close();
}

int close() {
    game::kernel::os::Logger::log(LOG_INFO, "Application closed succesfully");
    return 0;
}