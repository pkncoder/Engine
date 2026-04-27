#include "Application.h"

#include "../resources/AssetManager.h"
#include "../services/Input.h"
#include "../services/Timer.h"
#include "Defaults.h"
#include <iostream>

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

    // Initialize the asset manager
    AssetManager::init();

    // Initialize the scene
    scene = Scene();

    // TODO: temp
    {
        // Load the model data
        // TODO: Figure out about the auto type and when to use it
        auto modelData = AssetManager::loadMesh("assets/models/cube.obj");

        // Check to make sure the model has data in it
        if (modelData.has_value())
            scene.addDebugMesh(modelData.value());

        // Print out the information about the model
        scene.printDebugInfo();
    }

    // Construct the rasterizer and init it
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();
}

// Main loop
void Application::run() {

    // Save a value to keep track of when to log
    double logCounter = 0.0f;

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        SCOPED_PROFILE("Run Loop"); // Setup timer for run loop

        // Update the timer service and run the log function
        Timer::update();

        // Poll inputs, and then handle them
        Input::update();
        handleInputs();

        // Clear Screen
        window->preFrame();

        // Render the scene
        START_PROFILE("Render"); // Start timer for renderer
        rasterizer->render(camera, window->getAspectRatio());
        END_PROFILE("Render"); // End Timer for renderer

        // Do things like event polling & buffer swapping
        window->postFrame();

        // Log performance every second
        logCounter += Timer::getDeltaTime();
        if (logCounter >= 1.0f) {
            Timer::logPerformance();
            logCounter = 0.0;
        }
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
