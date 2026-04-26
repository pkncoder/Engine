#include "Application.h"

#include "../services/Input.h"
#include "../services/Timer.h"

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() {}

// Init the window, camera, etc.
void Application::init() {

    // Create the window
    window = std::make_unique<Window>(
        300, 300, "Engine"); // Ensure GLAD is initialized here!

    // Init the input service
    Input::init(window->getNativeWindow());

    // Init the timer service
    Timer::init();

    // Init the camera at a starting pos
    // TODO: Get starting information elsewhere
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // Construct the rasterizer and init it
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();
}

// Main loop
void Application::run() {

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        // Update the timer information
        Timer::update();

        // Input poll
        Input::update();

        // TODO: Move elsewhere?
        {
            // Moving camera look at direction
            if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
                glm::vec2 delta = Input::getMouseDelta();
                camera.processAngleMovement(delta.x, -delta.y);
            }

            // Move the camera origin on movement
            // TODO: Move delta time into camera basic for one source of truth
            if (Input::isKeyPressed(GLFW_KEY_W))
                camera.processMovement(FORWARD);
            if (Input::isKeyPressed(GLFW_KEY_S))
                camera.processMovement(BACKWARD);
            if (Input::isKeyPressed(GLFW_KEY_A))
                camera.processMovement(LEFT);
            if (Input::isKeyPressed(GLFW_KEY_D))
                camera.processMovement(RIGHT);
        }

        // Clear Screen
        window->preFrame();

        // Render the scene
        rasterizer->render(camera, window->getAspectRatio());

        // Do things like event polling & buffer swapping
        window->postFrame();
    }
}

} // namespace Engine
