#include "AssetManager.h"

#include "../services/Logger.h"
#include "../services/UUID.h"
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

    // TODO: Load and create a default material

    Logger::info("ASSET", "AssetManager Initialized."); // Logging
}

// Load a CPUModelData - currently only supporting file types of .obj & .mtl
AssetHandle AssetManager::loadModel(const std::string &filepath) {

    // Check for returning cache by filepath
    const auto cachedPathIttr = cachedPaths.find(filepath);
    if (cachedPathIttr != cachedPaths.end()) {
        Logger::info("ASSET", "Returning cached model at: " + filepath);
        return cachedPathIttr->second;
    }

    // Load the model file through the model loader
    // TODO: wat.
    std::vector<CPUMeshData> meshes = ModelLoader::loadOBJ(filepath);

    // Check for failed mesh loading
    if (meshes.empty()) {
        Logger::error("ASSET",
                      "AssetManager Failed to load meshes at: " + filepath);
        return INVALID_ASSET_HANDLE; // Failed to load
    }

    // Final mesh handle list
    std::vector<AssetHandle> meshHandles;

    // Loop each mesh
    for (auto &mesh : meshes) {

        // Gen a new asset handle
        AssetHandle newHandle = UUID();

        // Assign the mesh to the list and cache
        meshHandles.push_back(newHandle);
        meshCache[newHandle] = std::make_shared<CPUMeshData>(mesh);
    }

    // Model data with the mesh handles
    std::shared_ptr<CPUModelData> model =
        std::make_shared<CPUModelData>(meshHandles);

    // Create a new AssetHandle
    AssetHandle newHandle = UUID();

    // Cache the model and return the new handle
    modelCache[newHandle] = model;
    return newHandle;
}

// Get a material from the cache
std::unordered_map<std::string, AssetHandle>
AssetManager::loadMaterialLibrary(const std::string &filepath) {

    // Final handle library
    std::unordered_map<std::string, AssetHandle> libraryHandles;

    // Load the mtl file
    std::unordered_map<std::string, CPUMaterialData> materials =
        MaterialLoader::loadMTL(filepath);

    // Loop each material
    for (const auto &[name, material] : materials) {

        // Get the cache key
        std::string cacheKey = filepath + ":" + name;

        // Look to see if this material has already been loaded
        const auto cachedPathIttr = cachedPaths.find(cacheKey);
        if (cachedPathIttr != cachedPaths.end()) {
            libraryHandles[name] = cachedPathIttr->second;
            continue;
        }

        // Generate a new handle
        AssetHandle newHandle = UUID();

        // Cache where caching caches
        materialCache[newHandle] =
            std::make_shared<CPUMaterialData>(material); // TODO: is this ok?
        cachedPaths[cacheKey] = newHandle;

        // Set the new handle with the material name
        libraryHandles[name] = newHandle;
    }

    return libraryHandles;
}

// Load a texture from a filepath
AssetHandle AssetManager::loadTexture(const std::string &filepath) {

    // Check cache, if it is already cached, return it
    const auto cachedPathIttr = cachedPaths.find(filepath);
    if (cachedPathIttr != cachedPaths.end()) {
        return cachedPathIttr->second;
    }

    // Load the new texture
    std::shared_ptr<CPUTextureData> textureData =
        TextureLoader::loadTexture(filepath);

    // TODO: Check for null texture

    // Get a new handle and cache the texture
    AssetHandle newHandle = UUID();
    textureCache[newHandle] = textureData;

    return newHandle;
}

std::shared_ptr<CPUModelData>
AssetManager::getModel(const AssetHandle handle) const {

    // Find the model and return it
    const auto modelIttr = modelCache.find(handle);
    if (modelIttr != modelCache.end()) {
        return modelIttr->second;
    }

    // No model cached under that handle
    return nullptr;
};

std::shared_ptr<CPUMeshData>
AssetManager::getMesh(const AssetHandle handle) const {

    // Find the mesh and return it
    const auto meshIttr = meshCache.find(handle);
    if (meshIttr != meshCache.end()) {
        return meshIttr->second;
    }

    // No mesh cached under that handle
    return nullptr;
};

std::shared_ptr<CPUMaterialData>
AssetManager::getMaterial(const AssetHandle handle) const {

    // Find the material and return it
    const auto materialIttr = materialCache.find(handle);
    if (materialIttr != materialCache.end()) {
        return materialIttr->second;
    }

    // No material cached under that handle
    return nullptr;
};

std::shared_ptr<CPUTextureData>
AssetManager::getTexture(const AssetHandle handle) const {

    // Find the texture and return it
    const auto textureIttr = textureCache.find(handle);
    if (textureIttr != textureCache.end()) {
        return textureIttr->second;
    }

    // No texture cached under that handle
    return nullptr;
};
} // namespace Engine
