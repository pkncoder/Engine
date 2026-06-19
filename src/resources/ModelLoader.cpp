#include "ModelLoader.h"
#include "../services/Logger.h"
#include "CPUStructs.h"

#include <cstddef>
#include <tiny_obj_loader.h>

#include <fstream>
#include <string>
#include <unordered_map>

namespace Engine {

// Loading an obj mesh
CPUModelData ModelLoader::loadOBJ(const std::string &filepath) {

    CPUModelData model;

    // Get the file information for getting the outMtlFilename
    std::ifstream file(filepath);
    std::string line;

    // Loop every line, and look for "mtllib "
    while (std::getline(file, line)) {
        if (line.substr(0, 7) == "mtllib ") {
            model.materialPath = line.substr(7); // Get the base of the filename
            model.materialPath.erase(
                model.materialPath.find_last_not_of(" \n\r\t") +
                1); // Strip of extras
            break;
        }
    }
    file.close(); // Close file for cleanup

    // tinyobjloader obj file reading configuration
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.triangulate = true; // For converting quads to triangles
    readerConfig.mtl_search_path = "assets/materials"; // TODO: constant

    // Get a tinyobjloader reader
    tinyobj::ObjReader reader;

    // Try to read from the file, and parse information if error
    if (!reader.ParseFromFile(filepath, readerConfig)) { // Error
        if (!reader.Error().empty()) { // TinyObjLoader error or mid-read error
            Logger::error("ASSET", "TinyObj Error: " + reader.Error());
        }

        // Error
        return model;
    }

    // Reset the outmesh & update the filepath name
    model.name = filepath;

    // Get the attributes and shapes from the reader of the mesh
    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();
    const auto &materials = reader.GetMaterials();

    // Loop each shape
    for (const auto &shape : shapes) {

        std::unordered_map<int, CPUMeshData> subMeshes;

        // Avoid sending duplicated verticies by tracking unique ones, and
        // indexing any duplicates
        std::unordered_map<int, std::unordered_map<CPUVertex, uint32_t>>
            uniqueMaterialVerticies;

        size_t indexOffset = 0;

        // Loop each index in the shape
        for (size_t faceIndex = 0;
             faceIndex < shape.mesh.num_face_vertices.size(); faceIndex++) {

            size_t vertexNumber =
                size_t(shape.mesh.num_face_vertices[faceIndex]);

            int materialID = shape.mesh.material_ids.empty()
                                 ? -1
                                 : shape.mesh.material_ids[faceIndex];

            // Get references to the specific sub-mesh and deduplication map for
            // this material
            auto &meshData = subMeshes[materialID];
            auto &uniqueVertices = uniqueMaterialVerticies[materialID];

            for (size_t vertexIndex = 0; vertexIndex < vertexNumber;
                 vertexIndex++) {

                tinyobj::index_t index =
                    shape.mesh.indices[indexOffset + vertexIndex];

                // Initialize a temp vertex
                CPUVertex vertex{};

                vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                                   attrib.vertices[3 * index.vertex_index + 1],
                                   attrib.vertices[3 * index.vertex_index + 2]};

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                }

                if (index.texcoord_index >= 0) {
                    vertex.texCoords = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]};
                }

                if (uniqueVertices.count(vertex) == 0) {

                    uniqueVertices[vertex] =
                        static_cast<uint32_t>(meshData.vertices.size());

                    meshData.vertices.push_back(vertex);
                }

                meshData.indices.push_back(uniqueVertices[vertex]);
            }

            indexOffset += vertexNumber;
        }

        for (auto &[matID, meshData] : subMeshes) {

            // Assign the name from the parsed materials list
            if (matID >= 0 && matID < materials.size()) {
                meshData.materialName = materials[matID].name;
            } else {
                meshData.materialName = "ENG_Default";
            }

            // Add this sub-mesh chunk to the model
            model.meshes.push_back(meshData);
        }
    }

    return model;
}

} // namespace Engine

// --- DIAGNOSTIC LOGS ---
