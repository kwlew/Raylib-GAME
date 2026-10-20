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
            [[nodiscard]] bool isRunning() const noexcept;

            void frame(const std::function<void(float)>& update, const std::function<void()>& draw);

            void setFPS(int fps);

        private:
            GameConfig config_{};
            std::optional<window::Window> window_;

    };

}