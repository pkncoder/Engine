#pragma once

#include "../core/EngineContext.h"
#include "../resources/AssetManager.h"
#include "ECS.h"
#include "Scene.h"

#include <string>

namespace Engine {

class PrefabSpawner {
  public:
    // Spawn new entites from a .obj model file
    static std::vector<EntityID> spawnObjEntity(Scene &scene,
                                                AssetManager &assetManager,
                                                const std::string &filepath);
};

} // namespace Engine
