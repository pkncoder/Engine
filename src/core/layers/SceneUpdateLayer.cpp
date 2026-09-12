#include "SceneUpdateLayer.h"

#include "../../scene/SceneManager.h"
#include "../../scene/components/CameraComponent.h"
#include "../../scene/components/TransformComponent.h"
#include "../../services/Input.h"
#include "../../services/Logger.h"
#include "../events/KeyEvents.h"

#include <GLFW/glfw3.h>

#include <entt/entity/fwd.hpp>
#include <memory>

namespace Engine {

// Save the reference to the engineContext
SceneUpdateLayer::SceneUpdateLayer(EngineContext &engineContext)
    : engineContext(engineContext) {}

void SceneUpdateLayer::onAttach(EngineState &engineState) {
    // // TODO: make an init for sceneManager
    // sceneManager.init();
}

// Polling
void SceneUpdateLayer::onUpdate(EngineState &engineState) {

    // Get the scene manager for use
    SceneManager *sceneManager = engineContext.getScene();
    sceneManager->update();

    entt::entity camera = sceneManager->getScene().activeCameraID;

    CameraSystem cameraSystem = CameraSystem();

    float cameraSpeedModifier =
        Input::isKeyPressed(Key::RIGHT_SHIFT) ? 0.05 : 1.0;

    cameraSystem.update(sceneManager->getScene().getRegistry());
    //
    // // Poll for movement
    // if (Input::isKeyPressed(Key::W)) { // Foward
    //     camera.processMovement(engineState, FORWARD, cameraSpeedModifier);
    // }
    // if (Input::isKeyPressed(Key::S)) { // Backward
    //     camera.processMovement(engineState, BACKWARD, cameraSpeedModifier);
    // }
    // if (Input::isKeyPressed(Key::A)) { // Left
    //     camera.processMovement(engineState, LEFT, cameraSpeedModifier);
    // }
    // if (Input::isKeyPressed(Key::D)) { // Rigt
    //     camera.processMovement(engineState, RIGHT, cameraSpeedModifier);
    // }
    // if (Input::isKeyPressed(Key::SPACE)) { // Up
    //     camera.processMovement(engineState, UP, cameraSpeedModifier);
    // }
    // if (Input::isKeyPressed(Key::LEFT_SHIFT)) { // Down
    //     camera.processMovement(engineState, DOWN, cameraSpeedModifier);
    // }
    //
    // // Mouse movement
    // if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    //     glm::vec2 mouseDelta = Input::getMouseDelta();
    //
    //     // Swap y to fit into glfw & glad (0,0) differnces
    //     // TODO: Fixable?
    //     camera.processLookingDirectionMovement(engineState, mouseDelta.x,
    //                                            -mouseDelta.y);
    // }

    // Log the camera position into the dashboard
    // TODO: temp
    Logger::info(
        "CAMERA",
        "X: " + std::to_string(engineState.scene.camera.position.x) +
            "Y: " + std::to_string(engineState.scene.camera.position.y) +
            "Z: " + std::to_string(engineState.scene.camera.position.z),
        LogType::IN_PLACE);
}

// Deconstructors
// TODO: add
void SceneUpdateLayer::onDetach() {}

// Event dispatching
void SceneUpdateLayer::onEvent(std::shared_ptr<IEvent> event,
                               EngineState &state) {
    // TODO: Move movement here?
}

} // namespace Engine
