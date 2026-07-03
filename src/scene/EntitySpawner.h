#pragma once

#include "../resources/AssetManager.h"
#include "Entity.h"
#include "Scene.h"

#include <string>

namespace Engine {

class EntitySpawner {
  public:
    // Spawn new entites from a .obj model file
    static std::vector<Entity> spawnObjEntity(Scene &scene,
                                              AssetManager &assetManager,
                                              const std::string &filepath);
};

} // namespace Engine
