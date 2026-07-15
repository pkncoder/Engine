#pragma once

#include "../core/EngineContext.h"
#include "CPUStructs.h"

#include <glad/glad.h>

#include <string>
#include <unordered_map>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    AssetManager(EngineContext &engineContext);
    ~AssetManager() = default;

    // Getters and loaders
    const CPUModelData *loadModel(const std::string &filepath);
    const CPUMaterialData *getMaterial(const std::string &materialName);
    GLuint loadTexture(const std::string &filepath, bool &bumpTexture);

  private:
    // Cache materials to be fetched later
    void cacheMaterials(const std::string &filepath);

  private:
    EngineContext &engineContext;

    // Asset caches
    std::unordered_map<std::string, CPUModelData> modelCache;
    std::unordered_map<std::string, CPUMaterialData> materialCache;
    std::unordered_map<std::string, GLuint> textureCache;
};

} // namespace Engine
