#include "game/kernel/window.hpp"

#include <stdexcept>

namespace game {

    Window::Window(int width, int height, const char* title, bool resizable) {
        if (::IsWindowReady()) {
            throw std::runtime_error("Window is already initialized!");
        }

        ::SetConfigFlags(resizable ? FLAG_WINDOW_RESIZABLE : 0);
        ::InitWindow(width, height, title);

        if (!::IsWindowReady()) {
            throw std::runtime_error("Failed to initialize window!");
        }

        state_ = WindowState::Poll();
    }

    Window::~Window() noexcept {
        close();
    }

    // Safe to call more than once: raylib clears IsWindowReady() after CloseWindow().
    void Window::close() noexcept {
        if (::IsWindowReady()) {
            ::CloseWindow();
        }
    }

    void Window::setIcon(Image icon) {
        SetWindowIcon(icon);
    }

    void Window::setTitle(const char* title) {
        SetWindowTitle(title);
    }


    void Window::update() noexcept {
        state_ = WindowState::Poll();
    }

    bool Window::shouldClose() const noexcept {
        return ::WindowShouldClose();
    }

    const WindowState& Window::state() const noexcept {
        return state_;
    }

}