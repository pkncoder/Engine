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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <iostream>
#include <memory>

namespace Engine {

void DeferredRenderer::generateShadowMap() {

    // Generate 4 textures and 4 FBOs
    glGenTextures(4, shadowCubemap);
    glGenFramebuffers(4, shadowFBO);

    for (int j = 0; j < 4; ++j) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemap[j]);

        // Create each cubemap face
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                         GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT,
                         GL_FLOAT, nullptr);
        }

        // Texture params
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        // FBO setup
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[j]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             shadowCubemap[j], 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::init(EngineState &state) {

    // Flag for the cubemap
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Default textures
    {
        const auto setupDefaultTexture = [&](GLuint *texture,
                                             unsigned char pixel[],
                                             GLenum format) {
            glGenTextures(1, texture);
            glBindTexture(GL_TEXTURE_2D, *texture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, 1, 1, 0, format,
                         GL_UNSIGNED_BYTE, pixel);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        };

        unsigned char whitePixel[] = {255, 255, 255, 255};
        setupDefaultTexture(&defaultWhiteTexture, whitePixel, GL_RGBA);

        unsigned char redPixel[] = {255};
        setupDefaultTexture(&defaultGrayscaleTexture, redPixel, GL_RED);

        unsigned char flatNormalPixel[] = {128, 128, 255, 255};
        setupDefaultTexture(&defaultNormalTexture, flatNormalPixel, GL_RGBA);
    }

    generateShadowMap();

    // Shadow mapping shaders
    shadowProgram = IProgram();
    shadowProgram.attachShader(
        IShader("shaders/deferred/main/shadow.vert", Vertex));
    shadowProgram.attachShader(
        IShader("shaders/deferred/main/shadow.geom", Geometry));
    shadowProgram.attachShader(
        IShader("shaders/deferred/main/shadow.frag", Fragment));
    shadowProgram.link();

    // G-Buffer shaders
    gBufferProgram = IProgram();
    gBufferProgram.attachShader(
        IShader("shaders/deferred/main/gBuffer.vert", Vertex));
    gBufferProgram.attachShader(
        IShader("shaders/deferred/main/gBuffer.frag", Fragment));
    gBufferProgram.link();

    // Lighting shaders
    lightingProgram = IProgram();
    lightingProgram.attachShader(
        IShader("shaders/deferred/main/lighting.vert", Vertex));
    lightingProgram.attachShader(
        IShader("shaders/deferred/main/lighting.frag", Fragment));
    lightingProgram.link();

    // Post processing shaders
    postProgram = IProgram();
    postProgram.attachShader(
        IShader("shaders/deferred/main/post.vert", Vertex));
    postProgram.attachShader(
        IShader("shaders/deferred/main/post.frag", Fragment));
    postProgram.link();

    // Camera UBO creation & mapping
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
        BufferManager::mapUBO(
            cameraUBO, postProgram.ID, "CameraUBO",
            {"uCameraPos", "uViewProjection", "uInverseView"});
    }

    // Light UBO creation & mapping
    {
        lightUBO = BufferManager::createBuffer(
            "PointLightUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
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

    if (newWidth == currentWidth && newHeight == currentHeight)
        return;
    if (newWidth == 0 || newHeight == 0)
        return;

    IRenderer::resize(newWidth, newHeight);

    // Wipe the render targets
    clearAllRenderTargets();

    // Invalidate local handles so they are rebuilt in prepare()
    gBufferHandle = INVALID_RENDER_TARGET;
    finalOutputHandle = INVALID_RENDER_TARGET;
    postProcessedOutHandle = INVALID_RENDER_TARGET;
}

void DeferredRenderer::beginFrame(EngineState &state) { frameIndex++; }

void DeferredRenderer::extract(EngineState &state) {

    // Wipe for new commands
    opaqueCommands.clear();
    pointLights.clear();

    // Get the registry
    entt::registry &sceneRegistry =
        engineContext.getScene()->getScene().getRegistry();

    // Get the camera and camera transform component from the active camera
    auto [camera, cameraTransform] =
        sceneRegistry.try_get<CameraComponent, TransformComponent>(
            engineContext.getScene()->getScene().activeCameraID);

    // Set the camera data
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

    // Lambda to help build the model matrix
    const auto buildModel = [](const TransformComponent &t) {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, t.position);
        m *= glm::mat4_cast(t.rotation);
        m = glm::scale(m, t.scale);
        return m;
    };

    // Get the renderables
    auto renderables =
        engineContext.getScene()
            ->getScene()
            .getRegistry()
            .view<TransformComponent, MeshComponent, MaterialComponent>();

    // Loop each one
    for (auto [entity, transform, mesh, material] : renderables.each()) {
        RasterDrawCommand cmd;
        // cmd.vao = mesh.vao;
        // cmd.indexCount = mesh.indexCount;
        // cmd.modelMatrix = transform.getWorldMatrix();
        // cmd.materialData = material.getProperties();

        // Get the GPU mesh and set the relevent data
        GPUMesh *gpuMesh = GPUResourceManager::getOrUploadMesh(mesh.handle);
        cmd.vao = gpuMesh->vao;
        cmd.indexCount = gpuMesh->indexCount;
        cmd.modelMatrix = buildModel(transform); // Build the model matrix

        // Get the material data amd set the base material values
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

        // Set a bunch of textures
        cmd.textures[0] = getTexID("albedo", defaultWhiteTexture);
        cmd.textures[1] = getTexID("emissive", defaultWhiteTexture);
        cmd.textures[2] = getTexID("alpha", defaultGrayscaleTexture);
        cmd.textures[3] = getTexID("roughness", defaultWhiteTexture);
        cmd.textures[4] = getTexID("metallic", defaultWhiteTexture);
        cmd.textures[5] = getTexID("normal", defaultNormalTexture);
        cmd.textures[6] = getTexID("bump", defaultGrayscaleTexture);
        // cmd.textures[7] = shadowCubeMap;

        // cmd.isBumpMap = (cmd.textures[6] != defaultWhiteTexture &&
        //                  cmd.textures[5] == defaultNormalTexture);
        opaqueCommands.push_back(cmd);
    }

    // Get each light
    auto lightView = engineContext.getScene()
                         ->getScene()
                         .getRegistry()
                         .view<TransformComponent, PointLightComponent>();

    // Add each light + its data to the point lights array
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

    RenderPass shadowPass;
    shadowPass.name = "Point Light Shadow Pass";

    shadowPass.setup = [&]() {};

    shadowPass.execute = [&]() {
        if (pointLights.empty())
            return;

        // Determine how many lights to cast shadows for (max 4)
        int shadowCasters = std::min((int)pointLights.size(), 4);

        glViewport(0, 0, 1024, 1024);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        shadowProgram.bind();
        float nearPlane = 0.001f;
        float farPlane = 100.0f;
        shadowProgram.setFloat("u_FarPlane", farPlane);

        // Loop through each shadow-casting light
        for (int lightIdx = 0; lightIdx < shadowCasters; ++lightIdx) {
            glm::vec3 lightPos = pointLights[lightIdx].position;
            shadowProgram.setVec3("u_LightPos", lightPos);

            // Setup matrices for this specific light
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f,
                                                    nearPlane, farPlane);
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(1.0, 0.0, 0.0),
                                         glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(-1.0, 0.0, 0.0),
                                         glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(0.0, 1.0, 0.0),
                                         glm::vec3(0.0, 0.0, 1.0)));
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(0.0, -1.0, 0.0),
                                         glm::vec3(0.0, 0.0, -1.0)));
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(0.0, 0.0, 1.0),
                                         glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(
                shadowProj * glm::lookAt(lightPos,
                                         lightPos + glm::vec3(0.0, 0.0, -1.0),
                                         glm::vec3(0.0, -1.0, 0.0)));

            for (int i = 0; i < 6; ++i) {
                shadowProgram.setMat4("u_ShadowMatrices[" + std::to_string(i) +
                                          "]",
                                      shadowTransforms[i]);
            }

            // Bind the specific FBO for this light and clear it
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[lightIdx]);
            glClear(GL_DEPTH_BUFFER_BIT);

            // Draw opaque objects
            GLuint currentVAO = 0;
            for (const auto &cmd : opaqueCommands) {
                shadowProgram.setMat4("u_Model", cmd.modelMatrix);
                if (currentVAO != cmd.vao) {
                    glBindVertexArray(cmd.vao);
                    currentVAO = cmd.vao;
                }
                glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT,
                               0);
            }
        }
    };
    renderGraph.addPass(shadowPass);

    // Pass One - G-Buffer
    RenderPass gBufferPass;
    gBufferPass.name = "G-Buffer Pass";

    gBufferPass.setup = [&]() {
        //(Position (F32), Normal (F16), Albedo (UI8), RMA (UI8))
        std::vector<GLenum> gBufferFormats = {GL_RGBA32F, GL_RGBA16F, GL_RGBA8,
                                              GL_RGBA8};
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

        int shadowCasters = std::min((int)pointLights.size(), 4);
        lightingProgram.setInt("u_MaxShadowLights", shadowCasters);

        // Bind G-Buffer MRTs as input textures
        for (size_t i = 0; i < gBuffer->textureIDs.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, gBuffer->textureIDs[i]);
            lightingProgram.setInt("u_GBuffer" + std::to_string(i), i);
        }

        // Bind the shadow cubemaps starting at GL_TEXTURE5
        for (int i = 0; i < 4; ++i) {
            glActiveTexture(GL_TEXTURE5 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubemap[i]);
            lightingProgram.setInt("u_ShadowMaps[" + std::to_string(i) + "]",
                                   5 + i);
        }

        lightingProgram.setFloat("u_FarPlane", 100.0f);

        drawFullscreenQuad();
        for (size_t i = 0; i < gBuffer->textureIDs.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

    renderGraph.addPass(lightingPass);

    RenderPass postProcessingPass;
    postProcessingPass.name = "Post";

    RendererSettings &renderSettings = state.renderer.settings;

    postProcessingPass.setup = [&]() {
        std::vector<GLenum> formats = {GL_RGBA8};
        if (postProcessedOutHandle == INVALID_RENDER_TARGET) {
            postProcessedOutHandle =
                addRenderTarget("PostOutput", formats, false);
        }
    };

    postProcessingPass.execute = [&]() {
        RenderTarget *gBuffer = getRenderTarget(gBufferHandle);
        RenderTarget *finalOut = getRenderTarget(finalOutputHandle);
        RenderTarget *postProcessedOut =
            getRenderTarget(postProcessedOutHandle);

        // Bind and clear the fbos
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessedOut->fbo);
        glViewport(0, 0, currentWidth, currentHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST); // Fullscreen quad doesn't need depth testing

        // Bind the shader
        postProgram.bind();

        // Bind the textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer->textureIDs[0]);
        postProgram.setInt("u_GBuffer" + std::to_string(0), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, finalOut->textureIDs[0]);
        postProgram.setInt("uColorRender", 1);

        postProgram.setVec2("uResolution",
                            glm::vec2(currentWidth, currentHeight));

        // Exposure settings
        postProgram.setFloat("uExposure", renderSettings.exposure);

        // Post effects
        postProgram.setInt("uToneMap", renderSettings.toneMap);
        postProgram.setInt("uSRGB", renderSettings.srgb);

        // Fog settings
        postProgram.setInt("uFog", renderSettings.fog);
        postProgram.setVec3("uFogColor", renderSettings.fogColor);
        postProgram.setFloat("uFogDensity", renderSettings.fogDensity);

        // Vignette settings
        postProgram.setInt("uVignette", renderSettings.vignette);
        postProgram.setFloat("uVignetteRadius", renderSettings.vignetteRadius);
        postProgram.setFloat("uVignetteSoftness",
                             renderSettings.vignetteSoftness);

        drawFullscreenQuad();
    };

    renderGraph.addPass(postProcessingPass);

    // Compile the graph
    renderGraph.compile();
}

void DeferredRenderer::dispatch(EngineState &state) { renderGraph.execute(); }

void DeferredRenderer::present(EngineState &state) {
    RenderTarget *postProcessedOut = getRenderTarget(postProcessedOutHandle);

    if (postProcessedOut && postProcessedOut->fbo != 0) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, postProcessedOut->fbo);
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
