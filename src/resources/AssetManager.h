#pragma once

#include "CPUStructs.h"

#include <glad/glad.h>

#include <string>
#include <unordered_map>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    static void init();

    // Load and return a std::optional in case of failed loading
    static const CPUMeshData *loadMesh(const std::string &filepath);
    static GLuint loadTexture(const std::string &texture);

    static const CPUMaterialData *getMaterial(const std::string &materialName);

  private:
    static inline std::unordered_map<std::string, CPUMeshData> meshCache;
    static inline std::unordered_map<std::string, CPUMaterialData>
        materialCache;
    static inline std::unordered_map<std::string, GLuint> textureCache;

    static const void cacheMaterials(const std::string &filepath,
                                     CPUMeshData *tempMeshData);
};

} // namespace Engine
