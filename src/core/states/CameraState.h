#pragma once

#include "CameraSettings.h"
#include "Defaults.h"

#include <glm/glm.hpp>

namespace Engine {

struct CameraState {
  public:
    CameraSettings settings = CameraSettings();

    glm::vec3 position = Defaults::Camera::POSITION;

    float yaw = Defaults::Camera::YAW;
    float pitch = Defaults::Camera::PITCH;

    float fov = Defaults::Camera::FOV;
    bool cameraDirty = false;
};

} // namespace Engine
