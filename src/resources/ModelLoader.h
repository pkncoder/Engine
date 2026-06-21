#pragma once

#include "CPUStructs.h"

#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Load a .obj file, setting the .mtl filepath along the way
    static CPUModelData loadOBJ(const std::string &filepath);
};

} // namespace Engine
