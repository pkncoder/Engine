#pragma once

#include "CPUStructs.h"

#include <string>
#include <vector>

namespace Engine {

class MaterialLoader {
  public:
    // Load a mesh, return true for sucess, false for error
    static std::vector<CPUMaterialData> loadMTL(const std::string &filepath);
};

} // namespace Engine
