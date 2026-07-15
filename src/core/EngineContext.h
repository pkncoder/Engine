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
    inline AssetManager *getAsset() const { return assetManager.get(); }
    inline SceneManager *getScene() const { return sceneManager.get(); }
    inline RendererManager *getRenderer() const {
        return rendererManager.get();
    }

  private:
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<RendererManager> rendererManager;
};

} // namespace Engine
