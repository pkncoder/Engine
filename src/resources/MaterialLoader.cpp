#include "MaterialLoader.h"

#include "CPUStructs.h"

#include "tiny_obj_loader.h"

#include <fstream>

namespace Engine {
std::vector<CPUMaterialData>
MaterialLoader::loadMTL(const std::string &filepath) {

    // Final array of materials
    std::vector<CPUMaterialData> materials;

    // Variables for tinyobjloader
    std::map<std::string, int> materialMap;
    std::vector<tinyobj::material_t> loaderMaterials;
    std::string warn;
    std::string err;

    // Try and open the material file
    std::ifstream file(filepath);
    if (!file.is_open())
        return materials;

    // Load the mtl file
    tinyobj::LoadMtl(&materialMap, &loaderMaterials, &file, &warn, &err);

    // Get the base for the final texture path(s)
    std::string texturePathBase =
        filepath.substr(filepath.find_last_of("/") + 1);
    texturePathBase =
        texturePathBase.substr(0, texturePathBase.find(".")) + "/";

    // Loop over each loaded material
    for (const auto &material : loaderMaterials) {
        CPUMaterialData materialData; // CPUMaterial

        // Material name
        materialData.name = material.name;

        // Color data
        materialData.albedo = glm::vec3(
            material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        materialData.emmissive = glm::vec3(
            material.emission[0], material.emission[1], material.emission[2]);

        // Material data
        materialData.roughness = material.roughness;
        materialData.metallic = material.metallic;

        // Lambda function for assigning textures
        auto assignTexture = [&](const std::string &loadedTexName,
                                 const std::string &mapTypeKey) {
            if (!loadedTexName.empty()) {
                materialData.textureNames[mapTypeKey] =
                    texturePathBase + loadedTexName;
            }
        };

        // Assign the new texture paths to the material
        assignTexture(material.diffuse_texname, "albedo");
        assignTexture(material.emissive_texname, "emissive");
        assignTexture(material.roughness_texname, "roughness");
        assignTexture(material.metallic_texname, "metallic");
        assignTexture(material.alpha_texname, "alpha");

        // Check both normal_texname & bump_texname for our normal
        assignTexture(material.normal_texname, "normal");
        if (materialData.textureNames.find("normal") ==
            materialData.textureNames.end()) {
            assignTexture(material.bump_texname, "normal");
        }

        // Push back the new material
        materials.push_back(materialData);
    }

    return materials;
}
} // namespace Engine
