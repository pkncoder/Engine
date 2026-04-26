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
void Application::init() {

    // Create the window
    window = std::make_unique<Window>(
        300, 300, "Engine"); // Ensure GLAD is initialized here!

    // Init the input "service"
    Input::init(window->getNativeWindow());

    // Init the camera at a starting pos
    // TODO: Get starting information elsewhere
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // Construct the rasterizer and init it
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();
}

// Main loop
void Application::run() {

    // TODO: Move this into a timer class
    float lastFrame = 0.0f; // Frame delta info
    float deltaTime;
    int frameNum = 1;

    float lastTime = glfwGetTime();
    int nbFrames = 0;

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

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
        Input::update();

        // Moving camera direction
        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec2 delta = Input::getMouseDelta();
            camera.processAngleMovement(delta.x, -delta.y);
        }

        // 2. Moving camera position
        if (Input::isKeyPressed(GLFW_KEY_W))
            camera.processMovement(FORWARD, deltaTime);
        if (Input::isKeyPressed(GLFW_KEY_S))
            camera.processMovement(BACKWARD, deltaTime);
        if (Input::isKeyPressed(GLFW_KEY_A))
            camera.processMovement(LEFT, deltaTime);
        if (Input::isKeyPressed(GLFW_KEY_D))
            camera.processMovement(RIGHT, deltaTime);

        // Clear Screen
        window->preFrame();

        // Render the scene
        rasterizer->render(camera, window->getAspectRatio());

        // Do things like event polling & buffer swapping
        window->postFrame();
    }
}

} // namespace Engine
