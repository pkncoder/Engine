#include "EntitySpawner.h"

#include "../renderer/BufferManager.h"
#include "../resources/AssetManager.h"
#include "../services/Logger.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Engine {

std::vector<Entity> EntitySpawner::spawnObjEntity(Scene &scene,
                                                  const std::string &filepath) {

    std::vector<Entity> entities;

    // Get the mesh data & check for it's existace
    const CPUModelData *modelData = AssetManager::loadModel(filepath);
    if (!modelData) {
        Logger::error("ASSET", "Failed to load model at: " + filepath);
        return entities; // Return an invalid handle on failure
    }

    Logger::info("ASSET",
                 "Mesh length: " + std::to_string(modelData->meshes.size()));

    for (auto &mesh : modelData->meshes) {

        // Allocate an ID and instantiate it into an entity
        EntityID entityId = scene.createEntity();
        Entity entity(entityId, &scene);

        // Upload raw geometry to VRAM and bind the Mesh Component
        MeshComponent meshComponent = BufferManager::uploadMesh(mesh);
        entity.addComponent<MeshComponent>(meshComponent);

        // Apply a default spatial transform
        entity.addComponent<TransformComponent>(TransformComponent());

        // Get the material data that was loaded on mesh construction
        const CPUMaterialData *materialData =
            AssetManager::getMaterial(mesh.materialName);

        // Add the new material component
        if (materialData != nullptr) {

            // 1. Add the component and grab a reference to it immediately
            auto &material = entity.addComponent<MaterialComponent>(
                MaterialComponent(*materialData));

            // 2. Define our lambda helper
            // It takes the dictionary key ("albedo", "normal") and a REFERENCE
            // to the target variable
            auto loadTextureMap = [&](const std::string &mapKey,
                                      GLuint &targetVariable) {
                // Use an iterator to avoid searching the map twice
                auto it = materialData->textureNames.find(mapKey);

                if (it != materialData->textureNames.end()) {
                    targetVariable = AssetManager::loadTexture(
                        "assets/textures/" + it->second, material.isBumpMap);
                }
            };

            // 3. Process all the maps!
            loadTextureMap("albedo", material.albedoTexture);
            loadTextureMap("emissive", material.emissiveTexture);
            loadTextureMap("metallic", material.metallicTexture);
            loadTextureMap("roughness", material.roughnessTexture);
            loadTextureMap("normal", material.normalTexture);

            // 4. Handle the albedo color fallback
            if (material.albedoTexture != 0 &&
                glm::length(material.albedo) <= 0.001f) {
                material.albedo = glm::vec3(1.0f);
            }

            if (material.emissiveTexture != 0 &&
                glm::length(material.emmissive) <= 0.001f) {
                material.emmissive = glm::vec3(1.0f);
            }
        }

        else { // If it doesn't exist
            Logger::warn("SPAWNER",
                         "No material found for: " + mesh.materialName);

            // Error material
            entity.addComponent<MaterialComponent>(MaterialComponent());
        }

        entities.push_back(entity);
    }

    // Return the final entity
    return entities;
}

} // namespace Engine
