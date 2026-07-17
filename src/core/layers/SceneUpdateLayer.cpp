#include "SceneUpdateLayer.h"
#include "../../scene/SceneManager.h"
#include "../../services/Input.h"
#include "../events/KeyEvents.h"

#include <GLFW/glfw3.h>
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
    float cameraSpeedModifier =
        Input::isKeyPressed(Key::RIGHT_SHIFT) ? 0.05 : 1.0;

    // Poll for movement
    if (Input::isKeyPressed(Key::W)) { // Foward
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, FORWARD, cameraSpeedModifier);
    }
    if (Input::isKeyPressed(Key::S)) { // Backward
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, BACKWARD, cameraSpeedModifier);
    }
    if (Input::isKeyPressed(Key::A)) { // Left
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, LEFT, cameraSpeedModifier);
    }
    if (Input::isKeyPressed(Key::D)) { // Rigt
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, RIGHT, cameraSpeedModifier);
    }
    if (Input::isKeyPressed(Key::SPACE)) { // Up
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, UP, cameraSpeedModifier);
    }
    if (Input::isKeyPressed(Key::LEFT_SHIFT)) { // Down
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, DOWN, cameraSpeedModifier);
    }

    // Mouse movement
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        Camera &camera = sceneManager->getCamera();
        glm::vec2 mouseDelta = Input::getMouseDelta();

        // Swap y to fit into glfw & glad (0,0) differnces
        // TODO: Fixable?
        camera.processLookingDirectionMovement(engineState, mouseDelta.x,
                                               -mouseDelta.y);
    }
}

// Deconstructors
// TODO: add
void SceneUpdateLayer::onDetach() {}
// Event dispatching
void SceneUpdateLayer::onEvent(std::shared_ptr<IEvent> event,
                               EngineState &state) {}

} // namespace Engine
