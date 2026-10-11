#include "game/kernel/logger.hpp"
#include "game/kernel/engine.hpp"
#include "game/config.hpp"
#include "game/version.hpp"

#include <raylib.h>

#include <cstdlib>
#include <climits>

int main() {
    using game::kernel::Engine;
    using game::kernel::Logger;

    Engine engine;

    Logger::initialize(LOG_INFO, LOG_WARNING);

    auto config = game::defaultConfig;
    Logger::log(LOG_INFO, "Starting game with configuration: %dx%d, %d FPS", config.window.width, config.window.height, config.window.targetFPS);
    Logger::log(LOG_INFO, "Game Version: v%s", game::version::string);

    if (!engine.initialize(config.window, "My Game")) {
        Logger::log(LOG_ERROR, "Failed to initialize the game engine.");
        return EXIT_FAILURE;
    }

    auto update = [&]([[maybe_unused]] float dt) {
        
    };

    auto draw = [&] {
        ClearBackground({ 0, 0, 0, 255 });
        if (config.window.showFPS) {
            engine.debugFPS();
        }
        if (config.window.showMemory) {
            engine.debugMemory();
        }
    };

    while (engine.running()) {
        engine.frame(update, draw);
    }

    engine.shutdown();

    return EXIT_SUCCESS;
}
