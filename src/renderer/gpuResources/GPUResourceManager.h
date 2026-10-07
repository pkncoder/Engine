#pragma once

#include "../../core/EngineContext.h"
#include "../../resources/AssetManager.h"
#include "../../resources/CPUStructs.h"
#include "../GPUStructs.h"
#include "../shaders/GPUProgram.h"
#include "../shaders/GPUShader.h"
#include "GPUBuffer.h"
#include "GPURenderTarget.h"
#include "GPUTexture.h"
#include "IGPUResource.h"

#include <glad/glad.h>

#include <unordered_map>

namespace Engine {

class GPUResourceManager {
  public:
    static inline void init(AssetManager *_assetManager) {
        assetManager = _assetManager;
    }

    // Shaders
    static ResourceHandle addProgram(const std::string &name,
                                     const std::vector<ResourceHandle> shaders);
    static ResourceHandle addShader(const std::string &shaderPath,
                                    ShaderType type);

    // Buffers
    static ResourceHandle addBuffer(const std::string &name,
                                    const BufferType type,
                                    const BufferUsage usage,
                                    const GPUBufferDescriptor desc);

    // Render targets
    static ResourceHandle addRenderTarget(const std::string &name,
                                          const std::vector<GLenum> &formats,
                                          const RenderTargetDescriptor desc);

    static IGPUResource *getResource(const ResourceHandle resourceHandle);

    static GPUMesh *getOrUploadMesh(const ResourceHandle meshHandle);
    static GPUTexture *getOrUploadTexture(const ResourceHandle textureHandle);

  private:
    // Internal function for uploading resources to VRAM
    static GPUMesh uploadMesh(const CPUMeshData &mesh);
    static GPUTexture uploadTexture(const CPUTextureData &texture);

  private:
    // Injected asset manager
    static inline AssetManager *assetManager = nullptr;

    // Caches
    static inline std::unordered_map<AssetHandle, std::shared_ptr<IGPUResource>>
        resourceCache;

    // Shader code caches
    static inline std::unordered_map<std::string, ResourceHandle> programCache;
    static inline std::unordered_map<std::string, ResourceHandle> shaderCache;

    // Buffer caches
    static inline std::unordered_map<std::string, ResourceHandle> bufferCache;

    // Texture caches
    static inline std::unordered_map<std::string, ResourceHandle>
        renderTargetCache;

    static inline std::unordered_map<ResourceHandle, GPUMesh> gpuMeshCache;
    static inline std::unordered_map<ResourceHandle, GPUTexture> textureCache;
};

} // namespace Engine
