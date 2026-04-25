#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace Engine {
class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();

  bool ShouldClose() const;
  void PollEvents();
  void SwapBuffers();

  GLFWwindow *GetNativeWindow() const { return m_Window; }
  void OnUpdate();

private:
  GLFWwindow *m_Window;
  int m_Width, m_Height;
  std::string m_Title;
};
} // namespace Engine
