#include "game/kernel/engine.hpp"

namespace game::kernel {

    namespace {
        constexpr float maxFrameTime = 0.1f;

        constexpr float fpsReportInterval = 1.5f;

        constexpr float memorySampleInterval = 0.5f;

        constexpr double bytesPerMebibyte = 1024.0 * 1024.0;
    }

    using game::kernel::os::Logger;

    bool Engine::initialize(int width, int height, const char* title, bool resizable, int targetFPS) {
        Logger::initialize(LOG_INFO, LOG_WARNING);

        if (window_) {
            Logger::log(LOG_WARNING, "Engine is already initialized.");
            return false;
        }

        try {
            window_.emplace(width, height, title, resizable);
            // Pressing ESC makes running() return false so the main loop ends and shutdown() runs.
            SetExitKey(KEY_ESCAPE);
            setFPS(targetFPS);
            memoryUsage_ = os::getMemoryUsage();
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
        return initialize(settings.width, settings.height, title, settings.resizable, settings.targetFPS);
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
            Logger::log(LOG_INFO, "FPS: %d", GetFPS());
        }

        memorySampleTimer_ += frameTime;
        if (memorySampleTimer_ >= memorySampleInterval) {
            memorySampleTimer_ -= memorySampleInterval;
            memoryUsage_ = os::getMemoryUsage();
        }

        update(std::min(frameTime, maxFrameTime));

        BeginDrawing();
        draw();
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