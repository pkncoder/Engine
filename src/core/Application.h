#pragma once
#include "../renderer/Rasterizer.h"
#include "../scene/camera.h"
#include "Window.h"
#include <memory>

namespace Engine {

class Application {

  public:
    // Constructor & Deconstructor
    Application();
    ~Application();

    // Constructing the active render, window, etc.
    void Init();

    // Main loop
    void Run();

  private:
    // Window information
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;

    // Camera object
    Camera m_Camera;

    // Active renderer
    std::unique_ptr<Rasterizer> m_Rasterizer;
};

} // namespace Engine
