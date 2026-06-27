#include "Application.h"

#include "../renderer/RendererManager.h"
#include "../resources/AssetManager.h"
#include "../scene/Entity.h"
#include "../scene/EntitySpawner.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Input.h"
#include "../services/Logger.h"
#include "../services/Timer.h"

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>

namespace Engine {

// Constructor & Deconstructor
Application::Application() {}
Application::~Application() {
    Logger::shutdown();
    RendererManager::shutdown();
}

// Init the window, camera, etc.
void Application::init() {

    // Initialize the logger and set no_pending_logs
    Logger::init();
    Logger::setNoPendingLogs(true);

    // Create the window
    window = std::make_unique<Window>();

    // Init the input service
    Input::init(window->getNativeWindow());

    // Init the timer service
    Timer::init();

    // Init the camera at a starting pos
    camera = Camera();

    // Initialize the asset manager
    AssetManager::init();

    // Initialize the scene
    activeScene = Scene();

    RendererManager::init();

    // Register the scene components and load the scene
    // TODO: temp
    this->registerSceneComponents(activeScene);
    this->setupEntities(activeScene);

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
        RendererManager::render(*window, activeScene, camera);

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

    if (Input::isKeyJustPressed(GLFW_KEY_R)) {

        switch (RendererManager::getRenderChoice()) {
        case RenderChoice::RASTERIZER:
            RendererManager::swapActiveRenderer(RenderChoice::PATH_TRACER);
            break;
        case RenderChoice::PATH_TRACER:
            RendererManager::swapActiveRenderer(RenderChoice::RASTERIZER);
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
            activeScene, "assets/models/bunny.obj");

        bunny[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.0f, -1.2f, -4.0f);
    }

    // Dragon
    if (0) {
        std::vector<Entity> dragon = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/dragon.obj");

        dragon[0].getComponent<TransformComponent>().position =
            glm::vec3(1.0f, -0.6f, -4.0f);
    }

    // Cat
    if (0) {
        std::vector<Entity> cat =
            EntitySpawner::spawnObjEntity(activeScene, "assets/models/cat.obj");

        cat[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, -0.6f, -4.0f);
    }

    // 🗿
    if (0) {
        std::vector<Entity> moai = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/moai.obj");

        moai[0].getComponent<TransformComponent>().position =
            glm::vec3(0.0f, 1.3f, -4.0f);
        moai[0].getComponent<TransformComponent>().rotation =
            glm::quat(-0.707f, 0.0f, 0.707f, 0.0f);
        moai[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.14f, 0.14f, 0.14f);
    }

    // Diffuse cube & emmisive cube
    if (1) {
        std::vector<Entity> cube = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/cube.obj");

        cube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.4f, 0.2f, 0.8f);
        cube[0].getComponent<MaterialComponent>().roughness = 0.7;
        cube[0].getComponent<MaterialComponent>().metallic = 1.0;

        cube[0].getComponent<TransformComponent>().position =
            glm::vec3(-1.5f, 1.3f, -4.0f);
        cube[0].getComponent<TransformComponent>().scale =
            glm::vec3(0.4f, 0.4f, 0.4f);

        std::vector<Entity> emmissiveCube = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/cube.obj");

        emmissiveCube[0].getComponent<TransformComponent>().position =
            glm::vec3(4.0f, 0.0f, 2.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().albedo =
            glm::vec3(0.0f);
        emmissiveCube[0].getComponent<MaterialComponent>().emmissive =
            glm::vec3(1.0);
    }

    // Trans flag
    if (1) {
        std::vector<Entity> trans = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/trans.obj");

        for (Entity entity : trans) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(-1.0f, 0.0f, 0.0f);
        }
    }

    // Franch flag
    if (1) {
        std::vector<Entity> french = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/french.obj");

        for (Entity entity : french) {
            entity.getComponent<TransformComponent>().position +=
                glm::vec3(8.0f, 0.0f, 2.0f);
        }
    }

    // THE BACKROOMS????
    if (0) {
        // https://sketchfab.com/3d-models/backrooms-v2-level-0-made-by-me-in-blender-91d707acdfce4d5d940f7cb8c25c6e31#download
        std::vector<Entity> backrooms = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/backrooms_level1.obj");
    }

    // Breakfast room
    if (0) {
        std::vector<Entity> room = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/breakfast_room.obj");
    }

    // Sponza
    if (0) {
        std::vector<Entity> sponza = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/sponza.obj");
    }

    // Lost empire (Minecraft)
    if (0) {
        std::vector<Entity> lostEmpire = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/lost_empire.obj");
    }

    // oiiaioooooiai (broken texture)
    if (0) {
        std::vector<Entity> oiiaioooooiai = EntitySpawner::spawnObjEntity(
            activeScene, "assets/models/oiiaioooooiai.obj");
    }

    END_PROFILE_STACKED_LOG("Entity Loading");
}

} // namespace Engine
