#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"

#include <memory>
#include <string>

namespace Engine {
class TextureLoader {
  public:
    // Used to inject the asset manager
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    }

    // Load and return texture data from a file
    static std::shared_ptr<CPUTextureData>
    loadTexture(const std::string &filepath);

  private:
    // Injected asset manager
    inline static AssetManager *assetManager = nullptr;
};
} // namespace Engine
