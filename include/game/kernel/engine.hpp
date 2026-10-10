#pragma once

#include "window.hpp"
#include "game/config.hpp"

#include <functional>
#include <optional>

namespace game {
    
    class Engine {
        public:
            bool initialize(int width, int height, const char* title, bool resizeable, int targetFPS);
            bool initialize(const WindowSettings& settings, const char* title);
            int shutdown() noexcept;
            [[nodiscard]] bool running() const noexcept;
            // Runs one frame: update(dt) with the seconds since the last frame, then draw().
            void frame(const std::function<void(float)>& update, const std::function<void()>& draw);
            void debugFPS() const noexcept;
            void setFPS(int fps);

        private:
            std::optional<Window> window_;
            float fpsReportTimer_ = 0.0f;
            
        };
}
