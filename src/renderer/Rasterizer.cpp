#include "Rasterizer.h"
#include "GPUStructs.h"

#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "GPUResourceManager.h"
#include "IRenderer.h"
#include "Shader.h"
#include "buffers/BufferManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <memory>
#include <string>

namespace Engine {

void Rasterizer::setupDefaultTextures() {
    glGenTextures(1, &defaultWhiteTexture);
    glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
    unsigned char whitePixel[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 whitePixel);

    glGenTextures(1, &defaultNormalTexture);
    glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
    unsigned char flatNormalPixel[] = {128, 128, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 flatNormalPixel);
}

void Rasterizer::resize(const uint32_t newWidth, const uint32_t newHeight) {
    if (newWidth == currentWidth && newHeight == currentHeight)
        return;
    currentWidth = newWidth;
    currentHeight = newHeight;
}

void Rasterizer::setupShadowFBO(EngineState &state) {
    glGenTextures(1, &shadowCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);

    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT32F, state.renderer.settings.shadowWidth,
                     state.renderer.settings.shadowHeight, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::error("RENDERER", "Shadow cube map FBO incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Rasterizer::init(EngineState &state) {
    setupDefaultTextures();
    setupShadowFBO(state);

    // Register Passes
    addShaderNode("ShadowPass", "shaders/rasterizing/main/shadow.vert",
                  "shaders/rasterizing/main/shadow.geom",
                  "shaders/rasterizing/main/shadow.frag");

    addShaderNode("RasterPass", "shaders/rasterizing/main/raster.vert",
                  "shaders/rasterizing/main/raster.frag");

    // 1. Allocate UBOs
    cameraUBO = BufferManager::createBuffer(
        "CameraUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
        sizeof(CameraData), nullptr, true);
    globalSceneUBO = BufferManager::createBuffer(
        "GlobalSceneUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
        sizeof(GlobalSceneData), nullptr, true);
    objectUBO = BufferManager::createBuffer(
        "ObjectUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
        sizeof(ObjectRenderData), nullptr, true);

    // 2. Map Layouts (Ensuring your GLSL uses these exact block names)
    // Shadow Pass (Index 0)
    BufferManager::mapUBO(globalSceneUBO, shaderNodeTree[0].shader.ID,
                          "GlobalSceneUBO",
                          {"uShadowMatrices", "uLightPos", "uResolution",
                           "uFOV", "uShadowFarPlane"});
    BufferManager::mapUBO(objectUBO, shaderNodeTree[0].shader.ID, "ObjectUBO",
                          {"uModel"});

    // Main Raster Pass (Index 1)
    BufferManager::mapUBO(cameraUBO, shaderNodeTree[1].shader.ID, "CameraUBO",
                          {"uCameraPos", "uViewProjection", "uInverseView"});
    BufferManager::mapUBO(globalSceneUBO, shaderNodeTree[1].shader.ID,
                          "GlobalSceneUBO",
                          {"uShadowMatrices", "uLightPos", "uResolution",
                           "uFOV", "uShadowFarPlane"});
    BufferManager::mapUBO(objectUBO, shaderNodeTree[1].shader.ID, "ObjectUBO",
                          {"uModel", "uAlbedo", "uEmissive", "uRoughness",
                           "uMetallic", "uIsBumpMap"});

    // Setup Texture Sampler Uniforms (These remain standard uniforms)
    glUseProgram(shaderNodeTree[1].shader.ID);
    shaderNodeTree[1].shader.setInt("uAlbedoMap", 0);
    shaderNodeTree[1].shader.setInt("uEmissiveMap", 1);
    shaderNodeTree[1].shader.setInt("uAlphaMap", 2);
    shaderNodeTree[1].shader.setInt("uRoughnessMap", 3);
    shaderNodeTree[1].shader.setInt("uMetallicMap", 4);
    shaderNodeTree[1].shader.setInt("uNormalMap", 5);
    shaderNodeTree[1].shader.setInt("uBumpMap", 6);
    shaderNodeTree[1].shader.setInt("uShadowCubeMap", 7);
    glUseProgram(0);

    Logger::info("RENDERER", "Rasterizer initialized.");
}

void Rasterizer::beginFrame(EngineState &state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderPackets.clear();
    frameIndex++;
}

void Rasterizer::extract(EngineState &state) {
    // ... [No changes needed in extract] ...
    AssetManager *assetManager = engineContext.getAsset();
    SceneManager *sceneManager = engineContext.getScene();
    Scene &scene = sceneManager->getScene();
    Camera &camera = sceneManager->getCamera();

    if (currentHeight > 0) {
        cameraData.position = glm::vec4(camera.position, 0.0);
        cameraData.viewProjection =
            camera.getProjectionMatrix((float)currentWidth /
                                       (float)currentHeight) *
            camera.getViewMatrix();
        cameraData.inverseView = glm::inverse(camera.getViewMatrix());
    }
    activeLightPos = state.scene.camera.position;

    auto buildModel = [](const TransformComponent &t) {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, t.position);
        m *= glm::mat4_cast(t.rotation);
        m = glm::scale(m, t.scale);
        return m;
    };

    const auto renderables =
        scene.getMatchingEntities<TransformComponent, MeshComponent,
                                  MaterialComponent>();
    float bestEmissive = 0.0f;

    for (EntityID id : renderables) {
        const auto &transform = scene.getComponent<TransformComponent>(id);
        const auto &mesh = scene.getComponent<MeshComponent>(id);
        const auto &material = scene.getComponent<MaterialComponent>(id);

        auto cpuMat = assetManager->getMaterial(material.handle);
        if (glm::length(cpuMat->emissive) > 0.001 && cpuMat &&
            glm::length(cpuMat->emissive) > bestEmissive) {
            bestEmissive = glm::length(cpuMat->emissive);
            activeLightPos = transform.position;
        }
        renderPackets.push_back(
            {mesh.handle, material.handle, buildModel(transform)});
    }

    std::sort(renderPackets.begin(), renderPackets.end(),
              [](const RenderPacket &a, const RenderPacket &b) {
                  return a.materialHandle < b.materialHandle;
              });
}

void generateShadowMatricies(EngineState &state, RenderLayer *shadowStep,
                             glm::vec3 lightPos) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f,
                                            state.renderer.settings.shadowNear,
                                            state.renderer.settings.shadowFar);

    const glm::vec3 faceDirections[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                         {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
    const glm::vec3 faceUps[6] = {{0, -1, 0}, {0, -1, 0}, {0, 0, 1},
                                  {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};

    for (int face = 0; face < 6; face++) {
        glm::mat4 lightView = glm::lookAt(
            lightPos, lightPos + faceDirections[face], faceUps[face]);
        shadowStep->shadowTransforms.push_back(shadowProj * lightView);
    }
}

void Rasterizer::prepare(EngineState &state) {
    // 1. Push Camera Data
    BufferManager::setUBOValue(cameraUBO, "uCameraPos", sizeof(glm::vec4),
                               glm::value_ptr(cameraData.position));
    BufferManager::setUBOValue(cameraUBO, "uViewProjection", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.viewProjection));
    BufferManager::setUBOValue(cameraUBO, "uInverseView", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.inverseView));
    BufferManager::pushBuffer(cameraUBO, frameIndex);

    shaderNodeTree[0].renderSteps.clear();
    shaderNodeTree[1].renderSteps.clear();

    RenderLayer shadowStep;
    shadowStep.fbo = shadowFBO;
    shadowStep.renderWidth = state.renderer.settings.shadowWidth;
    shadowStep.renderHeight = state.renderer.settings.shadowHeight;
    shadowStep.isShadowPass = true;
    generateShadowMatricies(state, &shadowStep, activeLightPos);

    RenderLayer mainStep;
    mainStep.fbo = 0;
    mainStep.renderWidth = currentWidth;
    mainStep.renderHeight = currentHeight;
    mainStep.isShadowPass = false;

    // 2. Prepare Global Scene UBO Data
    GlobalSceneData globalData = {};
    for (int i = 0; i < 6; i++) {
        globalData.shadowMatrices[i] = shadowStep.shadowTransforms[i];
    }
    globalData.lightPos = glm::vec4(activeLightPos, 1.0f);
    globalData.resolution = glm::vec2(currentWidth, currentHeight);
    globalData.fov = state.scene.camera.fov;
    globalData.shadowFarPlane = state.renderer.settings.shadowFar;

    // We can map the whole struct directly into the UBO instead of setting
    // variables one by one Assuming your BufferManager has a way to upload raw
    // bytes to the CPU cache. If not, map individually:
    BufferManager::setUBOValue(globalSceneUBO, "uShadowMatrices",
                               sizeof(glm::mat4) * 6,
                               glm::value_ptr(globalData.shadowMatrices[0]));
    BufferManager::setUBOValue(globalSceneUBO, "uLightPos", sizeof(glm::vec4),
                               glm::value_ptr(globalData.lightPos));
    BufferManager::setUBOValue(globalSceneUBO, "uResolution", sizeof(glm::vec2),
                               glm::value_ptr(globalData.resolution));
    BufferManager::setUBOValue(globalSceneUBO, "uFOV", sizeof(float),
                               &globalData.fov);
    BufferManager::setUBOValue(globalSceneUBO, "uShadowFarPlane", sizeof(float),
                               &globalData.shadowFarPlane);
    BufferManager::pushBuffer(globalSceneUBO, frameIndex);

    for (const auto &packet : renderPackets) {
        GPUMesh *gpuMesh =
            GPUResourceManager::getOrUploadMesh(packet.meshHandle);
        CPUMaterialData *cpuMaterial =
            engineContext.getAsset()->getMaterial(packet.materialHandle).get();

        if (!gpuMesh || !cpuMaterial)
            continue;

        auto getTexID = [&](const std::string &key, GLuint fallback) -> GLuint {
            auto ittr = cpuMaterial->textureMaps.find(key);
            if (ittr != cpuMaterial->textureMaps.end()) {
                GPUTexture *tex =
                    GPUResourceManager::getOrUploadTexture(ittr->second);
                if (tex && tex->textureID != 0)
                    return tex->textureID;
            }
            return fallback;
        };

        /* ----------------- Shadow Pass Command ----------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand =
                std::make_shared<RasterDrawCommand>();
            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;
            drawCommand->textures[2] = getTexID("alpha", defaultWhiteTexture);
            shadowStep.commands.push_back(drawCommand);
        }

        /* ----------------- Main Pass Command ---------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand =
                std::make_shared<RasterDrawCommand>();
            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;

            drawCommand->albedo = cpuMaterial->albedo;
            drawCommand->emissive = cpuMaterial->emissive;
            drawCommand->roughness = cpuMaterial->roughness;
            drawCommand->metallic = cpuMaterial->metallic;

            drawCommand->textures[0] = getTexID("albedo", defaultWhiteTexture);
            drawCommand->textures[1] =
                getTexID("emissive", defaultWhiteTexture);
            drawCommand->textures[2] = getTexID("alpha", defaultWhiteTexture);
            drawCommand->textures[3] =
                getTexID("roughness", defaultWhiteTexture);
            drawCommand->textures[4] =
                getTexID("metallic", defaultWhiteTexture);
            drawCommand->textures[5] = getTexID("normal", defaultNormalTexture);
            drawCommand->textures[6] = getTexID("bump", defaultWhiteTexture);
            drawCommand->textures[7] = shadowCubeMap;

            drawCommand->isBumpMap =
                (drawCommand->textures[6] != defaultWhiteTexture &&
                 drawCommand->textures[5] == defaultNormalTexture);

            mainStep.commands.push_back(drawCommand);
        }
    }

    shaderNodeTree[0].renderSteps.push_back(shadowStep);
    shaderNodeTree[1].renderSteps.push_back(mainStep);
}

void Rasterizer::dispatch(EngineState &state) {
    for (const ShaderNode &pass : shaderNodeTree) {

        pass.shader.bind();

        for (const RenderLayer &layer : pass.renderSteps) {

            glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
            glViewport(0, 0, layer.renderWidth, layer.renderHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (layer.isShadowPass) {
                glCullFace(GL_FRONT);
            } else {
                glCullFace(GL_BACK);
            }

            GLuint currentVAO = 0;

            for (const std::shared_ptr<DrawCommand> cmd : layer.commands) {
                const auto command =
                    std::static_pointer_cast<RasterDrawCommand>(cmd);

                // Update Per-Object UBO
                BufferManager::setUBOValue(
                    objectUBO, "uModel", sizeof(glm::mat4),
                    glm::value_ptr(command->modelMatrix));

                if (!layer.isShadowPass) {
                    // Note: padding vec3s up to vec4s to satisfy std140 layout
                    glm::vec4 padAlbedo = glm::vec4(command->albedo, 1.0f);
                    glm::vec4 padEmissive = glm::vec4(command->emissive, 1.0f);

                    BufferManager::setUBOValue(objectUBO, "uAlbedo",
                                               sizeof(glm::vec4),
                                               glm::value_ptr(padAlbedo));
                    BufferManager::setUBOValue(objectUBO, "uEmissive",
                                               sizeof(glm::vec4),
                                               glm::value_ptr(padEmissive));
                    BufferManager::setUBOValue(objectUBO, "uRoughness",
                                               sizeof(float),
                                               &command->roughness);
                    BufferManager::setUBOValue(objectUBO, "uMetallic",
                                               sizeof(float),
                                               &command->metallic);
                    BufferManager::setUBOValue(objectUBO, "uIsBumpMap",
                                               sizeof(int),
                                               &command->isBumpMap);
                }

                // Push the object UBO memory to the GPU per draw call
                BufferManager::pushBuffer(objectUBO, frameIndex);

                // Bind Textures
                for (int i = 0; i < 8; ++i) {
                    if (command->textures[i] != 0) {
                        glActiveTexture(GL_TEXTURE0 + i);
                        GLuint target =
                            (i == 7) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
                        glBindTexture(target, command->textures[i]);
                    }
                }

                // Draw
                if (currentVAO != command->vao) {
                    glBindVertexArray(command->vao);
                    currentVAO = command->vao;
                }

                glDrawElements(GL_TRIANGLES, command->indexCount,
                               GL_UNSIGNED_INT, 0);
            }
        }
        pass.shader.unbind();
    }
}

void Rasterizer::postProcess(EngineState &state) {}

void Rasterizer::present(EngineState &state) {}

void Rasterizer::shutdown() {
    glDeleteFramebuffers(1, &shadowFBO);
    glDeleteTextures(1, &shadowCubeMap);
    glDeleteTextures(1, &defaultWhiteTexture);
    glDeleteTextures(1, &defaultNormalTexture);

    Logger::info("RENDERER", "Rasterizer shutdown complete.");
}

} // namespace Engine
