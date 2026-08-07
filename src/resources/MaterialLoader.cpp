#include "MaterialLoader.h"

#include "../services/Logger.h"
#include "tiny_obj_loader.h"

#include <fstream>

namespace Engine {

std::unordered_map<std::string, CPUMaterialData>
MaterialLoader::loadMTL(const std::string &filepath) {

    // Final array of materials
    std::unordered_map<std::string, CPUMaterialData> finalMaterials;

    // Variables for tinyobjloader
    std::map<std::string, int> materialMap;
    std::vector<tinyobj::material_t> loaderMaterials;
    std::string warn, err;

    // Try and open the material file
    std::ifstream file(filepath);
    if (!file.is_open())
        return finalMaterials;

    // Load the mtl file
    tinyobj::LoadMtl(&materialMap, &loaderMaterials, &file, &warn, &err);

    // Get the base for the final texture paths
    std::string texturePathBase =
        filepath.substr(filepath.find_last_of("/") + 1);
    texturePathBase =
        texturePathBase.substr(0, texturePathBase.find(".")) + "/";

    // Loop over each loaded material
    for (const auto &material : loaderMaterials) {

        // Final material data
        CPUMaterialData materialData;

        // Material name
        materialData.name = material.name;

        // Color data
        materialData.albedo = glm::vec3(
            material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        materialData.emissive = glm::vec3(
            material.emission[0], material.emission[1], material.emission[2]);

        // Material data
        materialData.roughness = material.roughness;
        materialData.metallic = material.metallic;

        // Lambda function for loading & assigning textures
        auto assignTexture = [&](const std::string &loadedTexName,
                                 const std::string &mapTypeKey) {
            if (!loadedTexName.empty()) {
                materialData.textureMaps[mapTypeKey] =
                    assetManager->loadTexture("assets/textures/" +
                                              texturePathBase + loadedTexName);
            }
        };

        // Assign the new texture paths to the material
        assignTexture(material.diffuse_texname, "albedo");
        assignTexture(material.emissive_texname, "emissive");
        assignTexture(material.roughness_texname, "roughness");
        assignTexture(material.metallic_texname, "metallic");
        assignTexture(material.alpha_texname, "alpha");
        assignTexture(material.normal_texname, "normal");
        assignTexture(material.bump_texname, "bump");

        // Set the new material
        finalMaterials[materialData.name] = materialData;

        Logger::info("ASSET", "Successfully cached material:" + material.name);
    }

    return finalMaterials;
}
} // namespace Engine
