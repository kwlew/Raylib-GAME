#include "game/kernel/engine.hpp"
#include "game/kernel/logger.hpp"
#include "game/kernel/window.hpp"

#include <exception>

namespace game {

    bool Engine::initialize(int width, int height, const char* title, bool resizeable, int targetFPS) {
        if (window_) {
            Logger::log(LOG_WARNING, "Engine is already initialized.");
            return false;
        }

        try {
            window_.emplace(width, height, title, resizeable);
            setFPS(targetFPS);
            Logger::log(
                LOG_INFO,
                "Engine initialized (%dx%d, %d FPS)",
                width,
                height,
                targetFPS
            );
            return true;
        } catch (const std::exception& exception) {
            Logger::log(LOG_ERROR, "Failed to initialize engine: %s", exception.what());
            return false;
        }
    }

    bool Engine::initialize(const WindowSettings& settings, const char* title) {
        return initialize(settings.width, settings.height, title, settings.resizeable, settings.targetFPS);
    }

    int Engine::shutdown() noexcept {
        window_.reset();
        return 0;
    }

    bool Engine::running() const noexcept {
        return window_ && !window_->shouldClose();
    }

    void Engine::drawFrame(const std::function<void()>& draw) {
        if (!window_) {
            return;
        }

        window_->update();

        BeginDrawing();
        draw();
        EndDrawing();
    }

    void Engine::debugFPS() const noexcept {
        DrawFPS(10, 10);
    }

    void Engine::setFPS(int fps) {
        SetTargetFPS(fps);
    }

}
