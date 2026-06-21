#pragma once

#include "CPUStructs.h"

#include <glad/glad.h>

#include <string>
#include <unordered_map>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    static void init();

    // Getters and loaders
    static const CPUModelData *loadModel(const std::string &filepath);
    static const CPUMaterialData *getMaterial(const std::string &materialName);
    static GLuint loadTexture(const std::string &filepath, bool &bumpTexture);

  private:
    // Asset caches
    static inline std::unordered_map<std::string, CPUModelData> modelCache;
    static inline std::unordered_map<std::string, CPUMaterialData>
        materialCache;
    static inline std::unordered_map<std::string, GLuint> textureCache;

    // Cache materials to be fetched later
    static const void cacheMaterials(const std::string &filepath);
};

} // namespace Engine
