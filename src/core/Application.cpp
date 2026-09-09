#include "Application.h"

#include "../resources/AssetManager.h"
#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Logger.h"
#include "../services/Timer.h"
#include "layers/RendererLayer.h"
#include "layers/SceneUpdateLayer.h"

#include <GLFW/glfw3.h>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float3.hpp>

#include <memory>

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() {

    // Set flags for logging
    Logger::setSkipDashboard(true);
    Logger::setNoPendingLogs(true);

    Logger::line();

    // Shutdown services
    // TODO: create
    // Input::shutdown();
    // Timer::shutdown();

    // Release and shutdown variables
    layerStack.shutdown();
    engineState.reset();

    // Kill the context, this also kills the window
    engineContext.reset();
    window.reset();

    // Shutdown the logger
    Logger::shutdown();
}

// Init the window, camera, etc.
void Application::init() {

    // Logger service
    {
        // Initialize the logger and set no_pending_logs
        Logger::init();
        Logger::setNoPendingLogs(true);
    }

    // Global engine state
    {
        // Get the state of the engine
        engineState = std::make_shared<EngineState>();
    }

    // GLFW services
    {
        // Create the window and set the event callback
        window = std::make_unique<Window>(*engineState.get());
        window->setEventCallback([this](std::shared_ptr<IEvent> event) {
            layerStack.dispatchEvent(event);
        });

        // Initialize the input service and set the event callback
        Input::init(window->getNativeWindow());
        Input::setEventCallback([this](std::shared_ptr<IEvent> event) {
            layerStack.dispatchEvent(event);
        });

        // Initialize the timer service
        Timer::init();
    }

    // Global engine context WIP
    {
        // Create & initialize the engine context
        engineContext = std::make_unique<EngineContext>();
        engineContext->init(*engineState.get());
    }

    // LayerStack
    {
        // Create the layer stack
        layerStack = LayerStack(engineState);

        // Initialize the layers
        auto sceneUpdateLayer =
            std::make_shared<SceneUpdateLayer>(*engineContext);
        auto rendererLayer = std::make_shared<RendererLayer>(*engineContext);

        // Push each layer
        layerStack.pushLayer(sceneUpdateLayer);
        layerStack.pushLayer(rendererLayer);
    }

    // TODO: temp; setup scene
    setupEntities();

    // Log the final init log & turn off setNoPendingLogs
    Logger::info("APPLICATION", "Application init complete");
    Logger::setNoPendingLogs(false);
}

// Main loop
void Application::run() {

    // Start of main loop, only ends when the window is set to
    while (!window->shouldClose()) {

        START_PROFILE("Run Loop"); // Setup timer for run loop

        // Update input & timer services
        Input::poll();
        Timer::update();

        // TODO: temp
        window->preFrame();

        // Dispatch the layer stack
        layerStack.dispatchStack();

        // TODO: temp
        window->postFrame();

        // Log profiling data
        Logger::info("PROFILE", "FPS: " + std::to_string(Timer::getFPS()),
                     LogType::IN_PLACE);
        Logger::info("PROFILE",
                     "Average FPS: " + std::to_string(Timer::getAverageFPS()),
                     LogType::IN_PLACE);

        END_PROFILE("Run Loop"); // End timer for run loop

        // Output any collected logs
        Logger::outputLogs();
    }
}

// TODO: temp

void Application::setupEntities() {
    START_PROFILE("Entity Loading");

    auto *sceneManager = engineContext->getScene();
    auto &scene = sceneManager->getScene();

    // Bunny
    if (0) {

        std::vector<entt::entity> bunny =
            sceneManager->loadObjScene("assets/models/bunny.obj");

        scene.getRegistry().try_get<TransformComponent>(bunny[0])->position =
            glm::vec3(-1.0f, -1.2f, -4.0f);
    }

    // Dragon
    if (0) {
        std::vector<entt::entity> dragon =
            sceneManager->loadObjScene("assets/models/dragon.obj");

        scene.getRegistry().try_get<TransformComponent>(dragon[0])->position =
            glm::vec3(1.0f, -0.6f, -4.0f);
    }

    // Cat
    if (0) {
        std::vector<entt::entity> cat =
            sceneManager->loadObjScene("assets/models/cat.obj");

        scene.getRegistry().try_get<TransformComponent>(cat[0])->position =
            glm::vec3(0.0f, -0.6f, -4.0f);
    }

    // 🗿
    if (0) {
        std::vector<entt::entity> moai =
            sceneManager->loadObjScene("assets/models/moai.obj");

        scene.getRegistry().try_get<TransformComponent>(moai[0])->position =
            glm::vec3(0.0f, 1.3f, -4.0f);
        scene.getRegistry().try_get<TransformComponent>(moai[0])->rotation =
            glm::quat(-0.707f, 0.0f, 0.707f, 0.0f);
        scene.getRegistry().try_get<TransformComponent>(moai[0])->scale =
            glm::vec3(0.14f, 0.14f, 0.14f);
    }

    // Diffuse cube & emmisive cube
    if (0) {
        std::vector<entt::entity> cube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        scene.getRegistry().try_get<TransformComponent>(cube[0])->position =
            glm::vec3(-1.5f, 1.3f, -4.0f);
        scene.getRegistry().try_get<TransformComponent>(cube[0])->scale =
            glm::vec3(0.4f, 0.4f, 0.4f);

        std::shared_ptr<CPUMaterialData> materialCube =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(cube[0])
                    ->handle);

        materialCube->albedo = glm::vec3(0.4f, 0.2f, 0.8f);
        materialCube->roughness = 0.7;
        materialCube->metallic = 1.0;

        std::vector<entt::entity> emissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        scene.getRegistry()
            .try_get<TransformComponent>(emissiveCube[0])
            ->position = glm::vec3(1.3f, 8.4f, -0.2f);
        scene.getRegistry()
            .try_get<TransformComponent>(emissiveCube[0])
            ->scale = glm::vec3(0.2f);

        std::shared_ptr<CPUMaterialData> materialEmissiveCube =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(emissiveCube[0])
                    ->handle);

        materialEmissiveCube->albedo = glm::vec3(0.0f);
        materialEmissiveCube->emissive = glm::vec3(2.0);
    }

    // Gay Room (me)
    if (0) {
        std::vector<entt::entity> room =
            sceneManager->loadObjScene("assets/models/gayRoom.obj");

        // for (entt::entity entity : room) {
        //     scene.getComponent<MaterialComponent>(entity).emmissive *=
        //         glm::vec3(1.0f);
        // }
    }

    // Trans flag
    if (0) {
        std::vector<entt::entity> trans =
            sceneManager->loadObjScene("assets/models/trans.obj");

        for (entt::entity entity : trans) {
            scene.getRegistry().try_get<TransformComponent>(entity)->position +=
                glm::vec3(-1.0f, 0.0f, 0.0f);
        }
    }

    // Franch flag
    if (0) {
        std::vector<entt::entity> french =
            sceneManager->loadObjScene("assets/models/french.obj");

        for (entt::entity entity : french) {
            scene.getRegistry().try_get<TransformComponent>(entity)->position +=
                glm::vec3(8.0f, 0.0f, 2.0f);
        }
    }

    // THE BACKROOMS????
    if (0) {
        // https: //
        // sketchfab.com/3d-models/backrooms-v2-level-0-made-by-me-in-blender-91d707acdfce4d5d940f7cb8c25c6e31#download
        std::vector<entt::entity> backrooms =
            sceneManager->loadObjScene("assets/models/backrooms_level1.obj");

        std::vector<entt::entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        // emmissiveCube[0].getComponent<TransformComponent>().position =
        //     glm::vec3(80.2f, 6.5f, -116.7f);
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->position = glm::vec3(47.1f, 8.1f, -50.8f);
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->scale = glm::vec3(0.2f);

        std::shared_ptr<CPUMaterialData> material =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(emmissiveCube[0])
                    ->handle);

        material->albedo = glm::vec3(0.0f);
        material->emissive = glm::vec3(2.0);

        // 0.5/0.3 exposure
        // 0.02/0.015 fog density; 0.5, 0.4, 0.1 fog color
        // 1.5 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // Breakfast room
    if (0) {
        std::vector<entt::entity> room =
            sceneManager->loadObjScene("assets/models/breakfast_room.obj");

        std::vector<entt::entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->position = glm::vec3(-2.2f, 3.8f, -1.9f);
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->scale = glm::vec3(0.05f);

        std::shared_ptr<CPUMaterialData> material =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(emmissiveCube[0])
                    ->handle);

        material->albedo = glm::vec3(0.0f);
        material->emissive = glm::vec3(0.8f, 0.7f, 0.2f);

        // 0.7 exposure
        // 0.02 fog density; 0.3, 0.3, 0.3 fog color
        // No vinette
        // 0.1, 0.3, 0.5 clear color
    }

    // Sponza
    if (1) {
        std::vector<entt::entity> sponza =
            sceneManager->loadObjScene("assets/models/sponza.obj");

        std::vector<entt::entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");
        //
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->position = glm::vec3(1.3f, 8.4f, -0.2f);
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->scale = glm::vec3(0.05f);

        std::shared_ptr<CPUMaterialData> material =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(emmissiveCube[0])
                    ->handle);

        material->albedo = glm::vec3(0.0f);
        material->emissive = glm::vec3(2.0);

        // 0.15 exposure
        // 0.02 fog density; 0.5, 0.4, 0.1 fog color
        // 1.5 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // Lost empire (Minecraft)
    if (0) {
        std::vector<entt::entity> lostEmpire =
            sceneManager->loadObjScene("assets/models/lost_empire.obj");

        std::vector<entt::entity> emmissiveCube =
            sceneManager->loadObjScene("assets/models/cube.obj");

        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->position = glm::vec3(-10.7f, 22.4f, 3.1f);
        scene.getRegistry()
            .try_get<TransformComponent>(emmissiveCube[0])
            ->scale = glm::vec3(0.05f);

        std::shared_ptr<CPUMaterialData> material =
            engineContext->getAsset()->getMaterial(
                scene.getRegistry()
                    .try_get<MaterialComponent>(emmissiveCube[0])
                    ->handle);

        material->albedo = glm::vec3(0.0f);
        material->emissive = glm::vec3(2.0);

        // 0.7 exposure
        // 0.02 fog density; 0.7, 0.85, 0.98 fog color
        // 1.74 vinette radius; 0.9 softness
        // 0.1, 0.1, 0.1 clear color
    }

    // oiiaioooooiai (broken texture)
    if (0) {
        std::vector<entt::entity> oiiaioooooiai =
            sceneManager->loadObjScene("assets/models/oiiaioooooiai.obj");
    }

    END_PROFILE_STACKED_LOG("Entity Loading");
}

} // namespace Engine
