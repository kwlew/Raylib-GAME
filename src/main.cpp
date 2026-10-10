#include "game/kernel/logger.hpp"
#include "game/kernel/engine.hpp"
#include "game/config.hpp"

#include <raylib.h>

#include <cstdlib>
#include <climits>

int main() {
    game::Engine engine;

    game::Logger::init(LOG_INFO, LOG_WARNING);

    auto config = game::defaultConfig;
    game::Logger::log(LOG_INFO, "Starting game with configuration: %dx%d, %d FPS", config.window.width, config.window.height, config.window.targetFPS);


    if (!engine.initialize(config.window, "My Game")) {
        game::Logger::log(LOG_ERROR, "Failed to initialize the game engine.");
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

    return EXIT_SUCCESS;
}
