#pragma once
#include "AssetTypes.h"
#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Returns true if successful, populates outMesh
    static bool loadOBJ(const std::string &filepath, MeshData &outMesh);
};

} // namespace Engine
