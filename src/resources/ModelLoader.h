#pragma once

#include "CPUStructs.h"

#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Load a mesh, return true for sucess, false for error
    static CPUModelData loadOBJ(const std::string &filepath);
};

} // namespace Engine
