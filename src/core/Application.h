#pragma once

#include "../scene/Scene.h"
#include "./states/EngineState.h"
#include "EngineContext.h"
#include "Window.h"

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

    // Window information
    std::unique_ptr<Window> window;
};

} // namespace Engine
