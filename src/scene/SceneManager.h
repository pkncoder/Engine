#pragma once

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "Camera.h"
#include "Scene.h"

#include <entt/entity/fwd.hpp>
#include <vector>

namespace Engine {

class SceneManager {
  public:
    SceneManager(EngineContext &engineContext, EngineState &engineState);
    ~SceneManager() = default;

    inline Camera &getCamera() { return camera; }
    inline Scene &getScene() { return scene; }

    // Wrapper for loading an obj
    std::vector<entt::entity> loadObjScene(const std::string &sourceDirectory,
                                           const std::string &filename);

  private:
    EngineContext &engineContext;

    Camera camera;
    Scene scene;
};

} // namespace Engine
