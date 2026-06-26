#pragma once

#include "../resources/CPUStructs.h"
#include "../scene/components/MeshComponent.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef> // for size_t

namespace Engine {

// UBO information
struct GPUUniformBuffer {

    // ID & bindings
    GLuint id = 0;
    GLuint bindingPoint = 0;

    // Max data size
    size_t size = 0;

    // Cache for checking duplicate data & saving data to send to the GPU
    std::vector<uint8_t> cpuCache;

    // Offsets for uniforms
    std::unordered_map<std::string, size_t> uniformOffsets;
};

// A modern, persistently mapped GPU buffer
// TODO: Decide on moving the implemtation to the BufferManager
struct PersistentBuffer {

    // ID & bindings
    GLuint id = 0;
    GLuint bindingIndex = 0;
    GLenum target = GL_SHADER_STORAGE_BUFFER; // TODO: Need this? Maybe use this
                                              // to reporpose

    // Pointer to data
    void *mappedPtr = nullptr;

    // Data size
    size_t size = 0;
    size_t elementSize = 0;

    // Allocates immutable VRAM and maps it permanently to CPU space
    void setup(GLenum bufferTarget, size_t bufferSize);

    // Fast copy data directly to VRAM
    void update(const void *data, size_t updateSize);

    // Data cleanup
    void shutdown();
};

class BufferManager {
  public:
    // Keep your existing Rasterizer methods here...
    static MeshComponent uploadMesh(const CPUMeshData &meshData);

    // --- NEW GLOBAL UBO MANAGEMENT INTERFACES ---

    // Allocates raw VRAM storage for a named UBO tracking channel
    static void createUBO(const std::string &uboName, uint32_t bindingPoint,
                          size_t totalBlockSize);

    // Reflects against compiled GLSL code to cache exact byte locations
    static void mapUBOLayout(const std::string &uboName, uint32_t programID,
                             const std::string &blockName,
                             const std::vector<std::string> &uniformNames);

    // Updates a specific variable inside the CPU cache side of the block
    static void setUBOValue(const std::string &uboName,
                            const std::string &uniformName, const void *data,
                            size_t dataSize);

    // Flushes the entire CPU memory region to VRAM at once
    static void pushUBO(const std::string &uboName);

    // Global cleanup handler
    static void shutdownRegistry();

  private:
    static inline std::unordered_map<std::string, MeshComponent> gpuMeshCache;
    static inline std::unordered_map<std::string, GPUUniformBuffer> uboRegistry;
};

} // namespace Engine
