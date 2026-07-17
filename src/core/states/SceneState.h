#pragma once

#include "CameraState.h"

namespace Engine {

struct SceneState {
  public:
    // Tempory debug camera state
    // TODO: temp
    CameraState camera = CameraState();
};

} // namespace Engine
