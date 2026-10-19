#include "game/kernel/engine.hpp"

#include "game/main_includes.hpp"
#include "game/config.hpp"


int main() {
    using game::kernel::Engine;
    using game::kernel::os::Logger;

    Engine engine;

    auto config = game::defaultConfig;

    if (!engine.initialize(config.window, "My Game")) {
        return EXIT_FAILURE;
    }

    auto update = [&](float dt) {

    };

    auto draw = [&] {
        ClearBackground({ 20, 20, 28, 255 });
    };

    while (engine.running()) {
        engine.frame(update, draw);
    }


    return EXIT_SUCCESS;
}