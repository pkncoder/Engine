#pragma once

#include "CPUStructs.h"

#include <string>
#include <vector>

namespace Engine {

class MaterialLoader {
  public:
    // Load a .mtl file; returns an array of materials + texture paths
    static std::vector<CPUMaterialData> loadMTL(const std::string &filepath);
};

} // namespace Engine
