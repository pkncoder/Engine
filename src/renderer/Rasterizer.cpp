#include "Rasterizer.h"
#include "GPUStructs.h"

#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "GPUResourceManager.h"
#include "IRenderer.h"
#include "buffers/BufferManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho / lookAt
#include <glm/gtc/type_ptr.hpp>

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

    // If the new size is the same as the old one, do nothing
    if (newWidth == currentWidth && newHeight == currentHeight) {
        return;
    }

    // Update the tracked size
    currentWidth = newWidth;
    currentHeight = newHeight;
}

void Rasterizer::setupShadowFBO(EngineState &state) {
    glGenTextures(1, &shadowCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F,
                     state.renderer.settings.shadowWidth,
                     state.renderer.settings.shadowHeight, 0, GL_RED, GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &shadowDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          state.renderer.settings.shadowWidth,
                          state.renderer.settings.shadowHeight);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, shadowDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::error("RENDERER", "Shadow cube map FBO incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Rasterizer::init(EngineState &state) {
    setupDefaultTextures();
    setupShadowFBO(state);

    // Register Passes into the IRenderer Graph
    addShaderNode("ShadowPass", "shaders/rasterizing/main/shadow.geom",
                  GL_GEOMETRY_SHADER);
    addShaderNode("RasterPass", "shaders/rasterizing/main/raster.vert",
                  "shaders/rasterizing/main/raster.frag");

    // Allocate persistent UBO
    cameraUBO = BufferManager::createBuffer(
        "CameraUBO", BufferType::UniformBuffer, BufferUsage::Dynamic,
        sizeof(CameraData), nullptr, true);

    // NOTE: In modern OpenGL, binding UBOs via layout(std140, binding = 0) in
    // the shader is preferred over manual block lookup, but your BufferManager
    // map layout goes here.
    BufferManager::mapUBOLayout(cameraUBO, 0, "CameraUBO",
                                {"uView", "uProjection"});

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
}

void Rasterizer::extract(EngineState &state) {
    AssetManager *assetManager = engineContext.getAsset();

    SceneManager *sceneManager = engineContext.getScene();
    Scene &scene = sceneManager->getScene();

    if (currentHeight > 0) {
        cameraData.view = sceneManager->getCamera().getViewMatrix();
        cameraData.projection = sceneManager->getCamera().getProjectionMatrix(
            (float)currentWidth / (float)currentHeight);
    }
    activeLightPos = state.scene.camera.position; // Fallback light pos

    // 2. Build Packets
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

        // Temp Light finding logic
        auto cpuMat = assetManager->getMaterial(material.handle);
        if (cpuMat && glm::length(cpuMat->emissive) > bestEmissive) {
            bestEmissive = glm::length(cpuMat->emissive);
            activeLightPos = transform.position;
        }

        renderPackets.push_back(
            {mesh.handle, material.handle, buildModel(transform)});
    }

    // 3. SORT PACKETS BY MATERIAL! (Crucial for performance)
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
    // input: list of render packets (mesh, material, modelMatrix)
    // output: list of draw commands within each shader pass

    BufferManager::setUBOValue(cameraUBO, "uView", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.view));
    BufferManager::setUBOValue(cameraUBO, "uProjection", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.projection));
    // BufferManager::pushBuffer(cameraUBO, frameIndex);

    // TODO: generate shadowFBO?

    shaderNodeTree[0].renderSteps.clear();
    shaderNodeTree[1].renderSteps.clear();

    // Create the render passes
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

    // Loop through the render packets
    for (const auto &packet : renderPackets) {

        GPUMesh *gpuMesh =
            GPUResourceManager::getOrUploadMesh(packet.meshHandle);
        CPUMaterialData *cpuMaterial = engineContext.getAsset()
                                           ->getMaterial(packet.materialHandle)
                                           .get(); // TEMP

        if (!gpuMesh || !cpuMaterial) {
            Logger::warn(
                "RENDERER",
                "Render Packet Skipped (GPU Mesh: " +
                    std::string(gpuMesh == nullptr ? "no" : "yes") +
                    ") \(CPU Material: " +
                    std::string(cpuMaterial == nullptr ? "no" : "yes") + ")");

            continue;
        }

        // Map material textures to GLuints safely
        auto getTexID = [&](const std::string &key, GLuint fallback) -> GLuint {
            auto ittr = cpuMaterial->textureMaps.find(key);
            if (ittr != cpuMaterial->textureMaps.end()) {
                GPUTexture *tex =
                    GPUResourceManager::getOrUploadTexture(ittr->second);
                if (tex && tex->textureID != 0)
                    return tex->textureID;
            }
            return fallback; // Return default white/normal texture instead of 0
        };

        /* ----------------- Shadow Pass ----------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand =
                std::make_shared<RasterDrawCommand>();

            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;

            // TODO: textures? (alpha map)
            drawCommand->textures[2] = getTexID("alpha", defaultWhiteTexture);

            // TODO: assigning uniforms??

            shadowStep.commands.push_back(drawCommand);
        }

        /* ----------------- Main Pass ---------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand =
                std::make_shared<RasterDrawCommand>();

            mainStep.fbo = 0; // Main screen

            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;

            drawCommand->albedo = cpuMaterial->albedo;
            drawCommand->emissive = cpuMaterial->emissive;
            drawCommand->roughness = cpuMaterial->roughness;
            drawCommand->metallic = cpuMaterial->metallic;

            // Main pass setup:
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

            // Calculate bump map logic and store it in the command
            drawCommand->isBumpMap = (drawCommand->textures[6] != 0 &&
                                      drawCommand->textures[5] == 0);

            mainStep.commands.push_back(drawCommand);
        }
    }

    // Push layers into the tree so dispatch can find them
    shaderNodeTree[0].renderSteps.push_back(shadowStep);
    shaderNodeTree[1].renderSteps.push_back(mainStep);
}

void Rasterizer::dispatch(EngineState &state) {
    for (const ShaderNode &pass : shaderNodeTree) {

        glUseProgram(pass.shader.ID);

        pass.shader.setMat4("uViewProjection",
                            cameraData.projection * cameraData.view);
        pass.shader.setVec3("uCameraPos", state.scene.camera.position);
        pass.shader.setFloat("uFOV", state.scene.camera.fov);
        pass.shader.setVec2("uResolution",
                            glm::vec2(currentWidth, currentHeight));

        for (const RenderLayer &layer : pass.renderSteps) {

            glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
            glViewport(0, 0, layer.renderWidth, layer.renderHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (layer.isShadowPass) {
                // Send the 6 matrices to the Geometry Shader uniform array
                for (int i = 0; i < 6; ++i) {
                    pass.shader.setMat4("uShadowMatrices[" + std::to_string(i) +
                                            "]",
                                        layer.shadowTransforms[i]);
                }
                pass.shader.setVec3("uLightPos", activeLightPos);
                pass.shader.setFloat("uFarPlane",
                                     state.renderer.settings.shadowFar);
            } else {
                // Main pass global uniforms
                pass.shader.setVec3("uCameraPos", state.scene.camera.position);
                pass.shader.setFloat("uShadowFarPlane",
                                     state.renderer.settings.shadowFar);
                pass.shader.setVec3("uLightPos", activeLightPos);
            }

            GLuint currentVAO = 0;

            for (const std::shared_ptr<DrawCommand> cmd : layer.commands) {
                const auto command =
                    std::static_pointer_cast<RasterDrawCommand>(cmd);

                // 1. Send Model Matrix
                pass.shader.setMat4("uModel", command->modelMatrix);

                // 2. Send Material Data (Only matters for the main pass, but
                // safe to set if unused)
                if (!layer.isShadowPass) {
                    pass.shader.setVec3("uAlbedo", command->albedo);
                    pass.shader.setVec3("uEmmissive", command->emissive);
                    pass.shader.setFloat("uRoughness", command->roughness);
                    pass.shader.setFloat("uMetallic", command->metallic);
                    pass.shader.setInt("uIsBumpMap",
                                       command->isBumpMap ? 1 : 0);
                }

                // 3. Bind Textures from the array
                for (int i = 0; i < 8; ++i) {
                    if (command->textures[i] != 0) {
                        glActiveTexture(GL_TEXTURE0 + i);
                        GLuint target =
                            (i == 7) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
                        glBindTexture(target, command->textures[i]);
                    }
                }

                // 4. Draw
                if (currentVAO != command->vao) {
                    glBindVertexArray(command->vao);
                    currentVAO = command->vao;
                }

                glDrawElements(GL_TRIANGLES, command->indexCount,
                               GL_UNSIGNED_INT, 0);
            }
        }
    }
}

void Rasterizer::postProcess(EngineState &state) {}

void Rasterizer::present(EngineState &state) {}

void Rasterizer::shutdown() {
    glDeleteFramebuffers(1, &shadowFBO);
    glDeleteRenderbuffers(1, &shadowDepthRBO);
    glDeleteTextures(1, &shadowCubeMap);
    glDeleteTextures(1, &defaultWhiteTexture);
    glDeleteTextures(1, &defaultNormalTexture);

    Logger::info("RENDERER", "Rasterizer shutdown complete.");
}

} // namespace Engine
