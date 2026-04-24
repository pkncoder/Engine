#include "Application.h"

namespace Engine {
Application::Application() {
  m_Window = std::make_unique<Window>(1280, 720, "Ray Tracer Engine - Phase I");
}

Application::~Application() {}

void Application::Run() {
  while (!m_Window->ShouldClose()) {
    // Future logic: dt calculation, input polling

    // Clear screen (The classic "Engine Black/Cornflower Blue")
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Window->SwapBuffers();
    m_Window->PollEvents();
  }
}
} // namespace Engine
