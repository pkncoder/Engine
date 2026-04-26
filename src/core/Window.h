#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Engine {
class Window {
  public:
    // Constructor & Deconstructor
    Window(int width, int height, const std::string &title);
    ~Window();

    // Check to see if the window is marked for death
    bool shouldClose() const;

    // Polling, swapping, and updating
    void pollEvents();
    void swapBuffers();
    void onUpdate();

    // Return the GLFW window (not our wrapper)
    GLFWwindow *getNativeWindow() const { return window; }

    // Size & aspect ratio
    void getSize(int &width, int &height) const;
    float getAspectRatio();

  private:
    // GLFW Window
    GLFWwindow *window;

    // Width & height of window
    int width, height;

    // Title of window
    std::string title;
};
} // namespace Engine
