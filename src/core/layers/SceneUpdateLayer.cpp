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

    CameraSystem cameraSystem = CameraSystem();
    cameraSystem.update(sceneManager->getScene().getRegistry());
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
