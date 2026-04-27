#include "Scene.h"
#include <iostream>

namespace Engine {

Scene::Scene() {}
Scene::~Scene() {}

void Scene::addDebugMesh(const MeshData &mesh) { debugMeshes.push_back(mesh); }

void Scene::printDebugInfo() const {
    std::cout << "\n--- SCENE DEBUG INFO ---" << std::endl;
    std::cout << "Total Meshes: " << debugMeshes.size() << std::endl;

    for (size_t i = 0; i < debugMeshes.size(); ++i) {
        const auto &mesh = debugMeshes[i];
        std::cout << "Mesh [" << i << "]: " << mesh.name << "\n";
        std::cout << "  Vertices: " << mesh.vertices.size() << "\n";
        std::cout << "  Indices: " << mesh.indices.size() << "\n";

        // Print the very first vertex position just to prove data is real
        if (!mesh.vertices.empty()) {
            const auto &v = mesh.vertices[0].position;
            std::cout << "  First Vertex Pos: (" << v.x << ", " << v.y << ", "
                      << v.z << ")\n";
        }
    }
    std::cout << "------------------------\n\n";
}

} // namespace Engine
