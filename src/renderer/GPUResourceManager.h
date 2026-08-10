#pragma once

#include "../core/EngineContext.h"
#include "../resources/AssetManager.h"
#include "../resources/CPUStructs.h"
#include "GPUStructs.h"

#include <glad/glad.h>

#include <unordered_map>

namespace Engine {

class GPUResourceManager {
  public:
    static inline void init(AssetManager *_assetManager) {
        assetManager = _assetManager;
    }

    // Tries to return resource from cache, uploads it to VRAM if not cached
    static GPUMesh *getOrUploadMesh(const AssetHandle assetHandle);
    static GPUTexture *getOrUploadTexture(const AssetHandle assetHandle);

  private:
    // Internal function for uploading resources to VRAM
    static GPUMesh uploadMesh(const CPUMeshData &mesh);
    static GPUTexture uploadTexture(const CPUTextureData &texture);

  private:
    // Injected asset manager
    static inline AssetManager *assetManager = nullptr;

    // Caches
    static inline std::unordered_map<AssetHandle, GPUMesh> gpuMeshCache;
    static inline std::unordered_map<AssetHandle, GPUTexture> gpuTextureCache;
};

} // namespace Engine
