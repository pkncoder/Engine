#include "SceneManager.h"

#include "EntitySpawner.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Engine {

SceneManager::SceneManager(EngineContext &engineContext,
                           EngineState &engineState)
    : engineContext(engineContext) {
    scene = Scene();
    camera = Camera(engineState.scene.camera);

    // Register components
    scene.registerComponent<TransformComponent>();
    scene.registerComponent<MeshComponent>();
    scene.registerComponent<MaterialComponent>();
}

std::vector<Entity> SceneManager::loadObjScene(const std::string filepath) {
    return EntitySpawner::spawnObjEntity(scene, filepath, engineContext);
}

} // namespace Engine
