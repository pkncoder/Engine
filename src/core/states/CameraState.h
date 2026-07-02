#pragma once

#include "CameraSettings.h"
#include "Defaults.h"

#include <glm/glm.hpp>

namespace Engine {

struct CameraState {
  public:
    CameraSettings settings = CameraSettings();

    glm::vec3 position = Defaults::Camera::POSITION;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    float yaw = Defaults::Camera::YAW;
    float pitch = Defaults::Camera::PITCH;
    float fov = Defaults::Camera::FOV;
};

} // namespace Engine
