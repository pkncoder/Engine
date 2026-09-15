#include "DeferedRenderer.h"

#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../scene/components/CameraComponent.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/PointLightComponent.h"
#include "../scene/components/TransformComponent.h"
#include "GPUStructs.h"
#include "IRenderer.h"
#include "RenderGraph.h"
#include "buffers/BufferManager.h"
#include <cstdint>
#include <entt/entity/fwd.hpp>

namespace Engine {

void DeferedRenderer::init(EngineState &state) {}

void DeferedRenderer::shutdown() {
    IRenderer::shutdown();
    renderGraph.clear();
}

void DeferedRenderer::resize(uint32_t newWidth, uint32_t newHeight) {
    IRenderer::resize(newWidth, newHeight);
}

void DeferedRenderer::beginFrame(EngineState &state) {}

void DeferedRenderer::extract(EngineState &state) {

    // 1. Clear previous frame's commands
    opaqueCommands.clear();
    pointLights.clear();

    entt::registry &sceneRegistry =
        engineContext.getScene()->getScene().getRegistry();

    auto [camera, cameraTransform] =
        sceneRegistry.try_get<CameraComponent, TransformComponent>(
            engineContext.getScene()->getScene().activeCameraID);

    if (currentHeight > 0) {
        cameraData.position = glm::vec4(cameraTransform->position, 1.0f);

        glm::mat4 viewMat =
            glm::lookAt(cameraTransform->position,
                        cameraTransform->position + camera->front, camera->up);

        glm::mat4 projMat =
            glm::perspective(glm::radians(camera->fov),
                             (float)currentWidth / (float)currentHeight,
                             camera->nearPlane, camera->farPlane);

        cameraData.viewProjection = projMat * viewMat;
        cameraData.inverseView = glm::inverse(viewMat);
    }

    const auto buildModel = [](const TransformComponent &t) {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, t.position);
        m *= glm::mat4_cast(t.rotation);
        m = glm::scale(m, t.scale);
        return m;
    };

    // 2. Extract Geometry (Opaque objects only for now)
    auto meshView =
        engineContext.getScene()
            ->getScene()
            .getRegistry()
            .view<TransformComponent, MeshComponent, MaterialComponent>();

    for (auto [entity, transform, mesh, material] : meshView.each()) {
        RasterDrawCommand cmd;
        // cmd.vao = mesh.vao;
        // cmd.indexCount = mesh.indexCount;
        // cmd.modelMatrix = transform.getWorldMatrix();
        // cmd.materialData = material.getProperties();

        GPUMesh gpuMesh;
        cmd.vao = gpuMesh.vao;
        cmd.indexCount = gpuMesh.indexCount;
        cmd.modelMatrix = buildModel(transform);

        CPUMaterialData cpuMaterial;
        cmd.albedo = cpuMaterial.albedo;
        cmd.emissive = cpuMaterial.emissive;
        cmd.roughness = cpuMaterial.roughness;
        cmd.metallic = cpuMaterial.metallic;

        // cmd.textures[0] = getTexID("albedo", defaultWhiteTexture);
        // cmd.textures[1] = getTexID("emissive", defaultWhiteTexture);
        // cmd.textures[2] = getTexID("alpha", defaultWhiteTexture);
        // cmd.textures[3] = getTexID("roughness", defaultWhiteTexture);
        // cmd.textures[4] = getTexID("metallic", defaultWhiteTexture);
        // cmd.textures[5] = getTexID("normal", defaultNormalTexture);
        // cmd.textures[6] = getTexID("bump", defaultWhiteTexture);
        // cmd.textures[7] = shadowCubeMap;

        // cmd.isBumpMap = (cmd.textures[6] != defaultWhiteTexture &&
        //                  cmd.textures[5] == defaultNormalTexture);
        opaqueCommands.push_back(cmd);
    }

    // 3. Extract Lights (Up to our UBO limits)
    auto lightView = engineContext.getScene()
                         ->getScene()
                         .getRegistry()
                         .view<TransformComponent, PointLightComponent>();

    for (auto [entity, transform, light] : lightView.each()) {
        PointLightData lightData;
        lightData.position = transform.position;
        lightData.emissive = light.emissive;
        lightData.intensity = light.intensity;

        pointLights.push_back(lightData);
    }
}

void DeferedRenderer::prepare(EngineState &state) {

    // -----------------  Update Unfiroms -----------------

    { // Camera
        BufferManager::updateBufferCache(
            cameraUBO, sizeof(CameraData), 0,
            reinterpret_cast<const void *>(&cameraData));
    }

    { // Lights
        LightUBOData lightUBOData;
        lightUBOData.lightCount =
            std::min((int)pointLights.size(), 4); // TODO: Static var
        for (int i = 0; i < lightUBOData.lightCount; ++i) {
            lightUBOData.lights[i] = pointLights[i];
        }
        BufferManager::updateBufferCache(
            lightUBO, sizeof(LightUBOData), 0,
            reinterpret_cast<const void *>(&lightUBOData));
    }

    // ----------------- Render Graph -----------------

    // Clear the render graph to rebuild
    renderGraph.clear();

    // Pass One - G-Buffer
    RenderPass gBufferPass;
    gBufferPass.name = "G-Buffer Pass";

    gBufferPass.setup = [&]() {
        // Set render targets

        // Build draw commands
    };

    gBufferPass.execute = [&]() {
        // Get the gBuffer

        // Bind and clear the fbo

        // Bind shader

        // Loop each element and draw
    };

    renderGraph.addPass(gBufferPass);

    // Pass Two - Defered lighting
    RenderPass lightingPass;
    lightingPass.name = "Lighting Pass";

    lightingPass.setup = [&]() {
        // Set the render targets
    };

    lightingPass.execute = [&]() {
        // Get the render targets

        // Bind and clear the fbos

        // Bind the shader

        // Draw
    };

    renderGraph.addPass(lightingPass);

    // Compile the graph
    renderGraph.compile();
}

void DeferedRenderer::dispatch(EngineState &state) { renderGraph.execute(); }

// TODO: Remove postProcess, this is done through the render graph
void DeferedRenderer::postProcess(EngineState &state) {}

void DeferedRenderer::present(EngineState &state) {}

} // namespace Engine
