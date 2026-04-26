#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Engine {
class Window {
  public:
    // Constructor & Deconstructor
    Window(int width, int height, const std::string &title);
    ~Window();

    // OpenGL settings function
    void setSettings();

    // Check to see if the window is marked for death
    bool shouldClose() const;

    // Polling, and swapping
    void pollEvents();
    void swapBuffers();

    // Pre and post frame actions
    void preFrame();
    void postFrame();

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
