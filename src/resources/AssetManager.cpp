#include "AssetManager.h"

#include "../services/Logger.h"
#include "CPUStructs.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

#include <__config>
#include <cstddef>
#include <string>

namespace Engine {

void AssetManager::init() {

    // Load a default material
    materialCache["ENG_Default"] = CPUMaterialData();

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

const CPUModelData *AssetManager::loadModel(const std::string &filepath) {

    if (modelCache.find(filepath) != modelCache.end()) {
        Logger::info("ASSET", "Returning cached mesh: " + filepath);
        return &modelCache[filepath];
    }

    CPUModelData model = ModelLoader::loadOBJ(filepath);

    if (!model.meshes.empty()) {

        // Load materials
        if (!model.materialPath.empty()) {
            cacheMaterials("assets/materials/" + model.materialPath);
        }

        Logger::info("ASSET", "Successfully loaded model at: " + filepath +
                                  " (" + std::to_string(model.meshes.size()) +
                                  " meshes)");
        Logger::space();

        modelCache[filepath] = model;
        return &modelCache[filepath];
    }

    Logger::error("ASSET", "AssetManager Failed to load mesh at: " + filepath);

    return nullptr; // Failed to load
}

const CPUMaterialData *
AssetManager::getMaterial(const std::string &materialName) {

    auto it = materialCache.find(materialName);
    if (it != materialCache.end()) {
        Logger::info("ASSET", "Returning cached material: " + materialName);
        return &it->second;
    }

    Logger::error("ASSET",
                  "AssetManager failed to load material: " + materialName);
    return nullptr;
}

const void AssetManager::cacheMaterials(const std::string &filepath) {
    std::vector<CPUMaterialData> materials = MaterialLoader::loadMTL(filepath);

    if (!materials.empty()) {
        for (auto &material : materials) {

            materialCache[material.name] = material;

            Logger::info("ASSET",
                         "Successfully cached material at: " + filepath);
        }
    }
}

GLuint AssetManager::loadTexture(const std::string &filepath) {
    // Check cache
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }

    // Load and cache
    // TODO: Swap to handles
    GLuint textureID = TextureLoader::loadTexture(filepath);
    if (textureID != 0) {
        textureCache[filepath] = textureID;
    }
    return textureID;
}

} // namespace Engine
