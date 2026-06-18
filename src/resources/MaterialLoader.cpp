#include "MaterialLoader.h"

#include "CPUStructs.h"

#include "tiny_obj_loader.h"

#include <fstream>
#include <vector>

namespace Engine {
std::vector<CPUMaterialData>
MaterialLoader::loadMTL(const std::string &filepath) {

    std::vector<CPUMaterialData> materials;

    std::map<std::string, int> materialMap;
    std::vector<tinyobj::material_t> loaderMaterials;
    std::string warn;
    std::string err;

    std::ifstream file(filepath);
    if (!file.is_open())
        return materials;

    tinyobj::LoadMtl(&materialMap, &loaderMaterials, &file, &warn, &err);

    for (const auto &material : loaderMaterials) {
        CPUMaterialData materialData;

        materialData.name = material.name;

        materialData.albedo = glm::vec3(
            material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        materialData.emmissive = glm::vec3(
            material.emission[0], material.emission[1], material.emission[2]);

        materialData.roughness = material.roughness;
        materialData.metallic = material.metallic;

        // TODO: temp
        if (!material.diffuse_texname.empty()) {

            std::string texturePathBase =
                filepath.substr(filepath.find_last_of("/") + 1);
            texturePathBase =
                texturePathBase.substr(0, texturePathBase.find(".")) + "/";

            materialData.textureNames["diffuse"] =
                texturePathBase + material.diffuse_texname;
        }

        materials.push_back(materialData);
    }

    return materials;
}
} // namespace Engine
