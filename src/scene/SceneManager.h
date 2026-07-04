#pragma once

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "Camera.h"
#include "Scene.h"

#include "Entity.h"

#include <vector>

namespace Engine {

class SceneManager {
  public:
    SceneManager(EngineContext &engineContext, EngineState &engineState);
    ~SceneManager() = default;

    Camera camera;
    Scene scene;

    std::vector<Entity> loadObjScene(const std::string filepath);

  private:
    EngineContext &engineContext;
};

} // namespace Engine
