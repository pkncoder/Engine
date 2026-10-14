#pragma once

#include "../enums/BufferType.h"
#include "../enums/BufferUsage.h"
#include "../enums/ShaderType.h"
#include "../gpuResources/GPUBuffer.h"
#include "../gpuResources/GPURenderTarget.h"
#include "../gpuResources/GPUResourceManager.h"
#include "../shaders/GPUShader.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

using ResourceHandle = std::uint64_t;

namespace Engine {

struct RenderPass {

  public:
    // Public functions set by lambdas
    std::function<void()> setup;
    std::function<void()> execute;

  public:
    // Identifier for UI
    std::string name;

    ResourceHandle program;

  public:
    // Shaders
    ResourceHandle addProgram(const std::vector<ResourceHandle> shaders);
    ResourceHandle addShader(const std::string &shaderPath, ShaderType type);

    // Storage
    ResourceHandle addBuffer(const std::string &name, const BufferType type,
                             const BufferUsage usage,
                             const GPUBufferDescriptor desc);

    // Textures
    ResourceHandle addRenderTarget(const std::string &name,
                                   const std::vector<GLenum> &formats,
                                   const RenderTargetDescriptor desc);
    // ResourceHandle addTexture(const ResourceHandle textureHandle);
    GPUTexture *addTexture(const ResourceHandle textureHandle);

  private:
    // Program

    // Links to resource handles
    std::vector<ResourceHandle> inputs;
    std::vector<ResourceHandle> outputs;
};

} // namespace Engine
