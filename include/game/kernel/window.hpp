#pragma once

#include "game/kernel/window_state.hpp"

#include <raylib.h>

namespace game {

    class Window final {
        public:
        Window(int width, int height, const char* title, bool resizeable = false);
        ~Window() noexcept;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        void setIcon(Image image);
        void setTitle(const char *title);

        void update() noexcept;
        [[nodiscard]] bool shouldClose() const noexcept;
        [[nodiscard]] const WindowState& state() const noexcept;

        private:
        WindowState state_{};
    };

}