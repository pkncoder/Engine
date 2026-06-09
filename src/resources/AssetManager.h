#pragma once

#include "CPUStructs.h"

#include <string>
#include <unordered_map>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    static void init();

    // Load and return a std::optional in case of failed loading
    static const CPUMeshData *loadMesh(const std::string &filepath);
    static const void loadMaterialBank(const std::string &filepath);

    static const CPUMaterialData *getMaterial(const std::string &materialName);

  private:
    static inline std::unordered_map<std::string, CPUMeshData> cpuMeshCache;
    static inline std::unordered_map<std::string, CPUMaterialData> matMeshCache;
};

} // namespace Engine
