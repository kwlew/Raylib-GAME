#include <raylib.h>

#include "game/kernel/os/logger.hpp"
#include "game/kernel/engine.hpp"


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

    Logger::log(LOG_INFO, "Application started successfully");

    while(engine.isRunning()) {
        
    }

    return close();
}

int close() {
    game::kernel::os::Logger::log(LOG_INFO, "Application closed succesfully");
    return 0;
}