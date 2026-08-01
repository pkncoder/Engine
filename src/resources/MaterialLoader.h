#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"

#include <string>

namespace Engine {

class MaterialLoader {
  public:
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    }

    // Load a .mtl file; returns an array of materials + texture paths
    static std::unordered_map<std::string, CPUMaterialData>
    loadMTL(const std::string &filepath);

  private:
    inline static AssetManager *assetManager = nullptr;
};

} // namespace Engine
