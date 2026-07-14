#pragma once

#include "glad/glad.h"
#include "states/EngineState.h"
#include <GLFW/glfw3.h>

namespace Engine {
class Window {
  public:
    // Constructor & Deconstructor
    Window(EngineState &state);
    ~Window();

    // OpenGL settings function
    void setSettings();

    // Check to see if the window is marked for death
    bool shouldClose();

    // Polling, and swapping
    void pollEvents();
    void swapBuffers();

    // Pre and post frame actions
    void preFrame();
    void postFrame();

    // Return the GLFW window (not our wrapper)
    GLFWwindow *getNativeWindow() { return window; }

  private:
    // GLFW Window
    GLFWwindow *window = nullptr;
    EngineState *engineState = nullptr;
};
} // namespace Engine
