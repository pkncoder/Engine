#pragma once

#include "states/EngineState.h"

#include <memory>

namespace Engine {

// Foward declarations, as each of these includes engine context
class AssetManager;
class RendererManager;
class SceneManager;

class EngineContext {
  public:
    // Constructor & Deconstructor; defaulted in .cpp file
    EngineContext();
    ~EngineContext();

    // Main init function for setting the attributes
    void init(EngineState &state);

    // Getters for the managers
    inline AssetManager *getAsset() const { return assetManager.get(); }
    inline SceneManager *getScene() const { return sceneManager.get(); }
    inline RendererManager *getRenderer() const {
        return rendererManager.get();
    }

  private:
    // Unique pointers for each manager
    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<RendererManager> rendererManager;
};

} // namespace Engine
