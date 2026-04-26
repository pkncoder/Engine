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
    bool ShouldClose() const;

    // Polling, swapping, and updating
    void PollEvents();
    void SwapBuffers();
    void OnUpdate();

    // Return the GLFW window (not our wrapper)
    GLFWwindow *GetNativeWindow() const { return m_Window; }

    // Size & aspect ratio
    void GetSize(int &width, int &height) const;
    float GetAspectRatio();

  private:
    // GLFW Window
    GLFWwindow *m_Window;

    // Width & height of window
    int m_Width, m_Height;

    // Title of window
    std::string m_Title;
};
} // namespace Engine
