#pragma once

#include "AssetManager.h"
#include "CPUStructs.h"
#include <glad/glad.h>

#include <memory>
#include <string>

namespace Engine {
class TextureLoader {
  public:
    static inline void init(AssetManager *assetManagerPtr) {
        assetManager = assetManagerPtr;
    }
    // Loads a texture from file and return the OpenGL Texture ID
    static std::shared_ptr<CPUTextureData>
    loadTexture(const std::string &filepath);

  private:
    inline static AssetManager *assetManager = nullptr;
};
} // namespace Engine
