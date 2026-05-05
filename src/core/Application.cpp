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

    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

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

        // Load the meshes to CPU from disk
        // auto optionalMeshOne =
        //     AssetManager::loadMesh("assets/models/bunny.obj");
        // auto optionalMeshTwo =
        //     AssetManager::loadMesh("assets/models/dragon.obj");
        // auto optionalMeshThree =
        //     AssetManager::loadMesh("assets/models/cat.obj");
        // auto optionalMeshFour =
        //     AssetManager::loadMesh("assets/models/moai.obj");
        auto optionalMeshFive =
            AssetManager::loadMesh("assets/models/cube.obj");

        END_PROFILE_STACKED_LOG("Mesh Loading");

        // Upload the CPU mesh data to the GPU (VRAM)
        // MeshComponent meshComponentOne =
        //     BufferManager::uploadMesh(optionalMeshOne.value());
        // MeshComponent meshComponentTwo =
        //     BufferManager::uploadMesh(optionalMeshTwo.value());
        // MeshComponent meshComponentThree =
        //     BufferManager::uploadMesh(optionalMeshThree.value());
        // MeshComponent meshComponentFour =
        //     BufferManager::uploadMesh(optionalMeshFour.value());
        MeshComponent meshComponentFive =
            BufferManager::uploadMesh(optionalMeshFive.value());

        // Set mesh component ids for the path tracer
        // meshComponentOne.assetID = "assets/models/bunny.obj";
        // meshComponentTwo.assetID = "assets/models/dragon.obj";
        // meshComponentThree.assetID = "assets/models/cat.obj";
        // meshComponentFour.assetID = "assets/models/moai.obj";
        meshComponentFive.assetID = "assets/models/cube.obj";

        // Create entity wrappers & instiate entity ids in the ECS (Scene.h)
        // Entity entityOne(activeScene.createEntity(), &activeScene);
        // Entity entityTwo(activeScene.createEntity(), &activeScene);
        // Entity entityThree(activeScene.createEntity(), &activeScene);
        // Entity entityFour(activeScene.createEntity(), &activeScene);
        Entity entityFive(activeScene.createEntity(), &activeScene);

        // // Add mesh #1 components
        // entityOne.addComponent<MeshComponent>(meshComponentOne);
        // entityOne.addComponent<Transform>({glm::vec3(-1.0f, -1.2f, -4.0f),
        //                                    glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        //                                    glm::vec3(1.0f, 1.0f, 1.0f)});
        //
        // // Add mesh #2 components
        // entityTwo.addComponent<MeshComponent>(meshComponentTwo);
        // entityTwo.addComponent<Transform>({glm::vec3(1.0f, -0.6f, -3.5f),
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
        entityFive.addComponent<Transform>(
            {glm::vec3(0.0f, 1.3f, -4.0f),
             glm::quat(-0.707f, 0.0f, 0.707f, 0.0f),
             glm::vec3(0.14f, 0.14f, 0.14f)});
    }

    // Construct the rasterizer and init it
    // rasterizer = std::make_unique<Rasterizer>();
    // rasterizer->init();
    pathTracer = std::make_unique<PathTracer>();
    pathTracer->init();

    glGenFramebuffers(1, &presentFBO);

    int width, height;
    window->getSize(width, height);
    pathTracer->resize(width, height);

    Logger::info("SYSTEM", "Application init complete");
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
        pathTracer->render(camera, activeScene, window->getAspectRatio());
        END_PROFILE("Render"); // End Timer for renderer

        // 2. Present the compute texture to the main window
        presentToScreen();

        // Do things like event polling & buffer swapping
        window->postFrame();

        Logger::info("PROFILE", "FPS: " + std::to_string(Timer::getFPS()),
                     LogType::IN_PLACE);
        Logger::info("PROFILE",
                     "Average FPS: " + std::to_string(Timer::getAverageFPS()),
                     LogType::IN_PLACE);

        END_PROFILE("Run Loop"); // End timer for run loop

        Logger::outputLogs();
    }
}

// Helper function to draw the compute shader texture to the window
void Application::presentToScreen() {
    // Bind our temporary FBO for reading
    glBindFramebuffer(GL_READ_FRAMEBUFFER, presentFBO);

    // Attach the Path Tracer's output texture to it
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, pathTracer->getOutputTexture(), 0);

    // Bind the default window buffer for drawing
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    int width, height;
    window->getSize(width, height);
    pathTracer->resize(width, height);

    // Blit (copy) the pixels from the texture FBO to the screen
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
