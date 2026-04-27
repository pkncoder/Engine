#include "AssetManager.h"

#include "ModelLoader.h"

#include <iostream>

namespace Engine {

// AssetManager initilaization
void AssetManager::init() {
    std::cout << "AssetManager Initialized." << std::endl; // Logging
}

// Load a mesh + optional for error catching
std::optional<MeshData> AssetManager::loadMesh(const std::string &filepath) {

    // Load the mesh & check for errors
    MeshData mesh;
    if (ModelLoader::loadOBJ(filepath, mesh)) { // Success
        std::cout << "Successfully loaded mesh: " << filepath << " ("
                  << mesh.vertices.size() << " vertices)" << std::endl;
        return mesh;
    }

    // Error
    std::cerr << "AssetManager Failed to load mesh: " << filepath << std::endl;
    return std::nullopt;
}

} // namespace Engine
