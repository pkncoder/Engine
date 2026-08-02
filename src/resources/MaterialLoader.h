#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"

#include <string>

namespace Engine {

class MaterialLoader {
  public:
    // Inject asset manager
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    }

    // Load a map of material name : material data; Load textures on the way
    static std::unordered_map<std::string, CPUMaterialData>
    loadMTL(const std::string &filepath);

  private:
    // Injected asset manager
    inline static AssetManager *assetManager = nullptr;
};

} // namespace Engine
