#pragma once

#include "game/kernel/window/window.hpp"
#include "game/config.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace game::kernel {

    class Engine {
        public:
            Engine() = default;
            ~Engine();

            Engine(const Engine&) = delete;
            Engine& operator=(const Engine&) = delete;
            Engine(Engine&&) = delete;
            Engine& operator=(Engine&&) = delete;

            bool initialize(const GameConfig& config);
            bool shutdown();
            [[nodiscard]] bool running() const noexcept;
            void frame(const std::function<void(float)>& update, const std::function<void()>& draw);
            // Adds a line to the debug overlay for this frame only. Call every frame from update or draw.
            void debugText(std::string_view text, Color color = LIME);
            [[nodiscard]] bool debugVisible() const noexcept;
            void setFPS(int fps);

        private:
            void drawDebugLine(int row, const char* text, Color color) const noexcept;

            struct DebugLine {
                std::string text;
                Color color;
            };

            GameConfig config_{};
            std::optional<window::Window> window_;
            Font debugFont_{};
            float fpsReportTimer_ = 0.0f;
            float memorySampleTimer_ = 0.0f;
            std::size_t memoryUsage_ = 0;
            bool debugVisible_ = true;
            std::vector<DebugLine> debugLines_;
    };
}
