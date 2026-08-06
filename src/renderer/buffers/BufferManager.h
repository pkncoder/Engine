#pragma once

#include "../../resources/CPUStructs.h"
#include "../../scene/components/MeshComponent.h"
#include "UBO.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>

namespace Engine {

// Persistant GPU buffer, defaults to a SSBO
// TODO: Decide on moving the implemtation to the BufferManager

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
