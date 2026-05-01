#include "ModelLoader.h"

#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <unordered_map>

// TODO: Here? Can be moved by vertex definition?
namespace std {
template <> struct hash<Engine::Vertex> {
    size_t operator()(Engine::Vertex const &vertex) const {
        // Using a simple bit-shifting combine method
        size_t res = 0;

        // GLM's gtx/hash.hpp provides these:
        res ^= hash<glm::vec3>()(vertex.position) + 0x9e3779b9 + (res << 6) +
               (res >> 2);
        res ^= hash<glm::vec3>()(vertex.normal) + 0x9e3779b9 + (res << 6) +
               (res >> 2);
        res ^= hash<glm::vec2>()(vertex.texCoords) + 0x9e3779b9 + (res << 6) +
               (res >> 2);

        return res;
    }
};
} // namespace std

namespace Engine {

// Loading an obj mesh
bool ModelLoader::loadOBJ(const std::string &filepath, MeshData &outMesh) {

    // tinyobjloader obj file reading configuration
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.triangulate = true; // For converting quads to triangles

    // Get a tinyobjloader reader
    tinyobj::ObjReader reader;

    // Try to read from the file, and parse information if error
    if (!reader.ParseFromFile(filepath, readerConfig)) { // Error
        if (!reader.Error().empty()) { // TinyObjLoader error or mid-read error
            std::cerr << "TinyObj Error: " << reader.Error() << std::endl;
        }
        return false;
    }

    // Set the outMesh information
    outMesh.name = filepath;
    outMesh.vertices.clear();
    outMesh.indices.clear();

    // Push the new vertex and indecies into the outMesh
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // Get the attributes and shapes from the reader
    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();

    // Loop each shape
    for (const auto &shape : shapes) {

        // Loop each index in the shape
        for (const auto &index : shape.mesh.indices) {

            // Initialize a temp vertex
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

            // CHECK IF WE ALREADY LOADED THIS EXACT VERTEX
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] =
                    static_cast<uint32_t>(outMesh.vertices.size());
                outMesh.vertices.push_back(vertex);
            }

            // Always push the index pointing to the unique vertex
            outMesh.indices.push_back(uniqueVertices[vertex]);
        }
    }

    std::cout << "Vert List Length: " << outMesh.vertices.size() << std::endl;

    // No errors
    return true;
}

} // namespace Engine

// // --- DIAGNOSTIC LOGS ---
// std::cout << "[Diagnostic] File: " << filepath << std::endl;
// std::cout << "[Diagnostic] Shapes found: " << shapes.size() << std::endl;
// std::cout << "[Diagnostic] Raw Vertices in attrib: "
//           << (attrib.vertices.size() / 3) << std::endl;
// TODO: Add to a logger ^^^^^^^^^
