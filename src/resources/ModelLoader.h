#pragma once

#include "CPUStructs.h"

#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Load a mesh, return true for sucess, false for error
    static bool loadOBJ(const std::string &filepath, CPUMeshData &outMesh);
};

} // namespace Engine
