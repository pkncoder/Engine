#include "SceneUpdateLayer.h"
#include "../../scene/SceneManager.h"
#include "../../services/Input.h"
#include "../events/KeyEvents.h"

#include <GLFW/glfw3.h>
#include <memory>

namespace Engine {

SceneUpdateLayer::SceneUpdateLayer(EngineContext &engineContext)
    : engineContext(engineContext) {}

void SceneUpdateLayer::onAttach(EngineState &engineState) {
    // // TODO: make an init for sceneManager
    // sceneManager.init();
}

void SceneUpdateLayer::onUpdate(EngineState &engineState) {

    SceneManager *sceneManager = engineContext.getScene();

    if (Input::isKeyPressed(Key::W)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, FORWARD);
    }
    if (Input::isKeyPressed(Key::S)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, BACKWARD);
    }
    if (Input::isKeyPressed(Key::A)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, LEFT);
    }
    if (Input::isKeyPressed(Key::D)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, RIGHT);
    }
    if (Input::isKeyPressed(Key::SPACE)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, UP);
    }
    if (Input::isKeyPressed(Key::LEFT_SHIFT)) {
        Camera &camera = sceneManager->getCamera();

        camera.processMovement(engineState, DOWN);
    }
    if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        Camera &camera = sceneManager->getCamera();
        glm::vec2 mouseDelta = Input::getMouseDelta();

        camera.processLookingDirectionMovement(engineState, mouseDelta.x,
                                               -mouseDelta.y);
    }
}

void SceneUpdateLayer::onEvent(std::shared_ptr<IEvent> event,
                               EngineState &state) {}

} // namespace Engine
