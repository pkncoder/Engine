#pragma once

#include "glad/glad.h"
#include "states/WindowState.h"
#include <GLFW/glfw3.h>
#include <c++/v1/__config>
#include <cstddef>

namespace Engine {
class Window {
  public:
    // Constructor & Deconstructor
    Window(WindowState &state);
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
    WindowState *windowState = nullptr;

    static void framebufferSizeCallback(GLFWwindow *window, int width,
                                        int height);
};
} // namespace Engine
