#include "GPUResourceManager.h"

#include "../../services/Logger.h"
#include "../../services/UUIDGenerator.h"
#include "../buffers/BufferManager.h"
#include "GPUBuffer.h"

#include <memory>

namespace Engine {

// TODO: temp using string
ResourceHandle GPUResourceManager::addProgram(
    const std::string &name, const std::vector<ResourceHandle> shaderHandles) {

    auto ittr = programCache.find(name);
    if (ittr != programCache.end()) {
        return resourceCache.find(ittr->second)->first;
    }

    GPUProgram program;

    for (auto &shaderHandle : shaderHandles) {
        // TODO: Do a safety
        std::shared_ptr<GPUShader> shader =
            std::static_pointer_cast<GPUShader>(resourceCache[shaderHandle]);
        program.attachShader(*shader);
    }

    program.link();

    ResourceHandle newResourceHandle = UUIDGenerator::generate();
    programCache[name] = newResourceHandle;
    resourceCache[newResourceHandle] = std::make_shared<GPUProgram>(program);

    return newResourceHandle;
}

ResourceHandle GPUResourceManager::addShader(const std::string &shaderPath,
                                             ShaderType type) {
    auto ittr = shaderCache.find(shaderPath);
    if (ittr != shaderCache.end()) {
        return resourceCache.find(ittr->second)->first;
    }

    // TODO: change the gpu shader constructor to std::string
    GPUShader shader = GPUShader(shaderPath.c_str(), type);

    ResourceHandle newResourceHandle = UUIDGenerator::generate();
    shaderCache[shaderPath] = newResourceHandle;
    resourceCache[newResourceHandle] = std::make_shared<GPUShader>(shader);

    return newResourceHandle;
}

ResourceHandle GPUResourceManager::addBuffer(const std::string &name,
                                             const BufferType type,
                                             const BufferUsage usage,
                                             const GPUBufferDescriptor desc) {
    ResourceHandle bufferHandle;
    bufferHandle = BufferManager::createBuffer(name, type, usage, desc.size);

    bufferCache[name] = bufferHandle;
    // resourceCache[bufferHandle] =
    // std::make_shared<GPUBuffer>(*BufferManager::getBuffer(bufferHandle));

    return bufferHandle;
}

ResourceHandle
GPUResourceManager::addRenderTarget(const std::string &name,
                                    const std::vector<GLenum> &formats,
                                    const RenderTargetDescriptor desc) {

    auto ittr = renderTargetCache.find(name);
    if (ittr != renderTargetCache.end()) {
        return resourceCache.find(ittr->second)->first;
    }

    GPURenderTarget renderTarget;
    renderTarget.name = name;
    renderTarget.handle = UUIDGenerator::generate();

    glGenFramebuffers(1, &renderTarget.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.fbo);

    std::vector<GLenum> drawBuffers;

    for (size_t i = 0; i < formats.size(); i++) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, formats[i], desc.width, desc.height, 0,
                     GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                               GL_TEXTURE_2D, texture, 0);

        renderTarget.textureIDs.push_back(texture);
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    if (!drawBuffers.empty()) {
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    }

    if (desc.createDepth) {
        glGenRenderbuffers(1, &renderTarget.depthTextureID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderTarget.depthTextureID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, desc.width,
                              desc.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, renderTarget.depthTextureID);
    } else {
        renderTarget.depthTextureID = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderTargetCache[name] = renderTarget.handle;
    resourceCache[renderTarget.handle] =
        std::make_shared<GPURenderTarget>(renderTarget);

    return renderTarget.handle;
}

GPUMesh *GPUResourceManager::getOrUploadMesh(const ResourceHandle handle) {

    // Find the mesh in the cache and return a refrence
    auto ittr = gpuMeshCache.find(handle);
    if (ittr != gpuMeshCache.end()) {
        return &ittr->second;
    }

    if (!assetManager) {
        Logger::fatal("GPU RESOURCE", "ASSET MANAGER NOT INITIALIZED");
        return nullptr;
    }

    // If it doesn't exist, get the mesh data from asset manager
    auto cpuMesh = assetManager->getMesh(handle);
    if (!cpuMesh) {
        Logger::debug("NOASSETHANDLE: " + std::to_string(handle));
        return nullptr; // Invalid handle!
    }

    // Upload the mesh to VRAM
    GPUMesh newGPUMesh = uploadMesh(*cpuMesh);

    // Cache it under the asset handle and return it
    gpuMeshCache[handle] = newGPUMesh;
    return &gpuMeshCache[handle];
}
//
GPUTexture *
GPUResourceManager::getOrUploadTexture(const ResourceHandle handle) {

    // Find the mesh in the cache and return a refrence
    auto ittr = textureCache.find(handle);
    if (ittr != textureCache.end()) {
        return &ittr->second;
    }

    // If it is not in the cache, get the texture data from asset manager
    auto cpuTexture = assetManager->getTexture(handle);
    if (!cpuTexture) {
        return nullptr; // Invalid handle!
    }

    // Upload the texture to VRAM
    GPUTexture newGPUTexture = uploadTexture(*cpuTexture);

    // Cache the texture and return a refrence
    textureCache[handle] = newGPUTexture;
    return &textureCache[handle];
}

GPUMesh GPUResourceManager::uploadMesh(const CPUMeshData &meshData) {

    // Create the new mesh component and set the index count
    GPUMesh gpuMesh;
    gpuMesh.indexCount = meshData.indices.size();

    // Generating arrays and buffers
    glGenVertexArrays(1, &gpuMesh.vao);
    glGenBuffers(1, &gpuMesh.vbo);
    glGenBuffers(1, &gpuMesh.ebo);

    // Bind the vertex array data
    glBindVertexArray(gpuMesh.vao);

    // Bind and set the data for the vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(CPUVertex),
                 meshData.vertices.data(), GL_STATIC_DRAW);

    // Bind the element buffer and data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshData.indices.size() * sizeof(uint32_t),
                 meshData.indices.data(), GL_STATIC_DRAW);

    // Vertex attribute setting (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, position));

    // Vertex attribute setting (normal)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, normal));

    // Vertex attribute setting (texture)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CPUVertex),
                          (void *)offsetof(CPUVertex, texCoords));

    // Unbind the vertex array for storage reasongs
    glBindVertexArray(0);

    // Return the mesh component previously made
    return gpuMesh;
}
//
GPUTexture
GPUResourceManager::uploadTexture(const CPUTextureData &textureData) {

    GPUTexture gpuTexture;

    // TODO: temp format
    GLenum format = (textureData.channels == 1)   ? GL_RED
                    : (textureData.channels == 4) ? GL_RGBA
                                                  : GL_RGB;

    // Get the raw pixel data
    const void *rawPixelData = textureData.pixels.data();

    // Create a texture
    glGenTextures(1, &gpuTexture.textureID);

    // Set the pixel data and generate LODs
    glBindTexture(GL_TEXTURE_2D, gpuTexture.textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, textureData.width,
                 textureData.height, 0, format, GL_UNSIGNED_BYTE, rawPixelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Return the final mesh
    return gpuTexture;
}

} // namespace Engine
