#include "game/kernel/engine.hpp"

#include "game/kernel/os/logger.hpp"

#include <algorithm>
#include <exception>

namespace game::kernel {

    using game::kernel::os::Logger;

    Engine::~Engine() {
        if (window_) {
            shutdown();
        }
    }

    bool Engine::initialize(const GameConfig& config) {
        Logger::initialize(config.log.appLevel, config.log.raylibLevel);

        if (window_) {
            Logger::log(LOG_WARNING, "Engine is already initialized.");
            return false;
        }

        config_ = config;
        
        try {
            window_.emplace(
                config.window.width,
                config.window.height,
                config.window.title,
                config.window.resizable,
                config.window.vsync,
                config.window.targetFPS
            );

            SetExitKey(KEY_ESCAPE);

            Logger::log(
                LOG_INFO,
                "Engine initialized: \"%s\" (%dx%d, %d FPS)",
                config.window.title,
                config.window.width,
                config.window.height,
                config.window.targetFPS
            );
            return true;

        } catch (const std::exception& exception) {
            Logger::log(LOG_ERROR, "Failed to initialize engine: %s", exception.what());
            return false;
        }
    }

    bool Engine::shutdown() {
        Logger::log(LOG_INFO, "Shutting down engine...");
        if (!window_) {
            Logger::log(LOG_WARNING, "Trying to shutdown unitialized engine.");
            return false;
        }

        window_.reset();
        Logger::log(LOG_INFO, "Engine shutdown.");
        return true;
    }

    bool Engine::isRunning() const noexcept {
        return window_ && !window_->shouldClose();
    }

    void Engine::frame(const std::function<void(float)>& update, const std::function<void()>& draw) {
        if (!window_) {
            return;
        }

        window_->update();

        const float frameTime = GetFrameTime();

        update(std::min(frameTime, config_.maxFrameTime));

        BeginDrawing();
        draw();

        EndDrawing();
    }

    void Engine::setFPS(int fps) {
        SetTargetFPS(fps);
    }

}