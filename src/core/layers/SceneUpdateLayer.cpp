#include "SceneUpdateLayer.h"

#include <memory>

namespace Engine {

SceneUpdateLayer::SceneUpdateLayer(std::shared_ptr<SceneManager> sceneManager)
    : sceneManager(sceneManager) {}

void SceneUpdateLayer::onAttach(EngineState &engineState) {
    // // TODO: make an init for sceneManager
    // sceneManager.init();
}

} // namespace Engine
