#include "Application.h"
#include "Input.h"
#include "Window.h"
#include <memory>

namespace Engine {

Application::Application() {}

// Init the window, camera, etc.
void Application::Init() {
    m_Window = std::make_unique<Window>(
        300, 300, "Engine"); // Ensure GLAD is initialized here!

    Input::Init(m_Window->GetNativeWindow());

    // Position camera at (0,0,3) looking at (0,0,0)
    m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    m_Rasterizer = std::make_unique<Rasterizer>();
    m_Rasterizer->Init();
}

// Main loop
void Application::Run() {
    float lastFrame = 0.0f; // Frame delta info

    while (!m_Window->ShouldClose()) {
        // 1. Calculate DeltaTime
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input poll
        Input::Update();

        // Moving camera direction
        if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 delta = Input::GetMouseDelta();
            m_Camera.ProcessMouseMovement(delta.x, -delta.y);
            ;
        }

        // 2. Moving camera position
        if (Input::IsKeyPressed(GLFW_KEY_W))
            m_Camera.ProcessKeyboard(FORWARD, deltaTime);
        if (Input::IsKeyPressed(GLFW_KEY_S))
            m_Camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (Input::IsKeyPressed(GLFW_KEY_A))
            m_Camera.ProcessKeyboard(LEFT, deltaTime);
        if (Input::IsKeyPressed(GLFW_KEY_D))
            m_Camera.ProcessKeyboard(RIGHT, deltaTime);

        // Screen clearing
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = m_Window->GetAspectRatio();
        glm::mat4 proj = m_Camera.GetProjectionMatrix(aspect);
        m_Rasterizer->Render(m_Camera.GetViewMatrix(), proj);
        // Render the scene

        // Do things like event polling & buffer swapping
        m_Window->OnUpdate();
    }
}

Application::~Application() {}

} // namespace Engine
