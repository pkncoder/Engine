#include "ModelLoader.h"

#include <tiny_obj_loader.h>

#include <iostream>

namespace Engine {

// Loading an obj mesh
bool ModelLoader::loadOBJ(const std::string &filepath, MeshData &outMesh) {

    // tinyobjloader obj file reading configuration
    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate = true; // For converting quads to triangles

    // Get a tinyobjloader reader
    tinyobj::ObjReader reader;

    // Try to read from the file, and parse information if error
    if (!reader.ParseFromFile(filepath, reader_config)) { // Error
        if (!reader.Error().empty()) { // TinyObjLoader error or mid-read error
            std::cerr << "TinyObj Error: " << reader.Error() << std::endl;
        }
        return false;
    }

    // Get the attributes and shapes from the reader
    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();

    // Set the outMesh information
    outMesh.name = filepath;
    outMesh.vertices.clear();
    outMesh.indices.clear();

    // Loop each shape
    // TODO: Is better way?
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

            // Push the new vertex and indecies into the outMesh
            outMesh.vertices.push_back(vertex);
            outMesh.indices.push_back((uint32_t)outMesh.indices.size());
        }
    }

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
