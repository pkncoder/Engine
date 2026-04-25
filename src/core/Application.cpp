#include "Application.h"
#include "Input.h"
#include "Window.h"
#include <memory>

namespace Engine {
void Application::Init() {
  m_Window = std::make_unique<Window>(
      300, 300, "Engine"); // Ensure GLAD is initialized here!

  // Position camera at (0,0,3) looking at (0,0,0)
  m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

  m_Rasterizer = std::make_unique<Rasterizer>();
  m_Rasterizer->Init();

  Input::Init(m_Window->GetNativeWindow());

  glEnable(GL_DEPTH_TEST);
}

Application::Application() {}
Application::~Application() {}

void Application::Run() {
  float lastFrame = 0.0f;

  while (!m_Window->ShouldClose()) {
    // 1. Calculate DeltaTime
    float currentFrame = (float)glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    Input::Update();

    // Now you can use it for your camera!
    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      glm::vec2 delta = Input::GetMouseDelta();
      m_Camera.ProcessMouseMovement(
          delta.x, -delta.y); // Y is usually inverted in screen space
      ;
    }

    // 2. Process Input -> Camera (SRP in action)
    if (Input::IsKeyPressed(GLFW_KEY_W))
      m_Camera.ProcessKeyboard(FORWARD, deltaTime);
    if (Input::IsKeyPressed(GLFW_KEY_S))
      m_Camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (Input::IsKeyPressed(GLFW_KEY_A))
      m_Camera.ProcessKeyboard(LEFT, deltaTime);
    if (Input::IsKeyPressed(GLFW_KEY_D))
      m_Camera.ProcessKeyboard(RIGHT, deltaTime);

    // 3. Clear Screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 4. Render Scene
    // Inside Render(), use m_Camera.GetProjectionMatrix(aspect) *
    // m_Camera.GetViewMatrix()
    m_Rasterizer->Render(m_Camera);

    // 5. Update Window (Swap buffers/Poll events)
    m_Window->OnUpdate();
  }
}
} // namespace Engine
