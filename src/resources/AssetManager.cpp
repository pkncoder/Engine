#include "AssetManager.h"

#include "../services/Logger.h"
#include "ModelLoader.h"

#include <iostream>
#include <string>

namespace Engine {

// AssetManager initilaization
void AssetManager::init() {
    Logger::info("SYSTEM", "AssetManager Initialized."); // Logging
}

// Load a mesh + optional for error catching
std::optional<MeshData> AssetManager::loadMesh(const std::string &filepath) {

    Logger::info("SYSTEM", "Starting to load mesh: " + filepath);

    // Load the mesh & check for errors
    MeshData mesh;
    if (ModelLoader::loadOBJ(filepath, mesh)) { // Success
        Logger::info("SYSTEM", "Successfully loaded mesh: " + filepath + " (" +
                                   std::to_string(mesh.vertices.size()) +
                                   " vertices)");
        Logger::space();
        return mesh;
    }

    // Error
    Logger::error("SYSTEM", "AssetManager Failed to load mesh: ");
    return std::nullopt;
}

} // namespace Engine
