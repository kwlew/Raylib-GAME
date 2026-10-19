#include "game/kernel/window/window.hpp"

#include <stdexcept>

namespace game::kernel::window {

Window::Window(int width, int height, const char* title, bool resizable, bool vsync) {
    if (IsWindowReady()) {
        throw std::runtime_error("Window is already initialized!");
    }

    unsigned int flags = 0;
    if (resizable) flags |= FLAG_WINDOW_RESIZABLE;
    if (vsync) flags |= FLAG_VSYNC_HINT;
    SetConfigFlags(flags);
    InitWindow(width, height, title);

    if (!IsWindowReady()) {
        throw std::runtime_error("Failed to initialize window!");
    }

    state_ = WindowState::poll();
}

Window::~Window() noexcept {
    close();
}

// Safe to call more than once: raylib clears IsWindowReady() after CloseWindow().
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

} // namespace game::kernel::window
