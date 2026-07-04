#pragma once

#include "states/EngineState.h"

#include <memory>

namespace Engine {

class AssetManager;
class RendererManager;
class SceneManager;

class EngineContext {
  public:
    EngineContext();
    ~EngineContext();

    void init(EngineState &state);

    // Getters for Services
    AssetManager &getAsset() { return *assetManager; }
    SceneManager &getScene() { return *sceneManager; }
    RendererManager &getRenderer() { return *rendererManager; }

  private:
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<RendererManager> rendererManager;
};

} // namespace Engine
