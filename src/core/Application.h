#pragma once

#include "../renderer/Rasterizer.h"
#include "../scene/Camera.h"
#include "../scene/Scene.h"
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
    // Window information
    std::unique_ptr<Window> window;

    // Scene object
    Scene activeScene;

    // Camera object
    Camera camera;

    // Active renderer
    std::unique_ptr<Rasterizer> rasterizer;

    // Handling inputs
    void handleInputs();
};

} // namespace Engine
