#pragma once

#include "../renderer/IRenderer.h"
#include "../renderer/PathTracer.h"
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
    // Presenting a texture to the screen
    void presentToScreen();

    // Handling inputs
    void handleInputs();

  private:
    // Window information
    std::unique_ptr<Window> window;

    // Scene object
    Scene activeScene;

    // Camera object
    Camera camera;

    // Both renderers stay loaded in VRAM
    std::unique_ptr<Rasterizer> rasterizer = nullptr;
    std::unique_ptr<PathTracer> pathTracer = nullptr;

    // Pointer to the currently active one
    IRenderer *activeRenderer = nullptr;

    GLuint presentFBO = 0;

    bool swapActiveRendererMark = false;
    bool swapActiveRendererLock = false;
};

} // namespace Engine
