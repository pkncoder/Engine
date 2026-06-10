#include "EntitySpawner.h"

#include "../renderer/BufferManager.h"
#include "../resources/AssetManager.h"
#include "../services/Logger.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

namespace Engine {

Entity EntitySpawner::spawnModel(Scene &scene, const std::string &filepath) {

    // Get the mesh data & check for it's existace
    const CPUMeshData *meshData = AssetManager::loadMesh(filepath);
    if (!meshData) {
        Logger::error("SPAWNER",
                      "Failed to load mesh data for asset: " + filepath);
        return Entity(NULL_ENTITY,
                      nullptr); // Return an invalid handle on failure
    }

    // Allocate an ID and instantiate it into an entity
    EntityID entityId = scene.createEntity();
    Entity entity(entityId, &scene);

    // Upload raw geometry to VRAM and bind the Mesh Component
    MeshComponent meshComponent = BufferManager::uploadMesh(*meshData);
    entity.addComponent<MeshComponent>(meshComponent);

    // Apply a default spatial transform
    // TODO: Defaults?
    entity.addComponent<Transform>({
        glm::vec3(0.0f),                   // Position
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f), // Rotation
        glm::vec3(1.0f)                    // Scale
    });

    // Get the material data that was loaded on mesh construction
    const CPUMaterialData *materialData =
        AssetManager::getMaterial(meshData->materialName);

    // Check to make sure it exissts
    if (materialData != nullptr) {
        // TODO: Make a CPUMaterialData.into<MaterialComponent> "sorry for the
        // rust"
        entity.addComponent<MaterialComponent>( // Instantiate a material
                                                // component
            MaterialComponent{materialData->albedo, materialData->emmission,
                              materialData->roughness, materialData->metallic});
    } else { // If it doesn't exist
        Logger::warn("SPAWNER", "No material found for '" +
                                    meshData->materialName +
                                    "'. Defaulting baseline.");

        // Default material
        // TODO: DEfault?
        entity.addComponent<MaterialComponent>(MaterialComponent{
            glm::vec3(0.7f), // Mid-gray albedo
            glm::vec3(0.0f), // No emission
            0.5f,            // Roughness
            0.0f             // Non-metallic
        });
    }

    // Return the final entity
    return entity;
}

} // namespace Engine
