#include "Application.h"

#include "../renderer/BufferManager.h"
#include "../resources/AssetManager.h"
#include "../scene/Entity.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Timer.h"
#include "Defaults.h"
#include <GLFW/glfw3.h>

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
    activeScene = Scene();
    activeScene.registerComponent<Transform>();
    activeScene
        .registerComponent<MeshComponent>(); // Register our new component

    // 1. Load data from disk to CPU memory
    auto optionalMeshData = AssetManager::loadMesh("assets/models/bunny.obj");

    if (optionalMeshData.has_value()) {
        // 2. Upload CPU data to GPU memory (VRAM) via BufferManager
        MeshComponent cubeMeshHandles =
            BufferManager::uploadMesh(optionalMeshData.value());

        // 3. Create Entity and attach components
        Entity myCube(activeScene.createEntity(), &activeScene);

        myCube.addComponent<Transform>(
            {glm::vec3(0.0f, 0.0f, -5.0f), // Positioned in front of camera
             glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)});

        // 4. Give the entity the GPU handles
        myCube.addComponent<MeshComponent>(cubeMeshHandles);
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
        rasterizer->render(camera, activeScene, window->getAspectRatio());
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
    if (Input::isKeyPressed(GLFW_KEY_SPACE))
        camera.processMovement(UP);
    if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        camera.processMovement(DOWN);
}

} // namespace Engine
