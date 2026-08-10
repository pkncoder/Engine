#include "GPUResourceManager.h"

#include "../services/Logger.h"

namespace Engine {

GPUMesh *GPUResourceManager::getOrUploadMesh(const AssetHandle handle) {

    // Find the mesh in the cache and return a refrence
    auto ittr = gpuMeshCache.find(handle);
    if (ittr != gpuMeshCache.end()) {
        return &ittr->second;
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

GPUTexture *GPUResourceManager::getOrUploadTexture(const AssetHandle handle) {

    // Find the mesh in the cache and return a refrence
    auto ittr = gpuTextureCache.find(handle);
    if (ittr != gpuTextureCache.end()) {
        return &ittr->second;
    }

    // If it is not in the cache, get the texture data from asset manager
    auto cpuTexture = assetManager->getTexture(handle);
    if (!cpuTexture)
        return nullptr; // Invalid handle!

    // Upload the texture to VRAM
    GPUTexture newGPUTexture = uploadTexture(*cpuTexture);

    // Cache the texture and return a refrence
    gpuTextureCache[handle] = newGPUTexture;
    return &gpuTextureCache[handle];
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
