#include "game/kernel/engine.hpp"

#include "game/kernel/os/logger.hpp"
#include "game/kernel/os/memory.hpp"

#include <algorithm>
#include <exception>

namespace game::kernel {

    namespace {
        constexpr float fpsReportInterval = 1.5f;

        constexpr float debugTextSpacing = 1.0f;

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
        debugVisible_ = config.debug.visible;

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

            // The GL context exists now, so the debug font can be uploaded.
            const char* path = TextFormat("%s%s", GetApplicationDirectory(), config.debug.fontPath);
            if (FileExists(path)) {
                debugFont_ = LoadFontEx(path, config.debug.fontSize, nullptr, 0);
                SetTextureFilter(debugFont_.texture, TEXTURE_FILTER_BILINEAR);
            } else {
                Logger::log(LOG_WARNING, "Debug font not found: %s", path);
                debugFont_ = GetFontDefault();
            }

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
        UnloadFont(debugFont_);
        debugFont_ = {};
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

        if (IsKeyPressed(config_.debug.toggleKey)) {
            debugVisible_ = !debugVisible_;
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
        if (debugVisible_) {
            int row = 0;
            if (config_.debug.showFPS) {
                const int fps = GetFPS();
                const Color color = fps < 15 ? RED : (fps < 30 ? ORANGE : LIME);
                drawDebugLine(row++, TextFormat("%2i FPS", fps), color);
            }
            if (config_.debug.showMemory) {
                const double mebibytes = static_cast<double>(memoryUsage_) / bytesPerMebibyte;
                drawDebugLine(row++, TextFormat("RAM: %.1f MB", mebibytes), LIME);
            }
            for (const DebugLine& line : debugLines_) {
                drawDebugLine(row++, line.text.c_str(), line.color);
            }
        }
        EndDrawing();

        debugLines_.clear();
    }

    void Engine::debugText(std::string_view text, Color color) {
        if (!debugVisible_) {
            return;
        }
        // Copy: callers may pass a TextFormat buffer, which raylib reuses.
        debugLines_.push_back({std::string(text), color});
    }

    bool Engine::debugVisible() const noexcept {
        return debugVisible_;
    }

    void Engine::drawDebugLine(int row, const char* text, Color color) const noexcept {
        const float fontSize = static_cast<float>(config_.debug.fontSize);
        const Vector2 position{10.0f, 10.0f + static_cast<float>(row) * (fontSize + 4.0f)};
        DrawTextEx(debugFont_, text, position, fontSize, debugTextSpacing, color);
    }

    void Engine::setFPS(int fps) {
        SetTargetFPS(fps);
    }

} // namespace game::kernel