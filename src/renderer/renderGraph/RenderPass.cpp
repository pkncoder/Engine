#include "RenderPass.h"

namespace Engine {

ResourceHandle
RenderPass::addProgram(const std::vector<ResourceHandle> shaders) {
    return GPUResourceManager::addProgram(name, shaders);
}

ResourceHandle RenderPass::addShader(const std::string &shaderPath,
                                     ShaderType type) {
    return GPUResourceManager::addShader(shaderPath, type);
}

ResourceHandle RenderPass::addBuffer(const std::string &name,
                                     const BufferType type,
                                     const BufferUsage usage,
                                     const GPUBufferDescriptor desc) {
    return GPUResourceManager::addBuffer(name, type, usage, desc);
}

ResourceHandle RenderPass::addRenderTarget(const std::string &name,
                                           const std::vector<GLenum> &formats,
                                           const RenderTargetDescriptor desc) {
    return GPUResourceManager::addRenderTarget(name, formats, desc);
}

GPUTexture *RenderPass::addTexture(const ResourceHandle textureHandle) {
    return GPUResourceManager::getOrUploadTexture(textureHandle);
}

}; // namespace Engine
