#pragma once

#include <raylib.h>

namespace game::kernel {

// First-person camera: sits at the player's eyes and is aimed with the mouse.
// Yaw 0 looks down -Z; positive yaw turns right, positive pitch looks up.
class FirstPersonCamera final {
public:
    // `feet` is the ground point the player stands on.
    explicit FirstPersonCamera(Vector3 feet) noexcept;

    // Turns the view by `delta` mouse pixels (+x right, +y down).
    void look(Vector2 delta) noexcept;
    // Moves the eyes to above `feet`, keeping the current view direction.
    void follow(Vector3 feet) noexcept;

    [[nodiscard]] float yaw() const noexcept;
    [[nodiscard]] const Camera3D& camera() const noexcept;

private:
    Camera3D camera_{};
    // Radians.
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
};

} // namespace game::kernel
