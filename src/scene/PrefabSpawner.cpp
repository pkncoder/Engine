#include "PrefabSpawner.h"

#include "../services/Logger.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"

#include <entt/entity/fwd.hpp>
#include <memory>

namespace Engine {

std::vector<entt::entity>
PrefabSpawner::spawnObjEntity(Scene &scene, AssetManager &assetManager,
                              const std::string &sourceDirectory,
                              const std::string &filename) {

    entt::registry &registry = scene.getRegistry();

    // Collection of all the new entities
    std::vector<entt::entity> entities;

    // Get the mesh data & check to make sure that it loaded right
    const AssetHandle modelHandle =
        assetManager.loadModel(sourceDirectory, filename);

    if (modelHandle == INVALID_ASSET_HANDLE) {
        Logger::error("ASSET",
                      "Failed to load model at: " + sourceDirectory + filename);
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
        const entt::entity entity = registry.create();

        // Create a new mesh component and add the entity
        const MeshComponent meshComponent =
            MeshComponent(meshData->name, meshHandle);
        registry.emplace<MeshComponent>(entity, meshComponent);

        // Apply the default transform component
        registry.emplace<TransformComponent>(entity, TransformComponent());

        // Check to see if the mesh has a material handle
        if (meshData->materialHandle != INVALID_ASSET_HANDLE) {

            // Add a material component to the entity
            registry.emplace<MaterialComponent>(
                entity, MaterialComponent(meshData->materialHandle));
        }

        // If the model has no texture
        else {
            Logger::warn("SPAWNER",
                         "No material found for: " +
                             std::to_string(meshData->materialHandle));

            // Add a default material
            registry.emplace<MaterialComponent>(entity, MaterialComponent());
        }

        // Push the new entity
        entities.push_back(entity);
    }

    // Return the final entity list
    return entities;
}

} // namespace Engine
