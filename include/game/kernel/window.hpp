#pragma once

#include "game/kernel/window_state.hpp"

#include <raylib.h>

namespace game {

    class Window final {
        public:
        Window(int width, int height, const char* title, bool resizable = false);
        ~Window() noexcept;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        void close() noexcept;

        void setIcon(Image icon);
        void setTitle(const char *title);

        void update() noexcept;
        [[nodiscard]] bool shouldClose() const noexcept;
        [[nodiscard]] const WindowState& state() const noexcept;

        private:
        WindowState state_{};
    };

}