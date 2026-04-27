#pragma once

#include "AssetTypes.h"

#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Load a mesh, return true for sucess, false for error
    static bool loadOBJ(const std::string &filepath, MeshData &outMesh);
};

} // namespace Engine
