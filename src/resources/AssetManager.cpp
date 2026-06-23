#include "AssetManager.h"

#include "../Constants.h"
#include "../services/Logger.h"
#include "CPUStructs.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

namespace Engine {

void AssetManager::init() {

    // Load a default material
    materialCache[Constants::Asset::DEFAULT_MATERIAL_NAME] = CPUMaterialData();

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a CPUModelData - currently only supporting file types of .obj & .mtl
const CPUModelData *AssetManager::loadModel(const std::string &filepath) {

    // Check to see if this filepath has already been loaded, if so return from
    // the cache
    if (modelCache.find(filepath) != modelCache.end()) {
        Logger::info("ASSET", "Returning cached mesh: " + filepath);
        return &modelCache[filepath];
    }

    // Load the model through the model loader
    CPUModelData model = ModelLoader::loadOBJ(filepath);

    // Check to see if the load was successful
    if (!model.meshes.empty()) {

        // Cache the materials from the found filepath
        if (!model.materialPath.empty()) {
            cacheMaterials(model.materialPath);
        }

        Logger::info("ASSET", "Successfully loaded model at: " + filepath +
                                  " (" + std::to_string(model.meshes.size()) +
                                  " meshes)");
        Logger::space();

        // Cache the model and return the new referance
        modelCache[filepath] = model;
        return &modelCache[filepath];
    }

    Logger::error("ASSET", "AssetManager Failed to load mesh at: " + filepath);

    return nullptr; // Failed to load
}

// Get a material from the cache
const CPUMaterialData *
AssetManager::getMaterial(const std::string &materialName) {

    const auto it = materialCache.find(materialName);
    if (it != materialCache.end()) {
        Logger::info("ASSET", "Returning cached material: " + materialName);
        return &it->second;
    }

    Logger::error("ASSET",
                  "AssetManager failed to load material: " + materialName);
    return nullptr;
}

// Load a texture from a filepath
GLuint AssetManager::loadTexture(const std::string &filepath,
                                 bool &bumpTexture) {
    // Check cache, if it is already cached, return it
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }

    // Load and cache the texture
    const GLuint textureID = TextureLoader::loadTexture(filepath, bumpTexture);
    if (textureID != 0) {
        textureCache[filepath] = textureID;
    }

    return textureID;
}

// Cache materials from the found filepath
const void AssetManager::cacheMaterials(const std::string &filepath) {

    // Get each material
    std::vector<CPUMaterialData> materials = MaterialLoader::loadMTL(filepath);

    // Check to see if there is any materials
    if (!materials.empty()) {

        // Loop each material & cache it
        for (auto &material : materials) {
            materialCache[material.name] = material;

            Logger::info("ASSET",
                         "Successfully cached material at: " + filepath);
        }
    }
}

} // namespace Engine
