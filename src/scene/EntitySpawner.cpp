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
            // Use the CPUMaterialData -> MaterialComponent constructor
            entity.addComponent<MaterialComponent>(
                MaterialComponent(*materialData));

            // TODO: temp
            if (materialData->textureNames.find("diffuse") !=
                materialData->textureNames.end()) {

                entity.getComponent<MaterialComponent>().albedoTexture =
                    AssetManager::loadTexture(
                        "assets/textures/" +
                        materialData->textureNames.find("diffuse")->second);
                entity.getComponent<MaterialComponent>().albedo =
                    glm::vec3(1.0f);
            }

        } else { // If it doesn't exist
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
