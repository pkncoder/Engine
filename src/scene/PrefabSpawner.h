#pragma once

#include "../core/EngineContext.h"
#include "../resources/AssetManager.h"
#include "Scene.h"

#include <entt/entity/fwd.hpp>
#include <string>

namespace Engine {

class PrefabSpawner {
  public:
    // Spawn new entites from a .obj model file
    static std::vector<entt::entity>
    spawnObjEntity(Scene &scene, AssetManager &assetManager,
                   const std::string &filepath);
};

} // namespace Engine
