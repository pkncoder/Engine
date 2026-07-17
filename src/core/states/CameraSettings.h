#pragma once

#include "Defaults.h"

namespace Engine {

struct CameraSettings {
  public:
    // Movement
    float sensitivity = Defaults::Camera::SENSITIVITY;
    float movementSpeed = Defaults::Camera::MOVEMENT_SPEED;
};

} // namespace Engine
