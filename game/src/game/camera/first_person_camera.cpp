#include "game/camera/first_person_camera.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace game::camera {

namespace {

Vector3 add(Vector3 a, Vector3 b) noexcept {
    return { .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z };
}

Vector3 lookDirection(float yaw, float pitch) noexcept {
    return {
        .x = std::cos(pitch) * std::sin(yaw),
        .y = std::sin(pitch),
        .z = std::cos(pitch) * std::cos(yaw),
    };
}

} // namespace

FirstPersonCamera makeFirstPersonCamera(Vector3 feet, float yaw) noexcept {
    FirstPersonCamera view;
    view.camera = {
        .position = {},
        .target = {},
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 70.0f,
        .projection = CAMERA_PERSPECTIVE,
    };
    updateFirstPersonCamera(view, feet, yaw);
    return view;
}

void applyMouseLook(FirstPersonCamera& view, float& yaw, Vector2 look) noexcept {
    yaw = std::remainder(yaw - look.x * view.sensitivity, 2.0f * std::numbers::pi_v<float>);
    view.pitch = std::clamp(view.pitch - look.y * view.sensitivity, -view.maxPitch, view.maxPitch);
}

void updateFirstPersonCamera(FirstPersonCamera& view, Vector3 feet, float yaw) noexcept {
    const Vector3 eye = add(feet, { .x = 0.0f, .y = view.eyeHeight, .z = 0.0f });
    view.camera.position = eye;
    view.camera.target = add(eye, lookDirection(yaw, view.pitch));
}

} // namespace game::camera
