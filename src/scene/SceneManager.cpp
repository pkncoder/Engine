#include "SceneManager.h"

#include "../services/Logger.h"
#include "JsonGLMHelper.h"
#include "PrefabSpawner.h"
#include "components/CameraComponent.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

namespace Engine {

void SceneManager::update() { cameraSystem.update(scene.getRegistry()); }

std::vector<entt::entity>
SceneManager::loadObjScene(const std::string &sourceDirectory,
                           const std::string &filename) {
    Logger::warn("SCENE", "Please use the new activeScene.json file instead");
    return PrefabSpawner::spawnObjEntity(scene, *engineContext.getAsset(),
                                         sourceDirectory, filename);
}

Scene &SceneManager::loadJsonScene(const std::string &filepath) {

    // Try to open the json file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::error("SCENE", "Failed to open json file");
        return scene;
    }

    // Try to parse and load the scene
    nlohmann::json jsonSceneData;
    try {
        file >> jsonSceneData;
    } catch (const nlohmann::json::parse_error &e) {
        Logger::error("SCENE", "JSON parsing error: ");
        std::cerr << e.what();
        return scene;
    }

    // Clear the scene registry
    entt::registry &registry = scene.getRegistry();
    registry.clear();

    // Load each camera entity
    std::vector<entt::entity> cameraEntities;
    if (jsonSceneData.contains("cameras") &&
        jsonSceneData["cameras"].is_array()) {

        // Loop each camera
        for (const auto &cameraJson : jsonSceneData["cameras"]) {

            // Create a new entity
            entt::entity entity = registry.create();

            // Default camera component
            CameraComponent cameraComponent;

            // Set the attributes
            cameraComponent.fov = cameraJson.value("fov", 60.0f);
            cameraComponent.yaw = cameraJson.value("yaw", 0.0f);
            cameraComponent.pitch = cameraJson.value("pitch", 0.0f);
            cameraComponent.sensitivity = cameraJson.value("sensitivity", 0.2f);
            cameraComponent.movementSpeed =
                cameraJson.value("movementSpeed", 7.0f);

            // Emplace the new camera component
            registry.emplace<CameraComponent>(entity, cameraComponent);

            // Default transform component
            TransformComponent transformComponent;

            // Check if position exists & set it
            if (cameraJson.contains("position")) {
                transformComponent.position =
                    cameraJson["position"].get<glm::vec3>();
            }

            // Emplace the new transform component
            registry.emplace<TransformComponent>(entity, transformComponent);

            // Save this entity so we can set the active camera index
            cameraEntities.push_back(entity);
        }
    }

    // Get the active camera index & check bounds before setting it
    int activeCameraIndex = jsonSceneData.value("activeCameraIndex", 0);
    if (activeCameraIndex >= 0 &&
        activeCameraIndex < static_cast<int>(cameraEntities.size())) {
        engineContext.getScene()->getScene().setActiveCameraID(
            cameraEntities[activeCameraIndex]);
    }

    // Loop each mesh in the scene
    if (jsonSceneData.contains("meshes") &&
        jsonSceneData["meshes"].is_array()) {
        for (const auto &meshJson : jsonSceneData["meshes"]) {

            // Get the source directory and filename
            std::string sourceDirectory = meshJson.value("sourceDirectory", "");
            std::string filename = meshJson.value("filename", "");

            // Load the new model
            AssetHandle modelHandle =
                engineContext.getAsset()->loadModel(sourceDirectory, filename);

            // Get the data of the model
            std::shared_ptr<CPUModelData> modelData =
                engineContext.getAsset()->getModel(modelHandle);

            // Loop each mesh to create a new renderable
            for (auto &meshHandle : modelData->meshHandles) {

                // Get the mesh data
                auto meshData = engineContext.getAsset()->getMesh(meshHandle);

                // Create the new entity
                entt::entity entity = registry.create();

                // Create a new mesh component and add the entity
                const MeshComponent meshComponent =
                    MeshComponent(meshData->name, meshHandle);
                registry.emplace<MeshComponent>(entity, meshComponent);

                // Transform component
                TransformComponent transformComponent;
                if (meshJson.contains("position")) // Pos
                    transformComponent.position =
                        meshJson["position"].get<glm::vec3>();
                if (meshJson.contains("rotation")) // Rotation
                    transformComponent.rotation =
                        meshJson["rotation"].get<glm::quat>();
                if (meshJson.contains("scale")) // Scale
                    transformComponent.scale =
                        meshJson["scale"].get<glm::vec3>();
                registry.emplace<TransformComponent>(entity,
                                                     transformComponent);

                // Check to see if the mesh has a material handle
                if (meshData->materialHandle != INVALID_ASSET_HANDLE) {

                    // Add a material component to the entity
                    registry.emplace<MaterialComponent>(
                        entity, MaterialComponent(meshData->materialHandle));
                }

                // If the model has no material component
                else {
                    Logger::warn("SCENE",
                                 "No material found for: " +
                                     std::to_string(meshData->materialHandle));

                    // Add a default material
                    registry.emplace<MaterialComponent>(entity,
                                                        MaterialComponent());
                }
            }
        }
    }

    // Loop each point light
    if (jsonSceneData.contains("lights") &&
        jsonSceneData["lights"].is_array()) {
        for (const auto &lightJson : jsonSceneData["lights"]) {

            // Create a new entity
            entt::entity entity = registry.create();

            // Default point light component
            PointLightComponent lightComponent;

            // Check for the color and set it
            if (lightJson.contains("color")) {
                lightComponent.emissive = lightJson["color"].get<glm::vec3>();
            }

            // Set the intensity
            lightComponent.intensity = lightJson.value("intensity", 1.0f);

            // Emplae the point light
            registry.emplace<PointLightComponent>(entity, lightComponent);

            // Default transform component
            TransformComponent transformComponent;
            if (lightJson.contains("position")) { // Check for the position
                transformComponent.position =
                    lightJson["position"].get<glm::vec3>();
            }

            // Emplace the transform component
            registry.emplace<TransformComponent>(entity, transformComponent);
        }
    }

    Logger::info("SCENE", "Sucessfully loaded scene from: " + filepath);
    return scene;
}

} // namespace Engine
