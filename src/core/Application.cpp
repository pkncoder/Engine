#include "Application.h"

#include "../services/Input.h"
#include "../services/Timer.h"
#include "Defaults.h"

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() {}

// Init the window, camera, etc.
void Application::init() {

    // Create the window
    window = std::make_unique<Window>(Defaults::Window::START_WIDTH,
                                      Defaults::Window::START_HEIGHT,
                                      Defaults::Window::START_TITLE);

    // Init the input service
    Input::init(window->getNativeWindow());

    // Init the timer service
    Timer::init();

    // Init the camera at a starting pos
    camera = Camera(Defaults::Camera::START_POSITION);

    // Construct the rasterizer and init it
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();
}

// Main loop
void Application::run() {

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        // Update the timer service and run the log function
        Timer::update();
        Timer::log();

        // Poll inputs, and then handle them
        Input::update();
        handleInputs();

        // Clear Screen
        window->preFrame();

        // Render the scene
        rasterizer->render(camera, window->getAspectRatio());

        // Do things like event polling & buffer swapping
        window->postFrame();
    }
}

// Handle any inputs that come in this frame
void Application::handleInputs() {

    // Moving camera look at direction
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec2 delta = Input::getMouseDelta();
        camera.processLookingDirectionMovement(delta.x, -delta.y);
    }

    // Move the camera origin on movement
    if (Input::isKeyPressed(GLFW_KEY_W))
        camera.processMovement(FORWARD);
    if (Input::isKeyPressed(GLFW_KEY_S))
        camera.processMovement(BACKWARD);
    if (Input::isKeyPressed(GLFW_KEY_A))
        camera.processMovement(LEFT);
    if (Input::isKeyPressed(GLFW_KEY_D))
        camera.processMovement(RIGHT);
}

} // namespace Engine
