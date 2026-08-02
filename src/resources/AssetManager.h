#pragma once

#include "../core/EngineContext.h"
#include "CPUStructs.h"

#include <string>
#include <unordered_map>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    AssetManager(EngineContext &engineContext);
    ~AssetManager() = default;

    // Asset loaders
    AssetHandle loadModel(const std::string &filepath);
    std::unordered_map<std::string, AssetHandle>
    loadMaterialLibrary(const std::string &filepath);
    AssetHandle loadTexture(const std::string &filepath);

    // Getters for assets; can return INVALID_MATERIAL_HANDLE (0)
    std::shared_ptr<CPUModelData> getModel(const AssetHandle handle) const;
    std::shared_ptr<CPUMeshData> getMesh(const AssetHandle handle) const;
    std::shared_ptr<CPUMaterialData>
    getMaterial(const AssetHandle handle) const;
    std::shared_ptr<CPUTextureData> getTexture(const AssetHandle handle) const;

  private:
    EngineContext &engineContext;

    // Asset caches
    std::unordered_map<AssetHandle, std::shared_ptr<CPUModelData>> modelCache;
    std::unordered_map<AssetHandle, std::shared_ptr<CPUMeshData>> meshCache;
    std::unordered_map<AssetHandle, std::shared_ptr<CPUMaterialData>>
        materialCache;
    std::unordered_map<AssetHandle, std::shared_ptr<CPUTextureData>>
        textureCache;

    // Filepath cache buffer
    std::unordered_map<std::string, AssetHandle> cachedPaths;
};

} // namespace Engine
