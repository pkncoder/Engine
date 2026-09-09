#include "SceneManager.h"

#include "PrefabSpawner.h"

namespace Engine {

SceneManager::SceneManager(EngineContext &engineContext,
                           EngineState &engineState)
    : engineContext(engineContext) {
    camera = Camera(engineState.scene.camera);
}

std::vector<entt::entity>
SceneManager::loadObjScene(const std::string filepath) {
    return PrefabSpawner::spawnObjEntity(scene, *engineContext.getAsset(),
                                         filepath);
}

} // namespace Engine
