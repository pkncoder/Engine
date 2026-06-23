#include "ModelLoader.h"

#include "../services/Logger.h"
#include "CPUStructs.h"

#include <tiny_obj_loader.h>

#include <fstream>
#include <unordered_map>

namespace Engine {

// Loading an obj mesh
CPUModelData ModelLoader::loadOBJ(const std::string &filepath) {

    CPUModelData model;

    // Load the file to search for the material file path
    std::ifstream file(filepath);
    std::string line;

    // Loop every line, until "mtllib " which contains the filename
    while (std::getline(file, line)) {
        if (line.substr(0, 7) == "mtllib ") {
            model.materialPath = line.substr(7); // Get the base of the filename
            model.materialPath.erase(
                model.materialPath.find_last_not_of(" \n\r\t") +
                1); // Strip off extras

            // Append the material file loaction
            // TODO: constant
            model.materialPath = "assets/materials/" + model.materialPath;
            break;
        }
    }
    file.close(); // Close file for cleanup

    // tinyobjloader obj file reader config
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.triangulate = true; // For converting quads to triangles
    readerConfig.mtl_search_path = "assets/materials"; // TODO: constant

    // Get a tinyobjloader readerkl
    tinyobj::ObjReader reader;

    // Try to read from the file and check for erros
    if (!reader.ParseFromFile(filepath, readerConfig)) { // Error
        if (!reader.Error().empty()) { // TinyObjLoader error or mid-read error
            Logger::error("ASSET", "tinyobjloader Error: " + reader.Error());
        }

        // Error
        return model;
    }

    // Set the filepath of the model
    model.name = filepath;

    // Get the attributes and shapes from the reader for the model
    const auto &attrib = reader.GetAttrib();
    const auto &shapes = reader.GetShapes();
    const auto &materials = reader.GetMaterials();

    // Loop each shape
    for (const auto &shape : shapes) {

        // Save a map of sub-meshes per shape
        std::unordered_map<int, CPUMeshData> subMeshes;

        // Track unique verticies by material id
        std::unordered_map<int, std::unordered_map<CPUVertex, uint32_t>>
            uniqueMaterialVerticies;

        // Keep an offset for each index to calculate vertex array indexes
        size_t vertexIndexOffset = 0;

        // Loop each index in the shape
        for (size_t faceIndex = 0;
             faceIndex < shape.mesh.num_face_vertices.size(); faceIndex++) {

            // Get the vertex number
            size_t vertexNumber =
                size_t(shape.mesh.num_face_vertices[faceIndex]);

            // Get this face's material id - defaults to -1 for no material
            int materialID = shape.mesh.material_ids.empty()
                                 ? -1
                                 : shape.mesh.material_ids[faceIndex];

            // Get the mesh data & vertice cache for this material id
            auto &meshData = subMeshes[materialID];
            auto &uniqueVertices = uniqueMaterialVerticies[materialID];

            // TODO: Constant
            meshData.materialName =
                shape.name + "_Mn" + std::to_string(materialID);

            // Loop each vertex
            for (size_t vertexIndex = 0; vertexIndex < vertexNumber;
                 vertexIndex++) {

                // Get the index of this vertex
                tinyobj::index_t index =
                    shape.mesh.indices[vertexIndexOffset + vertexIndex];

                // Initialize a temp vertex
                CPUVertex vertex{};

                // Get & set the vertex position attribute
                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};
                }

                // Get & set the vertex normal attribute
                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                }

                // Get & set the tex coordinate attribute
                if (index.texcoord_index >= 0) {
                    vertex.texCoords = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]};
                } else {
                    vertex.texCoords = glm::vec2(0.0f, 0.0f);
                }

                // Make sure data was loaded
                if (uniqueVertices.count(vertex) == 0) {

                    // Set the unique vertice with the vert size
                    uniqueVertices[vertex] =
                        static_cast<uint32_t>(meshData.vertices.size());

                    // Push the new verts to the mesh data
                    meshData.vertices.push_back(vertex);
                }

                // Push the new indicies to the mesh data
                meshData.indices.push_back(uniqueVertices[vertex]);
            }

            // Increase the index offset
            vertexIndexOffset += vertexNumber;
        }

        // Loop the final material id & meshdata from each sub mesh
        for (auto &[matID, meshData] : subMeshes) {

            // Assign the name from the parsed materials list
            if (matID >= 0 && matID < materials.size()) {
                meshData.materialName = materials[matID].name;
            } else {
                // Default to the default material
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
