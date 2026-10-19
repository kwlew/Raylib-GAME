#pragma once

#include "game/kernel/os/memory.hpp"
#include "game/kernel/os/logger.hpp"
#include "game/kernel/window/window.hpp"
#include "game/config.hpp"


#include <algorithm>
#include <exception>
#include <cstddef>
#include <functional>
#include <optional>

namespace game::kernel {

    class Engine {
        public:
            bool initialize(int width, int height, const char* title, bool resizable, int targetFPS);
            bool initialize(const WindowSettings& settings, const char* title);
            bool shutdown();
            [[nodiscard]] bool running() const noexcept;
            void frame(const std::function<void(float)>& update, const std::function<void()>& draw);
            void debugFPS() const noexcept;
            void debugMemory() const noexcept;
            void setFPS(int fps);

        private:
            std::optional<window::Window> window_;
            float fpsReportTimer_ = 0.0f;
            float memorySampleTimer_ = 0.0f;
            std::size_t memoryUsage_ = 0;
    };
}