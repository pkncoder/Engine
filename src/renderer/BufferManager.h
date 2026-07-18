#pragma once

#include "../resources/CPUStructs.h"
#include "../scene/components/MeshComponent.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef> // for size_t

namespace Engine {

// UBO information
struct GPUUniformBuffer {

  public:
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

// Persistant GPU buffer, defaults to a SSBO
// TODO: Decide on moving the implemtation to the BufferManager
struct PersistentBuffer {

  public:
    // Alocate VRAM and cache data to CPU
    void setup(const GLenum bufferTarget, const size_t bufferSize);

    // Update saved data, including in VRAM
    void update(const void *data, const size_t updateSize);

    // Data cleanup
    void shutdown();

  public:
    // ID & bindings
    GLuint id = 1;
    GLuint bindingIndex = 0;
    GLenum target = GL_SHADER_STORAGE_BUFFER; // TODO: Need this? Maybe use this
                                              // to reporpose

    // Pointer to data
    void *mappedPtr = nullptr;

    // Data size
    size_t size = 0;
    size_t elementSize = 0;
};

class BufferManager {
  public:
    // Upload a mesh to the GPU - resturns bindings
    static MeshComponent uploadMesh(const CPUMeshData &meshData);

    // Create a new UBO keyed by name - allocates the block area
    static void createUBO(const std::string &uboName,
                          const uint32_t bindingPoint,
                          const size_t totalBlockSize);

    // Reflects against compiled GLSL code to cache exact byte locations
    static void mapUBOLayout(const std::string &uboName,
                             const uint32_t programID,
                             const std::string &blockName,
                             const std::vector<std::string> &uniformNames);

    // Update a uniform
    static void setUBOValue(const std::string &uboName,
                            const std::string &uniformName, const void *data,
                            const size_t dataSize);

    // Flushes the entire CPU memory region to VRAM at once
    // TODO: Do this in windows
    static void pushUBO(const std::string &uboName);

    // UBO cleanup handler
    static void shutdownUBORegistry();

  private:
    // Mesh and UBO caches
    static inline std::unordered_map<std::string, MeshComponent> gpuMeshCache;
    static inline std::unordered_map<std::string, GPUUniformBuffer> uboRegistry;
};

} // namespace Engine
