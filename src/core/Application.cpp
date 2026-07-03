#include "Application.h"

#include "../renderer/RendererManager.h"
#include "../scene/Entity.h"
#include "../scene/EntitySpawner.h"
#include "../scene/Scene.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Logger.h"
#include "../services/Timer.h"
#include "states/RendererSettings.h"

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>

#include <memory>

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() { Logger::shutdown(); }

// Init the window, camera, etc.
void Application::init() {

    // Initialize the logger and set no_pending_logs
    Logger::init();
    Logger::setNoPendingLogs(true);

    engineState = EngineState();

    // Create the window & relating services
    window = std::make_unique<Window>(engineState.window);

    Input::init(window->getNativeWindow());
    Timer::init();

    // Init the camera at a starting pos
    camera = Camera(engineState.scene.camera);

    engineContext = std::make_unique<EngineContext>();
    engineContext->init(engineState);

    // Register the scene components and load the scene
    // TODO: temp
    this->registerSceneComponents(engineContext->getScene());
    this->setupEntities(engineContext->getScene());

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

        // Render
        engineContext->getRenderer().render(*window, camera);

        // Do things like event polling & buffer swapping
        window->postFrame();

        camera.cameraDirty = false;

        Logger::info("PROFILE", "FPS: " + std::to_string(Timer::getFPS()),
                     LogType::IN_PLACE);
        Logger::info("PROFILE",
                     "Average FPS: " + std::to_string(Timer::getAverageFPS()),
                     LogType::IN_PLACE);
        Logger::info("CAMERA",
                     "X: " + std::to_string(camera.position.x) +
                         "Y: " + std::to_string(camera.position.y) +
                         "Z: " + std::to_string(camera.position.z),
                     LogType::IN_PLACE);

        END_PROFILE("Run Loop"); // End timer for run loop

        Logger::outputLogs();
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

    // Move the camera origin slowly
    if (Input::isKeyPressed(GLFW_KEY_UP))
        camera.processMovement(FORWARD, 0.1);
    if (Input::isKeyPressed(GLFW_KEY_DOWN))
        camera.processMovement(BACKWARD, 0.1);
    if (Input::isKeyPressed(GLFW_KEY_LEFT))
        camera.processMovement(LEFT, 0.1);
    if (Input::isKeyPressed(GLFW_KEY_RIGHT))
        camera.processMovement(RIGHT, 0.1);
    if (Input::isKeyPressed(GLFW_KEY_ENTER))
        camera.processMovement(UP, 0.1);
    if (Input::isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
        camera.processMovement(DOWN, 0.1);

    if (Input::isKeyJustPressed(GLFW_KEY_R)) {

        switch (engineState.renderer.settings.currentRenderChoice) {
        case RenderChoice::RASTERIZER:

            // Check to see if compute shaders are compatable with this system
            if (engineState.renderer.settings
                    .systemComputeShaderCompatability) {

                // If they are, swap the render choice and set it in the engine
                // state
                engineContext->getRenderer().swapActiveRenderer(
                    RenderChoice::PATH_TRACER);
                engineState.renderer.settings.currentRenderChoice =
                    RenderChoice::PATH_TRACER;
            } else {

                // Else, send an error
                Logger::error("RENDERER",
                              "Path Tracer not supported on this system.");
            }
            break;
        case RenderChoice::PATH_TRACER:

            // Swap to rasterizer & set the render choice
            engineContext->getRenderer().swapActiveRenderer(
                RenderChoice::RASTERIZER);
            engineState.renderer.settings.currentRenderChoice =
                RenderChoice::RASTERIZER;
            break;
        }
    }
}

void Application::registerSceneComponents(Scene &scene) {
    // Register components
    scene.registerComponent<TransformComponent>();
    scene.registerComponent<MeshComponent>();
    scene.registerComponent<MaterialComponent>();
}

void Application::setupEntities(Scene &scene) {
    START_PROFILE("Entity Loading");

    // Bunny
    if (0) {
        std::vector<Entity> bunny = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/bunny.obj");

        bunny[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.0f, -1.2f, -4.0f);
    }

    // Dragon
    if (0) {
        std::vector<Entity> dragon = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/dragon.obj");

        dragon[0].getComponent<TransformComponent>().position =
            glm::vec3(1.0f, -0.6f, -4.0f);
    }

    // Cat
    if (0) {
        std::vector<Entity> cat = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cat.obj");

        cat[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, -0.6f, -4.0f);
    }

    // 🗿
    if (0) {
        std::vector<Entity> moai = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/moai.obj");

        moai[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, 1.3f, -4.0f);
        moai[0].getComponent<TransformComponent>().rotation =
            glm::quat(-0.707f, 0.0f, 0.707f, 0.0f);
        moai[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.14f, 0.14f, 0.14f);
    }

    // Diffuse cube & emmisive cube
    if (0) {
        std::vector<Entity> cube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        cube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.4f, 0.2f, 0.8f);
        cube[0].getComponent<MaterialComponent>().roughness = 0.7;
        cube[0].getComponent<MaterialComponent>().metallic = 1.0;

        cube[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.5f, 1.3f, -4.0f);
        cube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.4f, 0.4f, 0.4f);

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(1.3f, 8.4f, -0.2f);
        emmissiveCube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.2f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(2.0);
    }

    // Gay Room (me)
    if (1) {
        std::vector<Entity> room = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/gayRoom.obj");

        for (Entity entity : room) {
            entity.getComponent<MaterialComponent>().emmissive *=
                glm::vec3(1.0f);
        }
    }

    // Trans flag
    if (0) {
        std::vector<Entity> trans = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/trans.obj");

        for (Entity entity : trans) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(-1.0f, 0.0f, 0.0f);
        }
    }

    // Franch flag
    if (0) {
        std::vector<Entity> french = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/french.obj");

        for (Entity entity : french) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(8.0f, 0.0f, 2.0f);
        }
    }

    // THE BACKROOMS????
    if (0) {
        // https://sketchfab.com/3d-models/backrooms-v2-level-0-made-by-me-in-blender-91d707acdfce4d5d940f7cb8c25c6e31#download
        std::vector<Entity> backrooms = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/backrooms_level1.obj");

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        // emmissiveCube[0].getComponent<TransformComponent>().position =
        //     glm::vec3(80.2f, 6.5f, -116.7f);
        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(47.1f, 8.1f, -50.8f);
        emmissiveCube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.2f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(2.0);

        // 0.5/0.3 exposure
        // 0.02/0.015 fog density; 0.5, 0.4, 0.1 fog color
        // 1.5 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // Breakfast room
    if (0) {
        std::vector<Entity> room = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/breakfast_room.obj");

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(-2.2f, 3.8f, -1.9f);
        emmissiveCube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.05f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(0.8f, 0.7f, 0.2f);

        // 0.7 exposure
        // 0.02 fog density; 0.3, 0.3, 0.3 fog color
        // No vinette
        // 0.1, 0.3, 0.5 clear color
    }

    // Sponza
    if (0) {
        std::vector<Entity> sponza = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/sponza.obj");

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(1.3f, 8.4f, -0.2f);
        emmissiveCube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.05f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(2.0);

        // 0.15 exposure
        // 0.02 fog density; 0.5, 0.4, 0.1 fog color
        // 1.5 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // Lost empire (Minecraft)
    if (0) {
        std::vector<Entity> lostEmpire = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/lost_empire.obj");

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/cube.obj");

        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(-10.7f, 22.4f, 3.1f);
        emmissiveCube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.05f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(2.0);

        // 0.7 exposure
        // 0.02 fog density; 0.7, 0.85, 0.98 fog color
        // 1.74 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // oiiaioooooiai (broken texture)
    if (0) {
        std::vector<Entity> oiiaioooooiai = EntitySpawner::spawnObjEntity(
            engineContext->getScene(), engineContext->getAsset(),
            "assets/models/oiiaioooooiai.obj");
    }

    END_PROFILE_STACKED_LOG("Entity Loading");
}

} // namespace Engine
