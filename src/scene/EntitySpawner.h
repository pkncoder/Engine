#pragma once

#include "Entity.h"
#include "Scene.h"

#include <string>

namespace Engine {

class EntitySpawner {
  public:
    // Spawn new entites from a .obj model file
    static std::vector<Entity> spawnObjEntity(Scene &scene,
                                              const std::string &filepath);
};

} // namespace Engine
