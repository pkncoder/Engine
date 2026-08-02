#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"

#include <string>

namespace Engine {

class ModelLoader {
  public:
    // Inject asset manager
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    };

    // Load a .obj file returning mesh data; Does load mtl file along the way
    static std::vector<CPUMeshData> loadOBJ(const std::string &filepath);

  private:
    // Injected asset manager
    inline static AssetManager *assetManager = nullptr;
};

} // namespace Engine
