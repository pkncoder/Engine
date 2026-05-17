#pragma once

#include "CPUStructs.h"

#include <string>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    static void init();

    // Load and return a std::optional in case of failed loading
    static const CPUMeshData *loadMesh(const std::string &filepath);

  private:
    static inline std::unordered_map<std::string, CPUMeshData> cpuMeshCache;
};

} // namespace Engine
