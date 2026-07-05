#pragma once

#include "./states/EngineState.h"
#include "EngineContext.h"
#include "Window.h"
#include "layers/LayerStack.h"
#include "layers/RendererLayer.h"
#include "layers/SceneUpdateLayer.h"

#include <memory>

namespace Engine {

class Application {

  public:
    // Constructor & Deconstructor
    Application();
    ~Application();

    // Constructing the active render, window, etc.
    void init();

    // Main loop
    void run();

  private:
    // Presenting a texture to the screen
    void presentToScreen();

    // Handling inputs
    void handleInputs();

    // Loading the models + allocating the entities
    // TODO: temp
    void setupEntities();

  private:
    // Engine state
    std::unique_ptr<EngineContext> engineContext;
    EngineState engineState;

    // Layerstack
    LayerStack layerStack;
    std::shared_ptr<SceneUpdateLayer> sceneUpdateLayer;
    std::shared_ptr<RendererLayer> rendererLayer;

    // Window information
    std::unique_ptr<Window> window;
};

} // namespace Engine
