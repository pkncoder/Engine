#include "ModelLoader.h"

#include "../services/Logger.h"

#include <string>
#include <tiny_obj_loader.h>

#include <unordered_map>

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
            Logger::error("SYSTEM", "TinyObj Error: " + reader.Error());
        }

        // Error
        return false;
    }

    // Reset the outmesh & update the filepath name
    outMesh.name = filepath;
    outMesh.vertices.clear();
    outMesh.indices.clear();

    // Avoid sending duplicated verticies by tracking unique ones, and trashing
    // any duplicates
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // Get the attributes and shapes from the reader of the mesh
    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();

    Logger::line();
    Logger::info("SYSTEM", "Shapes: " + std::to_string(shapes.size()));
    Logger::info("SYSTEM",
                 "Raw Verts: " + std::to_string(attrib.vertices.size()));

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

            // Check if this vertex is unique
            if (uniqueVertices.count(vertex) == 0) { // If it is

                // Save the vertex in the map
                uniqueVertices[vertex] = static_cast<uint32_t>(
                    outMesh.vertices.size()); // Cast into a uint32

                // Only push the unique vertex
                outMesh.vertices.push_back(vertex);
            }

            // Push the indicie back for the unique vertex
            outMesh.indices.push_back(uniqueVertices[vertex]);
        }
    }

    // No errors
    return true;
}

} // namespace Engine

// --- DIAGNOSTIC LOGS ---
