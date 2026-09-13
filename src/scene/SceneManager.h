#pragma once

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "../system/camera/CameraSystem.h"
#include "Scene.h"

#include <entt/entity/fwd.hpp>
#include <vector>

namespace Engine {

class SceneManager {
  public:
    SceneManager(EngineContext &engineContext, EngineState &engineState);
    ~SceneManager() = default;

    void update();

    // inline Camera &getCamera() { return camera; }
    inline Scene &getScene() { return scene; }

    // Wrapper for loading an obj
    std::vector<entt::entity> loadObjScene(const std::string &sourceDirectory,
                                           const std::string &filename);

  private:
    EngineContext &engineContext;

    CameraSystem cameraSystem;

    // Camera camera;
    Scene scene;
};

} // namespace Engine
