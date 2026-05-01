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

    // TODO: temp (move it somewhere)
    {
        // Register components
        activeScene.registerComponent<Transform>();
        activeScene.registerComponent<MeshComponent>();

        // Load the meshes to CPU from disk
        auto optionalBunnyMesh =
            AssetManager::loadMesh("assets/models/cube.obj");
        auto optionalDragonMesh =
            AssetManager::loadMesh("assets/models/dragon.obj");
        auto optionalCatMesh = AssetManager::loadMesh("assets/models/cat.obj");

        // Upload the CPU mesh data to the GPU (VRAM)
        MeshComponent bunnyMesh =
            BufferManager::uploadMesh(optionalBunnyMesh.value());
        MeshComponent dragonMesh =
            BufferManager::uploadMesh(optionalDragonMesh.value());
        MeshComponent catMesh =
            BufferManager::uploadMesh(optionalCatMesh.value());

        // Create entity wrappers & instiate entity ids in the ECS (Scene.h)
        Entity bunny(activeScene.createEntity(), &activeScene);
        Entity dragon(activeScene.createEntity(), &activeScene);
        Entity cat(activeScene.createEntity(), &activeScene);

        // Add mesh #1 components
        bunny.addComponent<MeshComponent>(bunnyMesh);
        bunny.addComponent<Transform>({glm::vec3(-1.0f, -0.6f, -4.0f),
                                       glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                       glm::vec3(1.0f, 1.0f, 1.0f)});

        // Add mesh #2 components
        dragon.addComponent<MeshComponent>(dragonMesh);
        dragon.addComponent<Transform>({glm::vec3(1.0f, -0.0f, -3.5f),
                                        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                        glm::vec3(1.0f, 1.0f, 1.0f)});

        // Add mesh #3 components
        cat.addComponent<MeshComponent>(catMesh);
        cat.addComponent<Transform>({glm::vec3(0.0f, 0.0f, -4.0f),
                                     glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                     glm::vec3(1.0f, 1.0f, 1.0f)});
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
