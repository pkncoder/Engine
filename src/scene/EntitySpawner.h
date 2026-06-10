#pragma once

#include "Entity.h"
#include "Scene.h"

#include <string>

namespace Engine {

class EntitySpawner {
  public:
    // Spawn a new entity based on a .obj file
    static Entity spawnObjEntity(Scene &scene, const std::string &filepath);
};

} // namespace Engine
