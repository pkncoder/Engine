#include "Rasterizer.h"

#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "GPUResourceManager.h"
#include "IRenderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho / lookAt

#include <memory>
#include <string>

namespace Engine {

void Rasterizer::init(EngineState &state) {

    shader = Shader("shaders/rasterizing/main/raster.vert",
                    "shaders/rasterizing/main/raster.frag");

    shadowShader = Shader("shaders/rasterizing/main/shadow.vert",
                          "shaders/rasterizing/main/shadow.frag");

    /* --- TODO: TEMP --- */

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

    // --- NEW: Shadow Map FBO + Depth Texture ---
    // Create the R32F cube map — one face per direction, stores linear distance
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

    // Generate the shadow render buffer & attach the width & height
    glGenRenderbuffers(1, &shadowDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          state.renderer.settings.shadowWidth,
                          state.renderer.settings.shadowHeight);

    // Generate the shadow fbo + attach the render buffer
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, shadowDepthRBO);

    // Check to make sure it generated
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::error("RENDERER", "Shadow cube map FBO incomplete!");

    // Un-bind the the fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Logger::info("RENDERER", "Rasterizer initialized.");
}

void Rasterizer::beginFrame(EngineState &state) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_RenderPackets.clear();
}

void Rasterizer::extract(EngineState &state) {

    SceneManager *sceneManager = engineContext.getScene();
    AssetManager *assetManager = engineContext.getAsset();
    Scene &scene = sceneManager->getScene();

    CameraState &cameraState = state.scene.camera;

    // Pull renderables once — used in both passes
    const auto renderables =
        scene.getMatchingEntities<TransformComponent, MeshComponent,
                                  MaterialComponent>();

    // Model matrix helper lamba function
    auto buildModel = [](const TransformComponent &t) {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, t.position);
        m *= glm::mat4_cast(t.rotation);
        m = glm::scale(m, t.scale);
        return m;
    };

    // Lambda for binding texture maps
    int currentTextureUnit = 0;
    auto bindMap = [&](GLuint texID, const std::string &uniformName,
                       GLuint fallbackID) {
        glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
        glBindTexture(GL_TEXTURE_2D, texID != 0 ? texID : fallbackID);
        shader.setInt(uniformName, currentTextureUnit);
        currentTextureUnit++;
    };

    m_ActiveLightPos = cameraState.position;
    float bestEmissive = 0.0f;

    // Loop ECS ONCE and pack the data
    for (EntityID id : renderables) {
        const auto &transform = scene.getComponent<TransformComponent>(id);
        const auto &mesh = scene.getComponent<MeshComponent>(id);
        const auto &material = scene.getComponent<MaterialComponent>(id);

        // Find the brightest light (temporary logic)
        auto cpuMat = assetManager->getMaterial(material.handle);
        if (cpuMat) {
            float strength = glm::length(cpuMat->emissive);
            if (strength > bestEmissive) {
                bestEmissive = strength;
                m_ActiveLightPos = transform.position;
            }
        }

        // Push to the render packet list
        m_RenderPackets.push_back(
            {buildModel(transform), mesh.handle, material.handle});
    }
}

void Rasterizer::prepare(EngineState &state) {
    SceneManager *sceneManager = engineContext.getScene();
    Camera &camera = sceneManager->getCamera();

    // Pre-calculate all global matrices for the dispatch phase
    if (currentHeight > 0) {
        m_ViewMatrix = camera.getViewMatrix();
        m_ProjMatrix =
            camera.getProjectionMatrix(currentWidth / (float)currentHeight);
    }

    m_ShadowProjMatrix = glm::perspective(glm::radians(90.0f), 1.0f,
                                          state.renderer.settings.shadowNear,
                                          state.renderer.settings.shadowFar);
}

void Rasterizer::dispatch(EngineState &state) {

    AssetManager *assetManager = engineContext.getAsset();
    // GPUResourceManager* gpuManager = engineContext.getGPUResourceManager();
    GPUResourceManager::init(assetManager);

    int currentTextureUnit = 0;
    auto bindMap = [&](GPUTexture *gpuTex, const std::string &uniformName,
                       GLuint fallbackID, Shader &activeShader) {
        glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
        glBindTexture(GL_TEXTURE_2D, gpuTex ? gpuTex->textureID : fallbackID);
        activeShader.setInt(uniformName, currentTextureUnit);
        currentTextureUnit++;
    };

    // Offsets for the shadow map
    const glm::vec3 faceDirections[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                         {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
    const glm::vec3 faceUps[6] = {{0, -1, 0}, {0, -1, 0}, {0, 0, 1},
                                  {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};

    // Get the render dimentions & pre-shadow mapping
    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, savedViewport);

    // Change the size of theviewport & bind the shadow FBO
    glViewport(0, 0, state.renderer.settings.shadowHeight,
               state.renderer.settings.shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    // Bind the shadow map shader & upload uniforms
    shadowShader.bind();
    shadowShader.setVec3("uLightPos", m_ActiveLightPos);
    shadowShader.setFloat("uFarPlane", state.renderer.settings.shadowFar);

    // Loop each face for the mapping
    for (int face = 0; face < 6; face++) {
        // Point the FBO color output at this cube face and attach settings
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                               shadowCubeMap, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the light view mat, and upload the new viewProjection matrix
        glm::mat4 lightView =
            glm::lookAt(m_ActiveLightPos,
                        m_ActiveLightPos + faceDirections[face], faceUps[face]);
        shadowShader.setMat4("uLightSpaceMatrix",
                             m_ShadowProjMatrix * lightView);

        // Loop each renderable & and shadow map it
        for (const RenderPacket packet : m_RenderPackets) {

            GPUMesh *gpuMesh =
                GPUResourceManager::getOrUploadMesh(packet.meshHandle);
            if (!gpuMesh)
                continue;

            shadowShader.setMat4("uModel", packet.modelMatrix);

            currentTextureUnit = 0;
            auto cpuMat = assetManager->getMaterial(packet.materialHandle);

            if (cpuMat) {

                GPUTexture *alphaTex = nullptr;
                auto it = cpuMat->textureMaps.find("alpha");

                if (it != cpuMat->textureMaps.end()) {
                    // Found it! Use the handle to get the GPU texture
                    alphaTex =
                        GPUResourceManager::getOrUploadTexture(it->second);
                }

                bindMap(alphaTex, "uAlphaMap", defaultWhiteTexture,
                        shadowShader);
            }

            glBindVertexArray(gpuMesh->vao);
            glDrawElements(GL_TRIANGLES, gpuMesh->indexCount, GL_UNSIGNED_INT,
                           0);
        }
    }

    // Un-bind the framebuffer and re-enstate the new viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2],
               savedViewport[3]);

    /* ----------------- Pass 2 ----------------- */

    // Bind the main shader
    shader.bind();

    // Upload the uniforms for base rendering
    shader.setMat4("uViewProjection", m_ProjMatrix * m_ViewMatrix);
    shader.setVec3("uCameraPos", state.scene.camera.position);
    shader.setFloat("uFOV", state.scene.camera.fov);
    shader.setVec2("uResolution", glm::vec2(currentWidth, currentHeight));

    // Upload shadow mapping uniforms
    shader.setVec3("uLightPos", m_ActiveLightPos);
    shader.setFloat(
        "uShadowFarPlane",
        state.renderer.settings.shadowFar); // new — replaces uLightSpaceMatrix

    // Loop each renderablable
    for (const RenderPacket packet : m_RenderPackets) {

        GPUMesh *gpuMesh =
            GPUResourceManager::getOrUploadMesh(packet.meshHandle);
        if (!gpuMesh) {
            Logger::debug("NOMESH");
            continue;
        }

        shader.setMat4("uModel", packet.modelMatrix);

        // 2. FETCH MATERIAL DATA
        std::shared_ptr<CPUMaterialData> cpuMat =
            assetManager->getMaterial(packet.materialHandle);
        if (!cpuMat) {
            Logger::fatal("RENDER", "CPU material error, mat handle: " +
                                        std::to_string(packet.materialHandle));
        }

        shader.setVec3("uAlbedo", cpuMat->albedo);
        shader.setVec3("uEmmissive", cpuMat->emissive);
        shader.setFloat("uRoughness", cpuMat->roughness);
        shader.setFloat("uMetallic", cpuMat->metallic);

        // Texture map binding
        currentTextureUnit = 0;

        auto getSafeTex = [&](const std::string &key) -> GPUTexture * {
            auto it = cpuMat->textureMaps.find(key);
            if (it != cpuMat->textureMaps.end()) {
                return GPUResourceManager::getOrUploadTexture(it->second);
            }
            return nullptr; // Texture doesn't exist for this material
        };

        GPUTexture *albedoTex = getSafeTex("albedo");
        GPUTexture *emissiveTex = getSafeTex("emissive");
        GPUTexture *roughnessTex = getSafeTex("roughness");
        GPUTexture *metallicTex = getSafeTex("metallic");
        GPUTexture *alphaTex = getSafeTex("alpha");
        GPUTexture *normalTex = getSafeTex("normal");
        GPUTexture *bumpTex = getSafeTex("bump");

        bindMap(albedoTex, "uAlbedoMap", defaultWhiteTexture, shader);
        bindMap(emissiveTex, "uEmissiveMap", defaultWhiteTexture, shader);
        bindMap(roughnessTex, "uRoughnessMap", defaultWhiteTexture, shader);
        bindMap(metallicTex, "uMetallicMap", defaultWhiteTexture, shader);
        bindMap(alphaTex, "uAlphaMap", defaultWhiteTexture, shader);
        bindMap(normalTex, "uNormalMap", defaultNormalTexture, shader);
        bindMap(bumpTex, "uBumpMap", defaultWhiteTexture, shader);

        if (bumpTex && !normalTex) {
            Logger::debug("BUMP: " + cpuMat->name);
            shader.setInt("uIsBumpMap", 1);

        } else {
            shader.setInt("uIsBumpMap", 0);
        }

        glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
        shader.setInt("uShadowCubeMap", currentTextureUnit);
        currentTextureUnit++;

        // Bind the vao and draw the triangles
        glBindVertexArray(gpuMesh->vao);
        glDrawElements(GL_TRIANGLES, gpuMesh->indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

void Rasterizer::postProcess(EngineState &state) {}

void Rasterizer::present(EngineState &state) {}

void Rasterizer::resize(const uint32_t newWidth, const uint32_t newHeight) {

    // If the new size is the same as the old one, do nothing
    if (newWidth == currentWidth && newHeight == currentHeight) {
        return;
    }

    // Update the tracked size
    currentWidth = newWidth;
    currentHeight = newHeight;
}

void Rasterizer::shutdown() {
    glDeleteFramebuffers(1, &shadowFBO);
    glDeleteRenderbuffers(1, &shadowDepthRBO);
    glDeleteTextures(1, &shadowCubeMap);
    glDeleteTextures(1, &defaultWhiteTexture);
    glDeleteTextures(1, &defaultNormalTexture);

    Logger::info("RENDERER", "Rasterizer shutdown complete.");
}

} // namespace Engine
