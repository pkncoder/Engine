#pragma once

#include "./states/EngineState.h"
#include "EngineContext.h"
#include "Window.h"
#include "layers/LayerStack.h"

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
    // Loading the models + allocating the entities
    // TODO: temp
    void setupEntities();

  private:
    // Engine state
    std::unique_ptr<EngineContext> engineContext;
    std::shared_ptr<EngineState> engineState;

    // Layerstack
    LayerStack layerStack;

    // Window information
    std::unique_ptr<Window> window;
};

} // namespace Engine
