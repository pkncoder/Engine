#include "Application.h"

#include "../scene/Entity.h"
#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Logger.h"
#include "../services/Timer.h"

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

    engineState = std::make_shared<EngineState>();

    layerStack = LayerStack(engineState);

    // Create the window & relating services
    window = std::make_unique<Window>(*engineState.get());

    Input::init(window->getNativeWindow());
    Input::setEventCallback([this](std::shared_ptr<IEvent> event) {
        layerStack.dispatchEvent(event);
    });

    Timer::init();

    engineContext = std::make_unique<EngineContext>();
    engineContext->init(*engineState.get());

    sceneUpdateLayer = std::make_shared<SceneUpdateLayer>(*engineContext);
    layerStack.pushLayer(sceneUpdateLayer);

    rendererLayer =
        std::make_shared<RendererLayer>(*engineContext->getRenderer());
    layerStack.pushLayer(rendererLayer);

    setupEntities();

    Logger::info("APPLICATION", "Application init complete");
    Logger::setNoPendingLogs(false);
}

// Main loop
void Application::run() {

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        START_PROFILE("Run Loop"); // Setup timer for run loop

        Input::update();

        // Update the timer service and run the log function
        Timer::update();

        // Clear Screen
        window->preFrame();

        layerStack.dispatchStack();

        // Do things like event polling & buffer swapping
        window->postFrame();

        // TODO: temp
        // engineState.scene.camera.cameraDirty = false;

        Logger::info("PROFILE", "FPS: " + std::to_string(Timer::getFPS()),
                     LogType::IN_PLACE);
        Logger::info("PROFILE",
                     "Average FPS: " + std::to_string(Timer::getAverageFPS()),
                     LogType::IN_PLACE);
        Logger::info(
            "CAMERA",
            "X: " + std::to_string(engineState->scene.camera.position.x) +
                "Y: " + std::to_string(engineState->scene.camera.position.y) +
                "Z: " + std::to_string(engineState->scene.camera.position.z),
            LogType::IN_PLACE);

        END_PROFILE("Run Loop"); // End timer for run loop

        Logger::outputLogs();
    }
}

// Handle any inputs that come in this frame

void Application::setupEntities() {
    START_PROFILE("Entity Loading");

    auto *sceneManager = engineContext->getScene();

    // Bunny
    if (0) {

        std::vector<Entity> bunny =
            sceneManager->loadObjScene("assets/models/bunny.obj");

        bunny[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.0f, -1.2f, -4.0f);
    }

    // Dragon
    if (0) {
        std::vector<Entity> dragon =
            sceneManager->loadObjScene("assets/models/dragon.obj");

        dragon[0].getComponent<TransformComponent>().position =
            glm::vec3(1.0f, -0.6f, -4.0f);
    }

    // Cat
    if (0) {
        std::vector<Entity> cat =
            sceneManager->loadObjScene("assets/models/cat.obj");

        cat[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, -0.6f, -4.0f);
    }

    // 🗿
    if (0) {
        std::vector<Entity> moai =
            sceneManager->loadObjScene("assets/models/moai.obj");

        moai[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, 1.3f, -4.0f);
        moai[0].getComponent<TransformComponent>().rotation =
            glm::quat(-0.707f, 0.0f, 0.707f, 0.0f);
        moai[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.14f, 0.14f, 0.14f);
    }

    // Diffuse cube & emmisive cube
    if (0) {
        std::vector<Entity> cube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        cube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.4f, 0.2f, 0.8f);
        cube[0].getComponent<MaterialComponent>().roughness = 0.7;
        cube[0].getComponent<MaterialComponent>().metallic = 1.0;

        cube[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.5f, 1.3f, -4.0f);
        cube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.4f, 0.4f, 0.4f);

        std::vector<Entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

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
        std::vector<Entity> room =
            sceneManager->loadObjScene("assets/models/gayRoom.obj");

        for (Entity entity : room) {
            entity.getComponent<MaterialComponent>().emmissive *=
                glm::vec3(1.0f);
        }
    }

    // Trans flag
    if (0) {
        std::vector<Entity> trans =
            sceneManager->loadObjScene("assets/models/trans.obj");

        for (Entity entity : trans) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(-1.0f, 0.0f, 0.0f);
        }
    }

    // Franch flag
    if (0) {
        std::vector<Entity> french =
            sceneManager->loadObjScene("assets/models/french.obj");

        for (Entity entity : french) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(8.0f, 0.0f, 2.0f);
        }
    }

    // THE BACKROOMS????
    if (0) {
        // https://sketchfab.com/3d-models/backrooms-v2-level-0-made-by-me-in-blender-91d707acdfce4d5d940f7cb8c25c6e31#download
        std::vector<Entity> backrooms =
            sceneManager->loadObjScene("assets/models/backrooms_level1.obj");

        std::vector<Entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

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
        std::vector<Entity> room =
            sceneManager->loadObjScene("assets/models/breakfast_room.obj");

        std::vector<Entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

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
        std::vector<Entity> sponza =
            sceneManager->loadObjScene("assets/models/sponza.obj");

        std::vector<Entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

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
        std::vector<Entity> lostEmpire =
            sceneManager->loadObjScene("assets/models/lost_empire.obj");

        std::vector<Entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

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
        std::vector<Entity> oiiaioooooiai =
            sceneManager->loadObjScene("assets/models/oiiaioooooiai.obj");
    }

    END_PROFILE_STACKED_LOG("Entity Loading");
}

} // namespace Engine
