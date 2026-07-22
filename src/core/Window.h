#pragma once

#include "events/IEvent.h"
#include "glad/glad.h"
#include "states/EngineState.h"

#include <GLFW/glfw3.h>

#include <functional>
#include <memory>

namespace Engine {
class Window {
  public:
    // Constructor & Deconstructor
    Window(EngineState &state);
    ~Window();

    // OpenGL settings function
    void setSettings(EngineState &state) const;

    // Return the GLFW window (not our wrapper)
    inline GLFWwindow *getNativeWindow() const { return window; }

    inline void setEventCallback(
        const std::function<void(std::shared_ptr<IEvent>)> &callback) {
        dispatchEvent = callback;
    };

    // Pre and post frame actions
    void preFrame() const;
    void postFrame() const;

    // Check to see if the window is marked for death
    bool shouldClose() const;

  private:
    // Polling, and swapping
    void pollEvents() const;
    void swapBuffers() const;

    // Event dispatcher - set in setEventCallback()
    std::function<void(std::shared_ptr<IEvent>)> dispatchEvent = nullptr;

    // Resize callback
    static void framebufferSizeEventCallback(GLFWwindow *window, int width,
                                             int height);

  private:
    // GLFW Window
    GLFWwindow *window = nullptr;
};
} // namespace Engine
