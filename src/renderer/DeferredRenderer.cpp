#include "DeferredRenderer.h"

#include "../scene/Scene.h"
#include "../scene/SceneManager.h"
#include "../scene/components/CameraComponent.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/PointLightComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "GPUResourceManager.h"
#include "GPUStructs.h"
#include "IRenderer.h"
#include "RenderGraph.h"
#include "buffers/BufferManager.h"
#include "buffers/GPUBuffer.h"
#include "shaders/IProgram.h"
#include "shaders/IShader.h"

#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <memory>

namespace Engine {

void DeferredRenderer::init(EngineState &state) {

    {
        // Default white
        glGenTextures(1, &defaultWhiteTexture);
        glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
        unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, whitePixel);

        // Default black
        glGenTextures(1, &defaultNormalTexture);
        glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
        unsigned char flatNormalPixel[] = {128, 128, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, flatNormalPixel);
    }

    gBufferProgram = IProgram();
    gBufferProgram.attachShader(
        IShader("shaders/deferred/main/gBuffer.vert", Vertex));
    gBufferProgram.attachShader(
        IShader("shaders/deferred/main/gBuffer.frag", Fragment));
    gBufferProgram.link();

    lightingProgram = IProgram();
    lightingProgram.attachShader(
        IShader("shaders/deferred/main/lighting.vert", Vertex));
    lightingProgram.attachShader(
        IShader("shaders/deferred/main/lighting.frag", Fragment));
    lightingProgram.link();

    {
        cameraUBO = BufferManager::createBuffer(
            "CameraUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
            sizeof(CameraData), -1, nullptr, true);

        BufferManager::mapUBO(
            cameraUBO, gBufferProgram.ID, "CameraUBO",
            {"uCameraPos", "uViewProjection", "uInverseView"});

        BufferManager::mapUBO(
            cameraUBO, lightingProgram.ID, "CameraUBO",
            {"uCameraPos", "uViewProjection", "uInverseView"});
    }

    {
        lightUBO = BufferManager::createBuffer(
            "LightUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
            sizeof(LightUBOData), -1, nullptr, true);

        BufferManager::mapUBO(lightUBO, lightingProgram.ID, "PointLightUBO",
                              {"uPointLights", "pointLightCount"});
    }
}

void DeferredRenderer::shutdown() {
    IRenderer::shutdown();
    renderGraph.clear();
}

void DeferredRenderer::resize(uint32_t newWidth, uint32_t newHeight) {
    IRenderer::resize(newWidth,
                      newHeight); // Updates currentWidth/currentHeight

    // You will need to implement this in IRenderer to delete the
    // glGenFramebuffers and glGenTextures associated with your cached MRTs.
    clearAllRenderTargets();

    // Invalidate local handles so they are forcefully rebuilt in prepare()
    gBufferHandle = INVALID_RENDER_TARGET;
    finalOutputHandle = INVALID_RENDER_TARGET;
}

void DeferredRenderer::beginFrame(EngineState &state) { frameIndex++; }

void DeferredRenderer::extract(EngineState &state) {

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

        GPUMesh *gpuMesh = GPUResourceManager::getOrUploadMesh(mesh.handle);
        cmd.vao = gpuMesh->vao;
        cmd.indexCount = gpuMesh->indexCount;
        cmd.modelMatrix = buildModel(transform);

        std::shared_ptr<CPUMaterialData> cpuMaterial =
            engineContext.getAsset()->getMaterial(material.handle);
        cmd.albedo = cpuMaterial->albedo;
        cmd.emissive = cpuMaterial->emissive;
        cmd.roughness = cpuMaterial->roughness;
        cmd.metallic = cpuMaterial->metallic;

        // Lambda for getting a texture id safely
        const auto getTexID = [&](const std::string &key,
                                  const GLuint fallback) -> GLuint {
            auto ittr = cpuMaterial->textureMaps.find(key);
            if (ittr != cpuMaterial->textureMaps.end()) {
                GPUTexture *tex =
                    GPUResourceManager::getOrUploadTexture(ittr->second);
                if (tex && tex->textureID != 0)
                    return tex->textureID;
            }
            return fallback;
        };

        cmd.textures[0] = getTexID("albedo", defaultWhiteTexture);
        cmd.textures[1] = getTexID("emissive", defaultWhiteTexture);
        cmd.textures[2] = getTexID("alpha", defaultWhiteTexture);
        cmd.textures[3] = getTexID("roughness", defaultWhiteTexture);
        cmd.textures[4] = getTexID("metallic", defaultWhiteTexture);
        cmd.textures[5] = getTexID("normal", defaultNormalTexture);
        cmd.textures[6] = getTexID("bump", defaultWhiteTexture);
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

void DeferredRenderer::prepare(EngineState &state) {

    // -----------------  Update Unfiroms -----------------

    { // Camera
        BufferManager::updateBufferCache(
            cameraUBO, sizeof(CameraData), 0,
            reinterpret_cast<const void *>(&cameraData));
        BufferManager::pushBuffer(cameraUBO, frameIndex);
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
        BufferManager::pushBuffer(lightUBO, frameIndex);
    }

    // ----------------- Render Graph -----------------

    // Clear the render graph to rebuild
    renderGraph.clear();

    // Pass One - G-Buffer
    RenderPass gBufferPass;
    gBufferPass.name = "G-Buffer Pass";

    gBufferPass.setup = [&]() {
        //(Position (F32), Normal (F16), Albedo (UI8), RMA (UI8))
        std::vector<GLenum> gBufferFormats = {GL_RGBA32F, GL_RGBA16F, GL_RGBA8,
                                              GL_RGBA8, GL_RGBA16F};
        if (gBufferHandle == INVALID_RENDER_TARGET) {
            gBufferHandle = addRenderTarget("GBuffer", gBufferFormats, true);
        }

        // Build draw commands
    };

    gBufferPass.execute = [&]() {
        // Get the render targets
        RenderTarget *gBuffer = getRenderTarget(gBufferHandle);

        // Bind and clear the fbos
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->fbo);
        glViewport(0, 0, currentWidth, currentHeight);

        // Clear all bound MRT attachments and depth
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable gl settings
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);

        // Bind the shader
        gBufferProgram.bind();

        // Tell the shader which texture units correspond to which samplers
        gBufferProgram.setInt("u_AlbedoMap", 0);
        gBufferProgram.setInt("u_EmissiveMap", 1);
        gBufferProgram.setInt("u_AlphaMap", 2);
        gBufferProgram.setInt("u_NormalMap", 3);
        gBufferProgram.setInt("u_BumpMap", 4);

        // Draw
        GLuint currentVAO = 0;
        for (const auto &cmd : opaqueCommands) {

            // 1. Pass matrices
            gBufferProgram.setMat4("u_Model", cmd.modelMatrix);

            // 2. Bind Textures to their respective units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cmd.textures[0]); // Albedo

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, cmd.textures[1]); // Emissive

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cmd.textures[2]); // Alpha

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, cmd.textures[5]); // Normal

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, cmd.textures[6]); // Bump

            // 3. Pass Material Fallbacks/Data
            gBufferProgram.setVec3("u_AlbedoColor", cmd.albedo);
            gBufferProgram.setVec3("u_EmissiveColor", cmd.emissive);
            gBufferProgram.setFloat("u_Roughness", cmd.roughness);
            gBufferProgram.setFloat("u_Metallic", cmd.metallic);

            // 4. Draw
            if (currentVAO != cmd.vao) {
                glBindVertexArray(cmd.vao);
                currentVAO = cmd.vao;
            }
            glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT, 0);
        }
    };

    renderGraph.addPass(gBufferPass);

    // Pass Two - Defered lighting
    RenderPass lightingPass;
    lightingPass.name = "Lighting Pass";

    lightingPass.setup = [&]() {
        // Set the render targets
        std::vector<GLenum> finalFormats = {GL_RGBA8};
        if (finalOutputHandle == INVALID_RENDER_TARGET) {
            Logger::check();
            finalOutputHandle =
                addRenderTarget("FinalOutput", finalFormats, false);
        }
    };

    lightingPass.execute = [&]() {
        // Get the render targets
        RenderTarget *finalOut = getRenderTarget(finalOutputHandle);
        RenderTarget *gBuffer = getRenderTarget(gBufferHandle);

        // Bind and clear the fbos
        glBindFramebuffer(GL_FRAMEBUFFER, finalOut->fbo);
        glViewport(0, 0, currentWidth, currentHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST); // Fullscreen quad doesn't need depth testing

        // Bind the shader
        lightingProgram.bind();

        // Pass shadow constant
        lightingProgram.setInt("u_MaxShadowLights",
                               4); // TODO: static var

        // Bind G-Buffer MRTs as input textures
        for (size_t i = 0; i < gBuffer->textureIDs.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, gBuffer->textureIDs[i]);
            lightingProgram.setInt("u_GBuffer" + std::to_string(i), i);
        }

        // Draw Fullscreen Quad
        drawFullscreenQuad();
    };

    renderGraph.addPass(lightingPass);

    // Compile the graph
    renderGraph.compile();
}

void DeferredRenderer::dispatch(EngineState &state) { renderGraph.execute(); }

void DeferredRenderer::present(EngineState &state) {
    RenderTarget *finalOut = getRenderTarget(finalOutputHandle);

    if (finalOut && finalOut->fbo != 0) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, finalOut->fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default screen FBO

        glBlitFramebuffer(
            0, 0, currentWidth, currentHeight, // Source rect
            0, 0, currentWidth, currentHeight, // Destination rect
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST // Nearest filtering is fine for exact 1:1 copies
        );
    }
}

} // namespace Engine
