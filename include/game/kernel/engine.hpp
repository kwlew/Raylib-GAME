#pragma once

#include "game/kernel/window.hpp"
#include "game/config.hpp"

#include <cstddef>
#include <functional>
#include <optional>

namespace game {
    
    class Engine {
        public:
            bool initialize(int width, int height, const char* title, bool resizable, int targetFPS);
            bool initialize(const WindowSettings& settings, const char* title);
            bool shutdown();
            [[nodiscard]] bool running() const noexcept;
            // Runs one frame: update(dt) with the seconds since the last frame, then draw().
            void frame(const std::function<void(float)>& update, const std::function<void()>& draw);
            void debugFPS() const noexcept;
            // Draws the process's resident memory (RAM) usage below the FPS counter.
            void debugMemory() const noexcept;
            void setFPS(int fps);

        private:
            std::optional<Window> window_;
            float fpsReportTimer_ = 0.0f;
            float memorySampleTimer_ = 0.0f;
            std::size_t memoryUsage_ = 0;

        };
}
