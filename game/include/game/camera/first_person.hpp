#pragma once

#include <raylib.h>

namespace game::camera {

    struct FirstPersonCamera {
        Camera3D camera{};
        float pitch = 0.0f;
        float eyeHeight = 1.6f;
        float sensitivity = 0.0025f;
        float maxPitch = 1.5f;
    };

    [[nodiscard]] FirstPersonCamera makeFirstPersonCamera(Vector3 feet, float yaw) noexcept;

    void mouseLook(FirstPersonCamera& view, float& yaw, Vector2 look) noexcept;

    void updateFirstPersonCamera(FirstPersonCamera& view, Vector3 feet, float yaw) noexcept;

}