#include "MaterialLoader.h"

#include "CPUStructs.h"

#include "tiny_obj_loader.h"

#include <fstream>
#include <vector>

namespace Engine {
bool MaterialLoader::loadMTL(const std::string &filepath,
                             std::vector<CPUMaterialData> &outMaterials) {
    std::map<std::string, int> materialMap;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    std::ifstream file(filepath);
    if (!file.is_open())
        return false;

    tinyobj::LoadMtl(&materialMap, &materials, &file, &warn, &err);

    for (const auto &mat : materials) {
        CPUMaterialData materialData;

        materialData.name = mat.name;

        materialData.albedo =
            glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
        materialData.emmissive =
            glm::vec3(mat.emission[0], mat.emission[1], mat.emission[2]);

        materialData.roughness = mat.roughness;
        materialData.metallic = mat.metallic;

        outMaterials.push_back(materialData);
    }

    return true;
}
} // namespace Engine
