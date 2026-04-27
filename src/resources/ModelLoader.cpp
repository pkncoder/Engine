#include "ModelLoader.h"
#include <iostream>

// Note: Do NOT define TINYOBJLOADER_IMPLEMENTATION if using vcpkg's library
// version
#include <tiny_obj_loader.h>

namespace Engine {

bool ModelLoader::loadOBJ(const std::string &filepath, MeshData &outMesh) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate =
        true; // CRITICAL: Converts Blender Quads to Triangles

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObj Error: " << reader.Error() << std::endl;
        }
        return false;
    }

    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();

    // --- DIAGNOSTIC LOGS ---
    std::cout << "[Diagnostic] File: " << filepath << std::endl;
    std::cout << "[Diagnostic] Shapes found: " << shapes.size() << std::endl;
    std::cout << "[Diagnostic] Raw Vertices in attrib: "
              << (attrib.vertices.size() / 3) << std::endl;

    outMesh.name = filepath;
    outMesh.vertices.clear();
    outMesh.indices.clear();

    for (const auto &shape : shapes) {
        std::cout << "[Diagnostic] Shape '" << shape.name << "' has "
                  << shape.mesh.indices.size() << " indices." << std::endl;

        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};

            // Pull position
            vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            // Pull normals
            if (index.normal_index >= 0) {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            // Pull texcoords
            if (index.texcoord_index >= 0) {
                vertex.texCoords = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            outMesh.vertices.push_back(vertex);
            outMesh.indices.push_back((uint32_t)outMesh.indices.size());
        }
    }

    return true;
}
} // namespace Engine
