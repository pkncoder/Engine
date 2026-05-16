#include "Application.h"

#include "../renderer/BufferManager.h"
#include "../resources/AssetManager.h"
#include "../scene/Entity.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Logger.h"
#include "../services/Timer.h"
#include "Defaults.h"

#include <GLFW/glfw3.h>

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() { Logger::shutdown(); }

// Init the window, camera, etc.
void Application::init() {

    // Initialize the logger and set no_pending_logs
    Logger::init();
    Logger::setNoPendingLogs(true);

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

        START_PROFILE("Mesh Loading");

        // Upload the CPU mesh data to the GPU (VRAM)
        // MeshComponent meshComponentOne = BufferManager::uploadMesh(
        //     *AssetManager::loadMesh("assets/models/bunny.obj"));
        // MeshComponent meshComponentTwo = BufferManager::uploadMesh(
        //     *AssetManager::loadMesh("assets/models/dragon.obj"));
        // MeshComponent meshComponentThree = BufferManager::uploadMesh(
        //     *AssetManager::loadMesh("assets/models/cat.obj"));
        // MeshComponent meshComponentFour = BufferManager::uploadMesh(
        //     *AssetManager::loadMesh("assets/models/moai.obj"));
        MeshComponent meshComponentFive = BufferManager::uploadMesh(
            *AssetManager::loadMesh("assets/models/cube.obj"));

        END_PROFILE_STACKED_LOG("Mesh Loading");

        // Create entity wrappers & instiate entity ids in the ECS (Scene.h)
        // Entity entityOne(activeScene.createEntity(), &activeScene);
        // Entity entityTwo(activeScene.createEntity(), &activeScene);
        // Entity entityThree(activeScene.createEntity(), &activeScene);
        // Entity entityFour(activeScene.createEntity(), &activeScene);
        Entity entityFive(activeScene.createEntity(), &activeScene);

        // Add mesh #1 components
        // entityOne.addComponent<MeshComponent>(meshComponentOne);
        // entityOne.addComponent<Transform>({glm::vec3(-1.0f, -1.2f, -4.0f),
        //                                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        //                                    glm::vec3(1.0f, 1.0f, 1.0f)});
        //
        // // Add mesh #2 components
        // entityTwo.addComponent<MeshComponent>(meshComponentTwo);
        // entityTwo.addComponent<Transform>({glm::vec3(1.0f, -0.6f, -4.0f),
        //                                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        //                                    glm::vec3(1.0f, 1.0f, 1.0f)});
        //
        // // Add mesh #3 components
        // entityThree.addComponent<MeshComponent>(meshComponentThree);
        // entityThree.addComponent<Transform>({glm::vec3(0.0f, -0.6f, -4.0f),
        //                                      glm::quat(1.0f, 0.0f, 0.0f,
        //                                      0.0f),
        //                                      glm::vec3(1.0f, 1.0f, 1.0f)});
        //
        // // Add mesh #4 components
        // entityFour.addComponent<MeshComponent>(meshComponentFour);
        // entityFour.addComponent<Transform>(
        //     {glm::vec3(0.0f, 1.3f, -4.0f),
        //      glm::quat(-0.707f, 0.0f, 0.707f, 0.0f),
        //      glm::vec3(0.14f, 0.14f, 0.14f)});

        // Add mesh #4 components
        entityFive.addComponent<MeshComponent>(meshComponentFive);
        entityFive.addComponent<Transform>({glm::vec3(-1.5f, 1.3f, -4.0f),
                                            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.4f, 0.4f, 0.4f)});
    }

    // Construct the rasterizer and init it
    rasterizer = std::make_unique<Rasterizer>();
    rasterizer->init();

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if (major > 4 || (major == 4 && minor >= 6)) {
        pathTracer = std::make_unique<PathTracer>();
        pathTracer->init();
    }

    activeRenderer = rasterizer.get();

    Logger::info("APPLICATION", "Application init complete");
    Logger::setNoPendingLogs(false);
}

// Main loop
void Application::run() {

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        START_PROFILE("Run Loop"); // Setup timer for run loop

        // Update the timer service and run the log function
        Timer::update();

        // Poll inputs, and then handle them
        Input::update();
        handleInputs();

        // Clear Screen
        window->preFrame();

        // Render the scene
        START_PROFILE("Render"); // Start timer for renderer
        // rasterizer->render(camera, activeScene, window->getAspectRatio());
        activeRenderer->render(camera, activeScene, window->getAspectRatio());
        END_PROFILE("Render"); // End Timer for renderer

        START_PROFILE("Blit"); // Blit profiler
        int width, height;
        window->getSize(width, height);
        activeRenderer->resize(width, height);
        END_PROFILE("Blit"); // Blit profiler

        // 2. Present the compute texture to the main window
        activeRenderer->present(width, height);

        // Do things like event polling & buffer swapping
        window->postFrame();

        Logger::info("PROFILE", "FPS: " + std::to_string(Timer::getFPS()),
                     LogType::IN_PLACE);
        Logger::info("PROFILE",
                     "Average FPS: " + std::to_string(Timer::getAverageFPS()),
                     LogType::IN_PLACE);

        END_PROFILE("Run Loop"); // End timer for run loop

        Logger::outputLogs();

        Timer::periodicRun(3, []() {
            Logger::info("DEBUG", "Second: " + std::to_string(
                                                   (int)Timer::getTotalTime()));
        });
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

    if (swapActiveRendererMark && !swapActiveRendererLock) {

        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        if (activeRenderer == rasterizer.get()) {

            if (major > 4 || (major == 4 && minor >= 6)) {
                activeRenderer = pathTracer.get();
                Logger::info("RENDERER", "Swapped to Path Tracer");
            } else {
                Logger::error("RENDERER",
                              "Path Tracer is not supported on this system.");
            }
        } else {
            activeRenderer = rasterizer.get();
            Logger::info("RENDERER", "Swapped to Rasterizer");
        }

        swapActiveRendererLock = true;
    }

    if (Input::isKeyPressed(GLFW_KEY_R)) {
        swapActiveRendererMark = true;
    } else {
        swapActiveRendererMark = false;
        swapActiveRendererLock = false;
    }
}

} // namespace Engine
