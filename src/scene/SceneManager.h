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

    std::vector<Entity> loadObjScene(const std::string filepath);
    inline Camera &getCamera() { return camera; }

  public:
    Camera camera;
    Scene scene;

  private:
    EngineContext &engineContext;
};

} // namespace Engine
