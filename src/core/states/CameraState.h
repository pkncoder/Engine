#pragma once

#include "CameraSettings.h"
#include "Defaults.h"

#include <glm/glm.hpp>

namespace Engine {

struct CameraState {
  public:
    // Camera configurations
    CameraSettings settings = CameraSettings();

    // Positioning
    glm::vec3 position = Defaults::Camera::POSITION;

    // Angles
    float yaw = Defaults::Camera::YAW;
    float pitch = Defaults::Camera::PITCH;
    float fov = Defaults::Camera::FOV;

    // Camera dirty flag
    bool cameraDirty = false;
};

} // namespace Engine
