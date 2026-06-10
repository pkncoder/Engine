#include "AssetManager.h"

#include "../services/Logger.h"
#include "CPUStructs.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"

#include <string>

namespace Engine {

// AssetManager initilaization
void AssetManager::init() {

    // Setup an initial material in the cache for objects that don't have any
    // material listed
    // TODO: Rename
    // TODO: defaults
    matMeshCache["ENG_Default"] = CPUMaterialData{
        "ENG_Default", glm::vec3(0.9f), glm::vec3(0.0f), 1.0f, 0.0f};

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a mesh + optional for error catching
const CPUMeshData *AssetManager::loadMesh(const std::string &filepath) {

    // Check if we already loaded this OBJ
    if (cpuMeshCache.find(filepath) != cpuMeshCache.end()) {
        Logger::info("ASSET", "Returning cached mesh: " + filepath);
        return &cpuMeshCache[filepath]; // Instantly return the cached
                                        // data
    }

    Logger::line();
    Logger::info("ASSET", "Starting to load mesh: " + filepath);

    // 2. If not, load it via tinyobjloader
    CPUMeshData newMesh;
    std::string mtlFilename;

    if (ModelLoader::loadOBJ(filepath, newMesh, mtlFilename)) {

        if (!mtlFilename.empty()) {
            std::string base_dir =
                filepath.substr(0, filepath.find_last_of('/') + 1);
            std::string fullMtlPath = base_dir + mtlFilename;

            loadMaterialBank(fullMtlPath);
        }

        Logger::info("ASSET", "Successfully loaded mesh: " + filepath + " (" +
                                  std::to_string(newMesh.vertices.size()) +
                                  " vertices)");
        Logger::space();

        cpuMeshCache[filepath] = newMesh; // Save to cache
        return &cpuMeshCache[filepath];
    }

    Logger::error("ASSET", "AssetManager Failed to load mesh at: " + filepath);

    return nullptr; // Failed to load
}

const CPUMaterialData *
AssetManager::getMaterial(const std::string &materialName) {

    auto it = matMeshCache.find(materialName);
    if (it != matMeshCache.end()) {
        Logger::info("ASSET", "Returning cached material: " + materialName);
        return &it->second; // Safely return the memory address of the cached
                            // value
    }

    Logger::error("ASSET",
                  "AssetManager failed to load material: " + materialName);
    return nullptr;
}

const void AssetManager::loadMaterialBank(const std::string &filepath) {
    std::vector<CPUMaterialData> newMaterials;

    if (MaterialLoader::loadMTL(filepath, newMaterials)) {
        std::string base_dir =
            filepath.substr(0, filepath.find_last_of('/') + 1);

        for (auto &mat : newMaterials) {

            Logger::info("ASSET",
                         "Successfully cached material at: " + base_dir);

            matMeshCache[mat.name] = mat;
        }
    }
}

} // namespace Engine
