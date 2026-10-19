#include "game/kernel/engine.hpp"

#include "game/kernel/os/logger.hpp"
#include "game/kernel/os/memory.hpp"

#include <algorithm>
#include <exception>

namespace game::kernel {

    namespace {
        constexpr float fpsReportInterval = 1.5f;

        constexpr double bytesPerMebibyte = 1024.0 * 1024.0;
    }

    using game::kernel::os::Logger;

    Engine::~Engine() {
        if (window_) {
            shutdown();
        }
    }

    bool Engine::initialize(const GameConfig& config) {
        if (window_) {
            Logger::log(LOG_WARNING, "Engine is already initialized.");
            return false;
        }

        Logger::initialize(config.log.appLevel, config.log.raylibLevel);
        config_ = config;

        try {
            window_.emplace(
                config.window.width,
                config.window.height,
                config.window.title,
                config.window.resizable,
                config.window.vsync
            );
            // Pressing ESC makes running() return false so the main loop ends and the Engine destructor shuts down.
            SetExitKey(KEY_ESCAPE);
            setFPS(config.window.targetFPS);
            memoryUsage_ = os::getMemoryUsage();
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
            Logger::log(LOG_WARNING, "Engine is not initialized; nothing to shut down.");
            return false;
        }

        // Destroying the Window closes it exactly once; ~Engine then has nothing left to close.
        window_.reset();
        Logger::log(LOG_INFO, "Engine shutdown.");
        return true;
    }

    bool Engine::running() const noexcept {
        return window_ && !window_->shouldClose();
    }

    void Engine::frame(const std::function<void(float)>& update, const std::function<void()>& draw) {
        if (!window_) {
            return;
        }

        window_->update();

        const float frameTime = GetFrameTime();

        fpsReportTimer_ += frameTime;
        if (fpsReportTimer_ >= fpsReportInterval) {
            fpsReportTimer_ -= fpsReportInterval;
            if (config_.debug.logFPS) {
                Logger::log(LOG_INFO, "FPS: %d", GetFPS());
            }
        }

        memorySampleTimer_ += frameTime;
        if (memorySampleTimer_ >= config_.debug.memorySampleInterval) {
            memorySampleTimer_ -= config_.debug.memorySampleInterval;
            memoryUsage_ = os::getMemoryUsage();
        }

        update(std::min(frameTime, config_.maxFrameTime));

        BeginDrawing();
        draw();
        if (config_.debug.showFPS) debugFPS();
        if (config_.debug.showMemory) debugMemory();
        EndDrawing();
    }

    void Engine::debugFPS() const noexcept {
        DrawFPS(10, 10);
    }

    void Engine::debugMemory() const noexcept {
        const double mebibytes = static_cast<double>(memoryUsage_) / bytesPerMebibyte;
        DrawText(TextFormat("RAM: %.1f MB", mebibytes), 10, 30, 20, LIME);
    }

    void Engine::setFPS(int fps) {
        SetTargetFPS(fps);
    }

} // namespace game::kernel