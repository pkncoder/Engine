#include "PrefabSpawner.h"

#include "../renderer/BufferManager.h"
#include "../services/Logger.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"
#include <memory>

namespace Engine {

std::vector<Entity> PrefabSpawner::spawnObjEntity(Scene &scene,
                                                  AssetManager &assetManager,
                                                  const std::string &filepath) {

    // Collection of all the new entities
    std::vector<Entity> entities;

    // Get the mesh data & check to make sure that it loaded right
    const AssetHandle modelHandle = assetManager.loadModel(filepath);

    if (modelHandle == INVALID_ASSET_HANDLE) {
        Logger::error("ASSET", "Failed to load model at: " + filepath);
        return entities; // Return an invalid handle on failure
    }

    std::shared_ptr<CPUModelData> modelData =
        assetManager.getModel(modelHandle);

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
        std::shared_ptr<CPUMaterialData> materialData =
            assetManager.getMaterial(mesh.materialHandle);

        // Add the new material component
        if (materialData != nullptr) {

            // Add a material component to the entity & save it
            auto &material = entity.addComponent<MaterialComponent>(
                MaterialComponent(mesh.materialHandle));
        }

        // If the model has no texture
        else {
            Logger::warn("SPAWNER", "No material found for: " +
                                        std::to_string(mesh.materialHandle));

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
