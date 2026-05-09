#include "AssetManager.h"

#include "../services/Logger.h"
#include "ModelLoader.h"

#include <string>

namespace Engine {

// AssetManager initilaization
void AssetManager::init() {
    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a mesh + optional for error catching
const MeshData *AssetManager::loadMesh(const std::string &filepath) {

    // 1. Check if we already loaded this OBJ
    if (cpuMeshCache.find(filepath) != cpuMeshCache.end()) {
        Logger::info("ASSET", "Returning cached mesh: " + filepath);
        return &cpuMeshCache[filepath]; // Instantly return the cached
                                        // data
    }

    Logger::line();
    Logger::info("ASSET", "Starting to load mesh: " + filepath);

    // 2. If not, load it via tinyobjloader
    MeshData newMesh;
    if (ModelLoader::loadOBJ(filepath, newMesh)) {

        Logger::info("ASSET", "Successfully loaded mesh: " + filepath + " (" +
                                  std::to_string(newMesh.vertices.size()) +
                                  " vertices)");
        Logger::space();

        cpuMeshCache[filepath] = newMesh; // Save to cache
        return &cpuMeshCache[filepath];
    }

    Logger::error("ASSET", "AssetManager Failed to load mesh: ");

    return nullptr; // Failed to load
}
} // namespace Engine
