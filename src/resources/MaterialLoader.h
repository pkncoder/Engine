#pragma once

#include "CPUStructs.h"

#include <string>

namespace Engine {

class MaterialLoader {
  public:
    // Load a mesh, return true for sucess, false for error
    static bool loadMTL(const std::string &filepath,
                        std::vector<CPUMaterialData> &outMaterials);
};

} // namespace Engine
