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
    inline SceneManager(EngineContext &_engineContext, EngineState &engineState)
        : engineContext(_engineContext) {}
    ~SceneManager() = default;

    void update();

    // inline Camera &getCamera() { return camera; }
    inline Scene &getScene() { return scene; }

    // Wrapper for loading an obj
    std::vector<entt::entity> loadObjScene(const std::string &sourceDirectory,
                                           const std::string &filename);

    // Loading a scene
    Scene &loadJsonScene(EngineState &state, const std::string &filepath);

    inline void reloadScene(EngineState &state) {
        loadJsonScene(state, "assets/activeScene.json");
    }

  private:
    EngineContext &engineContext;

    CameraSystem cameraSystem;

    // Camera camera;
    Scene scene;
};

} // namespace Engine
