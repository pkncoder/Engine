#include "AssetManager.h"

#include "../services/Logger.h"
#include "../services/UUID.h"
#include "CPUStructs.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"
#include <memory>

namespace Engine {

AssetManager::AssetManager(EngineContext &engineContext)
    : engineContext(engineContext) {

    // Inject the asset manager to each loader
    ModelLoader::init(this);
    MaterialLoader::init(this);
    TextureLoader::init(this);

    // Load and create a default material

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a CPUModelData - currently only supporting file types of .obj & .mtl
AssetHandle AssetManager::loadModel(const std::string &filepath) {

    // Check to see if this filepath has already been loaded, if so return from
    // the cache
    const auto cachedPathIttr = cachedPaths.find(filepath);
    if (cachedPathIttr != cachedPaths.end()) {
        Logger::info("ASSET", "Returning cached model at: " + filepath);
        return cachedPathIttr->second;
    }

    // Load the model through the model loader
    std::vector<CPUMeshData> meshes = ModelLoader::loadOBJ(filepath);

    if (meshes.empty()) {
        Logger::error("ASSET",
                      "AssetManager Failed to load meshes at: " + filepath);
        return INVALID_ASSET_HANDLE; // Failed to load
    }

    std::vector<AssetHandle> meshHandles;

    // Check to see if the load was successful
    for (auto &mesh : meshes) {
        AssetHandle newHandle = UUID();
        meshHandles.push_back(newHandle);

        meshCache[newHandle] = std::make_shared<CPUMeshData>(mesh);
    }

    std::shared_ptr<CPUModelData> model =
        std::make_shared<CPUModelData>(meshHandles);

    // Create a new AssetHandle
    AssetHandle newHandle = UUID();

    // Cache the model and return the new referance
    modelCache[newHandle] = model;
    return newHandle;
}

// Get a material from the cache
std::unordered_map<std::string, AssetHandle>
AssetManager::loadMaterialLibrary(const std::string &filepath) {

    std::unordered_map<std::string, AssetHandle> libraryHandles;

    std::unordered_map<std::string, CPUMaterialData> materials =
        MaterialLoader::loadMTL(filepath);

    for (const auto &[name, material] : materials) {

        std::string cacheKey = filepath + ":" + name;

        const auto cachedPathIttr = cachedPaths.find(cacheKey);
        if (cachedPathIttr != cachedPaths.end()) {
            libraryHandles[name] = cachedPathIttr->second;
            continue; // Skip loading, as it already has been
        }

        // 4. Generate UUID and cache everything
        AssetHandle newHandle = UUID(); // Assuming your UUID acts as a handle

        materialCache[newHandle] =
            std::make_shared<CPUMaterialData>(material); // TODO: is this ok?
        cachedPaths[cacheKey] = newHandle;

        libraryHandles[name] = newHandle;
    }

    // Check for null material
    return libraryHandles;
}

// Load a texture from a filepath
AssetHandle AssetManager::loadTexture(const std::string &filepath) {
    // Check cache, if it is already cached, return it
    const auto cachedPathIttr = cachedPaths.find(filepath);
    if (cachedPathIttr != cachedPaths.end()) {
        return cachedPathIttr->second;
    }

    // Load and cache the texture
    std::shared_ptr<CPUTextureData> textureData =
        TextureLoader::loadTexture(filepath);

    // Check for null texture

    AssetHandle newHandle = UUID();
    textureCache[newHandle] = textureData;

    return newHandle;
}

std::shared_ptr<CPUModelData> AssetManager::getModel(AssetHandle handle) const {

    const auto modelIttr = modelCache.find(handle);

    if (modelIttr != modelCache.end()) {
        return modelIttr->second;
    }

    return nullptr;
};

std::shared_ptr<CPUMaterialData>
AssetManager::getMaterial(AssetHandle handle) const {

    const auto materialIttr = materialCache.find(handle);

    if (materialIttr != materialCache.end()) {
        return materialIttr->second;
    }

    return nullptr;
};

std::shared_ptr<CPUTextureData>
AssetManager::getTexture(AssetHandle handle) const {

    const auto textureIttr = textureCache.find(handle);

    if (textureIttr != textureCache.end()) {
        return textureIttr->second;
    }

    return nullptr;
};
} // namespace Engine
