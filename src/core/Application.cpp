#include "Application.h"
#include "Input.h"
#include "Window.h"
#include <iostream>
#include <memory>

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() {}

// Init the window, camera, etc.
void Application::Init() {

    // Create the window
    m_Window = std::make_unique<Window>(
        300, 300, "Engine"); // Ensure GLAD is initialized here!

    // Init the input "service"
    Input::Init(m_Window->GetNativeWindow());

    // Init the camera at a starting pos
    // TODO: Get starting information elsewhere
    m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // Construct the rasterizer and init it
    m_Rasterizer = std::make_unique<Rasterizer>();
    m_Rasterizer->Init();
}

// Main loop
void Application::Run() {

    // TODO: Move this into a timer class
    float lastFrame = 0.0f; // Frame delta info
    float deltaTime;
    int frameNum = 1;

    float lastTime = glfwGetTime();
    int nbFrames = 0;

    // Start of main loop, only ends when the window is set to
    while (!m_Window->ShouldClose()) {

        // TODO: Move this into a timer class
        {
            // Inside the while loop...
            double currentTime = glfwGetTime();
            deltaTime = (float)currentTime - lastFrame;
            lastFrame = (float)currentTime;
            nbFrames++;

            // Only print/calculate every 1 second
            if (currentTime - lastTime >= 1.0) {
                // Calculate ms/frame
                float msPerFrame = 1000.0 / double(nbFrames);
                float fps = double(nbFrames);

                std::cout << fps << " FPS (" << msPerFrame << " ms/frame)"
                          << std::endl;

                nbFrames = 0;
                lastTime += 1.0;
            }
        }

        // Input poll
        Input::Update();

        // Moving camera direction
        if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 delta = Input::GetMouseDelta();
            m_Camera.ProcessMouseMovement(delta.x, -delta.y);
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

        // Clear Screen
        // TODO: Add a WindowPreFrame thing
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the scene
        m_Rasterizer->Render(m_Camera, m_Window->GetAspectRatio());

        // Do things like event polling & buffer swapping
        m_Window->OnUpdate();
    }
}

} // namespace Engine
