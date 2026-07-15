#pragma once

#include "events/IEvent.h"
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

    inline void setEventCallback(
        const std::function<void(std::shared_ptr<IEvent>)> &callback) {
        dispatchEvent = callback;
    };

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
    static void framebufferSizeEventCallback(GLFWwindow *window, int width,
                                             int height);

    std::function<void(std::shared_ptr<IEvent>)> dispatchEvent = nullptr;

  private:
    // GLFW Window
    GLFWwindow *window = nullptr;
};
} // namespace Engine
