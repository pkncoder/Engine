#include "AssetManager.h"
#include "ModelLoader.h"
#include <iostream>

namespace Engine {

void AssetManager::init() {
    std::cout << "AssetManager Initialized." << std::endl;
}

std::optional<MeshData> AssetManager::loadMesh(const std::string &filepath) {
    MeshData mesh;
    if (ModelLoader::loadOBJ(filepath, mesh)) {
        std::cout << "Successfully loaded mesh: " << filepath << " ("
                  << mesh.vertices.size() << " vertices)" << std::endl;
        return mesh;
    }

    std::cerr << "AssetManager Failed to load mesh: " << filepath << std::endl;
    return std::nullopt;
}

} // namespace Engine
