#pragma once

#include "../core/EngineContext.h"
#include "../resources/AssetManager.h"
#include "../resources/CPUStructs.h"
#include "GPUStructs.h"

#include <OpenGL/gltypes.h>
#include <c++/v1/__config>
#include <glad/glad.h>
#include <unordered_map>

namespace Engine {

class GPUResourceManager {
  public:
    static inline void init(AssetManager *_assetManager) {
        assetManager = _assetManager;
    }

    static GPUMesh *getOrUploadMesh(AssetHandle assetHandle);
    static GPUTexture *getOrUploadTexture(AssetHandle assetHandle);

  private:
    static GPUMesh uploadMesh(CPUMeshData &mesh);
    static GPUTexture uploadTexture(CPUTextureData &texture);

  private:
    static inline AssetManager *assetManager = nullptr;

    static inline std::unordered_map<AssetHandle, GPUMesh> gpuMeshCache;
    static inline std::unordered_map<AssetHandle, GPUTexture> gpuTextureCache;
};

} // namespace Engine
