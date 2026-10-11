#include "game/kernel/camera.hpp"

#include <raymath.h>

#include <algorithm>
#include <cmath>

namespace game::kernel {

namespace {
// Eye height above the feet, in metres.
constexpr float eyeHeight = 1.6f;

constexpr float fieldOfView = 70.0f;

// Radians of rotation per pixel of mouse movement.
constexpr float mouseSensitivity = 0.0025f;

// Stops just short of straight up/down, where the view would flip.
constexpr float maxPitch = 89.0f * DEG2RAD;
} // namespace

FirstPersonCamera::FirstPersonCamera(Vector3 feet) noexcept {
    camera_.up = { 0.0f, 1.0f, 0.0f };
    camera_.fovy = fieldOfView;
    camera_.projection = CAMERA_PERSPECTIVE;
    follow(feet);
}

void FirstPersonCamera::look(Vector2 delta) noexcept {
    yaw_ += delta.x * mouseSensitivity;
    // Screen y grows downwards, so moving the mouse up looks up.
    pitch_ = std::clamp(pitch_ - (delta.y * mouseSensitivity), -maxPitch, maxPitch);
}

void FirstPersonCamera::follow(Vector3 feet) noexcept {
    camera_.position = { feet.x, feet.y + eyeHeight, feet.z };
    const Vector3 forward = {
        std::cos(pitch_) * std::sin(yaw_),
        std::sin(pitch_),
        -std::cos(pitch_) * std::cos(yaw_),
    };
    camera_.target = Vector3Add(camera_.position, forward);
}

float FirstPersonCamera::yaw() const noexcept {
    return yaw_;
}

const Camera3D& FirstPersonCamera::camera() const noexcept {
    return camera_;
}

} // namespace game::kernel
