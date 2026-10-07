#include "game/kernel/logger.hpp"
#include "game/kernel/engine.hpp"
#include "game/config.hpp"

#include <raylib.h>

int main() {
    game::Engine engine;

    game::Logger::init(LOG_INFO, LOG_WARNING);

    auto config = game::defaultConfig;
    game::Logger::log(LOG_INFO, "Starting game with configuration: %dx%d, %d FPS", config.window.width, config.window.height, config.window.targetFPS);


    if (!engine.initialize(config.window, "My Game")) {
        game::Logger::log(LOG_ERROR, "Failed to initialize the game engine.");
        return -1;
    }

    while (engine.running()) {
        engine.drawFrame([&] {
            if (config.window.showFPS) {
                engine.debugFPS();
            }
        });
    }

    return engine.shutdown();
}