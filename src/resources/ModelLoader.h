#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"

#include <memory>
#include <string>

namespace Engine {

class ModelLoader {
  public:
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    };

    // Load a .obj file, setting the .mtl filepath along the way
    static std::shared_ptr<CPUModelData> loadOBJ(const std::string &filepath);

  private:
    inline static AssetManager *assetManager = nullptr;
};

} // namespace Engine
