#include "AssetManager.h"

#include "../services/Logger.h"
#include "CPUStructs.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

#include <string>

namespace Engine {

// AssetManager initilaization
void AssetManager::init() {

    // Setup an initial material in the cache for objects that don't have any
    // material listed
    // TODO: defaults
    materialCache["ENG_Default"] = CPUMaterialData{
        "ENG_Default", glm::vec3(0.9f), glm::vec3(0.0f), 1.0f, 0.0f};

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a mesh + optional for error catching
const CPUMeshData *AssetManager::loadMesh(const std::string &filepath) {

    // Check if we already loaded this OBJ
    if (meshCache.find(filepath) != meshCache.end()) {
        Logger::info("ASSET", "Returning cached mesh: " + filepath);
        return &meshCache[filepath]; // Instantly return the cached
                                     // data
    }

    Logger::line();
    Logger::info("ASSET", "Starting to load mesh: " + filepath);

    // 2. If not, load it via tinyobjloader
    CPUMeshData newMesh;
    std::string mtlFilename;

    if (ModelLoader::loadOBJ(filepath, newMesh, mtlFilename)) {

        // Make sure to check the mtlFilename's existance
        if (!mtlFilename.empty()) {

            // Get the base directory path
            std::string base_dir =
                filepath.substr(0, filepath.find_last_of('/') + 1);

            // Find where the models/ part of the base_dir is so we can cut that
            // out
            size_t modelFolderPos = base_dir.find("models/");
            if (modelFolderPos != std::string::npos) { // Safety check

                // Replace "models/" (7 characters) with "materials/"
                base_dir.replace(modelFolderPos, 7, "materials/");
            } else {

                // Filepath is not as expected
                Logger::warn(
                    "ASSET",
                    "'models/' directory not found in path: " + filepath +
                        ". Falling back to default directory.");
            }

            // Get the full path
            std::string fullMtlPath = base_dir + mtlFilename;

            // Cache the materials
            cacheMaterials(fullMtlPath, &newMesh);
        }

        Logger::info("ASSET", "Successfully loaded mesh: " + filepath + " (" +
                                  std::to_string(newMesh.vertices.size()) +
                                  " vertices)");
        Logger::space();

        meshCache[filepath] = newMesh;
        return &meshCache[filepath];
    }

    Logger::error("ASSET", "AssetManager Failed to load mesh at: " + filepath);

    return nullptr; // Failed to load
}

const CPUMaterialData *
AssetManager::getMaterial(const std::string &materialName) {

    auto it = materialCache.find(materialName);
    if (it != materialCache.end()) {
        Logger::info("ASSET", "Returning cached material: " + materialName);
        return &it->second; // Safely return the memory address of the cached
                            // value
    }

    Logger::error("ASSET",
                  "AssetManager failed to load material: " + materialName);
    return nullptr;
}

const void AssetManager::cacheMaterials(const std::string &filepath,
                                        CPUMeshData *tempMeshData) {
    std::vector<CPUMaterialData> newMaterials;

    if (MaterialLoader::loadMTL(filepath, newMaterials)) {
        for (auto &mat : newMaterials) {

            Logger::info("ASSET",
                         "Successfully cached material at: " + filepath);

            materialCache[mat.name] = mat;

            tempMeshData->materialName = mat.name;
        }
    }
}

GLuint AssetManager::loadTexture(const std::string &filepath) {
    // Check cache
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }

    // Load and cache
    GLuint textureID = TextureLoader::loadTexture(filepath);
    if (textureID != 0) {
        textureCache[filepath] = textureID;
    }
    return textureID;
}

} // namespace Engine
