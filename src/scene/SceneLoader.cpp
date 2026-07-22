#include "SceneLoader.h"

#include "../renderer/BufferManager.h"
#include "../services/Logger.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Engine {

std::vector<Entity> SceneLoader::spawnObjEntity(Scene &scene,
                                                AssetManager &assetManager,
                                                const std::string &filepath) {

    // Collection of all the new entities
    std::vector<Entity> entities;

    // Get the mesh data & check to make sure that it loaded right
    const CPUModelData *modelData = assetManager.loadModel(filepath);
    if (!modelData) {
        Logger::error("ASSET", "Failed to load model at: " + filepath);
        return entities; // Return an invalid handle on failure
    }

    Logger::info("ASSET",
                 "Mesh length: " + std::to_string(modelData->meshes.size()));

    // Loop each mesh to create a new renderable
    for (auto &mesh : modelData->meshes) {

        // Allocate an ID and instantiate it into an entity
        const EntityID entityId = scene.createEntity();
        Entity entity(entityId, &scene);

        // Upload the mesh to VRAM and get the mesh component
        const MeshComponent meshComponent = BufferManager::uploadMesh(mesh);
        entity.addComponent<MeshComponent>(meshComponent);

        // Apply the default transform component
        entity.addComponent<TransformComponent>(TransformComponent());

        // Get the material data that was loaded on mesh construction
        const CPUMaterialData *materialData =
            assetManager.getMaterial(mesh.materialName);

        // Add the new material component
        if (materialData != nullptr) {

            // Add a material component to the entity & save it
            auto &material = entity.addComponent<MaterialComponent>(
                MaterialComponent(*materialData));

            // Define a lambda helper to load texture maps while checking if
            // they exist
            auto loadTextureMap = [&](const std::string &mapKey,
                                      GLuint &targetVariable) {
                // Use an iterator to avoid searching the map twice
                const auto it = materialData->textureNames.find(mapKey);

                if (it != materialData->textureNames.end()) {
                    targetVariable = assetManager.loadTexture(
                        "assets/textures/" + it->second, material.isBumpMap);
                }
            };

            // Load the texture maps
            loadTextureMap("albedo", material.albedoTexture);
            loadTextureMap("emissive", material.emissiveTexture);
            loadTextureMap("metallic", material.metallicTexture);
            loadTextureMap("roughness", material.roughnessTexture);
            loadTextureMap("alpha", material.alphaTexture);
            loadTextureMap("normal", material.normalTexture);

            // Handle albedo texture fallback
            if (material.albedoTexture != 0 &&
                glm::length(material.albedo) <= 0.001f) {
                material.albedo = glm::vec3(1.0f);
            }

            // Handle emissive texture fallback
            if (material.emissiveTexture != 0 &&
                glm::length(material.emmissive) <= 0.001f) {
                material.emmissive = glm::vec3(1.0f);
            }
        }

        // If the model has no texture
        else {
            Logger::warn("SPAWNER",
                         "No material found for: " + mesh.materialName);

            // Default material
            entity.addComponent<MaterialComponent>(MaterialComponent());
        }

        // Push the new entity
        entities.push_back(entity);
    }

    // Return the final entity list
    return entities;
}

} // namespace Engine
