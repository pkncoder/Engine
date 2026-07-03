#pragma once

#include "states/EngineState.h"

#include <memory>

namespace Engine {

class AssetManager;
class RendererManager;
class Scene;

class EngineContext {
  public:
    EngineContext() = default;
    ~EngineContext() = default;

    void init(EngineState &state);

    // Getters for Services
    AssetManager &getAsset() { return *assetManager; }
    Scene &getScene() { return *activeScene; }
    RendererManager &getRenderer() { return *rendererManager; }

  private:
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<Scene> activeScene;
    std::unique_ptr<RendererManager> rendererManager;
};

} // namespace Engine
