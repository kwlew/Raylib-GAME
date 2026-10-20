#include "game/kernel/window/window.hpp"

#include <stdexcept>

namespace game::kernel::window {

    Window::Window(int width, int height, const char* title, bool resizable, bool vsync, int fps) {
        if (IsWindowReady()) {
            throw std::runtime_error("Window already initialized");
        }

        unsigned int flags = 0;
        if (resizable) flags |= FLAG_WINDOW_RESIZABLE;
        if (vsync) flags |= FLAG_VSYNC_HINT;
        SetConfigFlags(flags);
        InitWindow(width, height, title);

        if (!IsWindowReady()) {
            throw std::runtime_error("Failed to initialize window");
        }

        SetTargetFPS(fps);

        state_ = WindowState::poll();
    }

    Window::~Window() noexcept {
        close();
    }

    void Window::close() noexcept {
        if (IsWindowReady()) {
            CloseWindow();
        }
    }

    void Window::setIcon(Image icon) {
        SetWindowIcon(icon);
    }

    void Window::setTitle(const char* title) {
        SetWindowTitle(title);
    }

    void Window::update() noexcept {
        state_ = WindowState::poll();
    }

    bool Window::shouldClose() const noexcept {
        return WindowShouldClose();
    }

    const WindowState& Window::state() const noexcept {
        return state_;
    }
}
