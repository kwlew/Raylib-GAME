#include "game/camera/first_person_camera.hpp"
#include "game/config.hpp"
#include "game/entities/def/entity.hpp"
#include "game/entities/player.hpp"
#include "game/kernel/engine.hpp"
#include "game/kernel/os/input.hpp"

#include <raylib.h>

#include <cmath>
#include <cstdlib>

int main() {
    game::kernel::Engine engine;
    game::GameConfig config;

    if (!engine.initialize(config)) {
        return EXIT_FAILURE;
    }

    DisableCursor();

    using game::entities::def::EntityType;
    using game::entities::def::drawEntity;
    using game::entities::def::makeEntity;

    auto player = makeEntity(EntityType::player, { 0.0f, 0.0f, 0.0f });
    auto zombie = makeEntity(EntityType::zombie, { 3.0f, 0.0f, 6.0f });
    zombie.yaw = std::atan2(player.position.x - zombie.position.x, player.position.z - zombie.position.z);

    auto view = game::camera::makeFirstPersonCamera(player.position, player.yaw);

    auto update = [&](float dt) {
        const auto input = game::kernel::os::pollInput();
        game::camera::applyMouseLook(view, player.yaw, input.look);
        game::entities::updatePlayer(player, input, dt);
        const float speed = std::hypot(player.velocity.x, player.velocity.z);
        engine.debugText(TextFormat("Speed: %.2f m/s", static_cast<double>(speed)));
        engine.debugText(TextFormat("Vertical: %.2f m/s", static_cast<double>(player.velocity.y)));
        engine.debugText(TextFormat("%s%s", player.grounded ? "Grounded" : "Airborne", input.walk ? " | Walking" : ""));
        game::camera::updateFirstPersonCamera(view, player.position, player.yaw);
    };
    auto draw = [&] {
        ClearBackground({ 20, 20, 28, 255 });
        BeginMode3D(view.camera);
        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 50.0f, 50.0f }, { 40, 40, 52, 255 });
        DrawGrid(50, 1.0f);
        drawEntity(zombie);
        EndMode3D();

        const float cx = static_cast<float>(GetScreenWidth()) * 0.5f;
        const float cy = static_cast<float>(GetScreenHeight()) * 0.5f;
        constexpr Color crosshair{ 255, 255, 255, 200 };
        DrawLineEx({ cx - 8.0f, cy }, { cx + 8.0f, cy }, 2.0f, crosshair);
        DrawLineEx({ cx, cy - 8.0f }, { cx, cy + 8.0f }, 2.0f, crosshair);
    };

    while (engine.running()) {
        engine.frame(update, draw);
    }

    return EXIT_SUCCESS;
}
