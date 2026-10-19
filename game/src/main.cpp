#include "game/kernel/engine.hpp"
#include "game/config.hpp"

#include <raylib.h>

#include <cstdlib>

int main() {
    game::kernel::Engine engine;
    game::GameConfig config;

    if (!engine.initialize(config)) {
        return EXIT_FAILURE;
    }

    auto update = [&](float /*dt*/) {};
    auto draw = [&] { ClearBackground({ 20, 20, 28, 255 }); };

    while (engine.running()) {
        engine.frame(update, draw);
    }

    return EXIT_SUCCESS;
}
