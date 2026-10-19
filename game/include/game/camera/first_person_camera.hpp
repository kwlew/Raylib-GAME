#pragma once

#include <raylib.h>

namespace game::camera {

struct FirstPersonCamera {
    Camera3D camera{};
    // Up/down look angle, in radians. Positive looks up.
    float pitch = 0.0f;
    // Eyes above the feet, in metres.
    float eyeHeight = 1.6f;
    // Radians turned per pixel of mouse movement.
    float sensitivity = 0.0025f;
    // Pitch limit either way, in radians; just under 90° so the view never flips.
    float maxPitch = 1.5f;
};

// Puts the camera at the eyes above `feet`, looking along `yaw` with level pitch.
[[nodiscard]] FirstPersonCamera makeFirstPersonCamera(Vector3 feet, float yaw) noexcept;

// Turns `yaw` (owned by the entity) and the camera's pitch from this frame's mouse movement.
void applyMouseLook(FirstPersonCamera& view, float& yaw) noexcept;

// Moves the camera to the eyes above `feet` and points it along `yaw` and pitch.
void updateFirstPersonCamera(FirstPersonCamera& view, Vector3 feet, float yaw) noexcept;

} // namespace game::camera
