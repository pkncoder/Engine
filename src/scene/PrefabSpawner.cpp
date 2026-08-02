#include "PrefabSpawner.h"

#include "../services/Logger.h"
#include "Entity.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

#include <memory>

namespace Engine {

std::vector<EntityID>
PrefabSpawner::spawnObjEntity(Scene &scene, AssetManager &assetManager,
                              const std::string &filepath) {

    // Collection of all the new entities
    std::vector<EntityID> entities;

    // Get the mesh data & check to make sure that it loaded right
    const AssetHandle modelHandle = assetManager.loadModel(filepath);
    if (modelHandle == INVALID_ASSET_HANDLE) {
        Logger::error("ASSET", "Failed to load model at: " + filepath);
        return entities; // TODO: temp - Return the empty array
    }

    // Get the data of the model
    std::shared_ptr<CPUModelData> modelData =
        assetManager.getModel(modelHandle);

    // Loop each mesh to create a new renderable
    for (auto &meshHandle : modelData->meshHandles) {

        // Get the mesh data
        auto meshData = assetManager.getMesh(meshHandle);

        // Allocate an ID and create a new entity helper
        const EntityID entityId = scene.createEntity();
        Entity entity(entityId, &scene);

        // Create a new mesh component and add the entity
        const MeshComponent meshComponent =
            MeshComponent(meshData->name, modelHandle);
        entity.addComponent<MeshComponent>(meshComponent);

        // Apply the default transform component
        entity.addComponent<TransformComponent>(TransformComponent());

        // Check to see if the mesh has a material handle
        if (meshData->materialHandle == INVALID_ASSET_HANDLE) {

            // Add a material component to the entity
            entity.addComponent<MaterialComponent>(
                MaterialComponent(meshData->materialHandle));
        }

        // If the model has no texture
        else {
            Logger::warn("SPAWNER",
                         "No material found for: " +
                             std::to_string(meshData->materialHandle));

            // Add a default material
            entity.addComponent<MaterialComponent>(MaterialComponent());
        }

        // Push the new entity
        entities.push_back(entityId);
    }

    // Return the final entity list
    return entities;
}

} // namespace Engine
