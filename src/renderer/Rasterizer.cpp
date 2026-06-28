#include "Rasterizer.h"

#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho / lookAt

namespace Engine {

void Rasterizer::init() {

    shader = Shader("shaders/rasterizing/main/raster.vert",
                    "shaders/rasterizing/main/raster.frag");

    // NEW: Shadow depth shader
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
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Depth renderbuffer — needed so depth testing works during each face
    // render
    glGenRenderbuffers(1, &shadowDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SHADOW_WIDTH,
                          SHADOW_HEIGHT);

    // FBO — color attachment is swapped per-face at render time
    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, shadowDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::error("RENDERER", "Shadow cube map FBO incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Logger::info("RENDERER", "Rasterizer initialized.");
}

void Rasterizer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {

    // Pull renderables once — used in both passes
    const auto renderables =
        activeScene.getMatchingEntities<TransformComponent, MeshComponent,
                                        MaterialComponent>();

    // Helper: build model matrix from a transform (avoids duplicating this in
    // both passes)
    auto buildModel = [](const TransformComponent &t) {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, t.position);
        m *= glm::mat4_cast(t.rotation);
        m = glm::scale(m, t.scale);
        return m;
    };

    // ----------------------------------------------------------------
    // Find the primary light: the most emissive object in the scene.
    // Falls back to the camera position if nothing emits.
    // ----------------------------------------------------------------
    glm::vec3 lightPos = camera.position;
    float bestEmissive = 0.0f;
    for (EntityID id : renderables) {
        const auto &mat = activeScene.getComponent<MaterialComponent>(id);
        float strength = glm::length(mat.emmissive);
        if (strength > bestEmissive) {
            bestEmissive = strength;
            lightPos =
                activeScene.getComponent<TransformComponent>(id).position;
        }
    }

    const glm::vec3 faceDirections[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                         {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
    const glm::vec3 faceUps[6] = {{0, -1, 0}, {0, -1, 0}, {0, 0, 1},
                                  {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};

    // 90 degree FOV, square aspect — exactly covers one cube face
    glm::mat4 shadowProj =
        glm::perspective(glm::radians(90.0f), 1.0f, SHADOW_NEAR, SHADOW_FAR);
    // ================================================================
    // PASS 1 — Shadow Map
    // Render depth from the light's point of view
    // ================================================================

    GLint savedViewport[4];
    glGetIntegerv(GL_VIEWPORT, savedViewport);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    shadowShader.bind();
    shadowShader.setVec3("uLightPos", lightPos);
    shadowShader.setFloat("uFarPlane", SHADOW_FAR);

    for (int face = 0; face < 6; face++) {
        // Point the FBO color output at this cube face
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                               shadowCubeMap, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView = glm::lookAt(
            lightPos, lightPos + faceDirections[face], faceUps[face]);
        shadowShader.setMat4("uLightSpaceMatrix", shadowProj * lightView);

        for (EntityID id : renderables) {
            const auto &mesh = activeScene.getComponent<MeshComponent>(id);
            const auto &transform =
                activeScene.getComponent<TransformComponent>(id);
            shadowShader.setMat4("uModel", buildModel(transform));
            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(savedViewport[0], savedViewport[1], savedViewport[2],
               savedViewport[3]);
    // ================================================================
    // PASS 2 — Main Raster Pass
    // ================================================================

    shader.bind();

    const glm::mat4 view = camera.getViewMatrix();
    const glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);

    shader.setMat4("uViewProjection", proj * view);
    shader.setVec3("uCameraPos", camera.position);
    shader.setFloat("uFOV", camera.fov);
    shader.setVec2("uResolution", glm::vec2(currentWidth, currentHeight));

    // NEW: pass shadow data to the main shader
    shader.setVec3("uLightPos", lightPos);
    shader.setFloat("uShadowFarPlane",
                    SHADOW_FAR); // new — replaces uLightSpaceMatrix

    for (EntityID id : renderables) {
        const auto &mesh = activeScene.getComponent<MeshComponent>(id);
        const auto &transform =
            activeScene.getComponent<TransformComponent>(id);
        const auto &material = activeScene.getComponent<MaterialComponent>(id);

        shader.setMat4("uModel", buildModel(transform));
        shader.setInt("uIsBumpMap", material.isBumpMap);

        shader.setVec3("uAlbedo", material.albedo);
        shader.setVec3("uEmmissive", material.emmissive);
        shader.setFloat("uRoughness", material.roughness);
        shader.setFloat("uMetallic", material.metallic);

        int currentTextureUnit = 0;
        auto bindMap = [&](GLuint texID, const std::string &uniformName,
                           GLuint fallbackID) {
            glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
            glBindTexture(GL_TEXTURE_2D, texID != 0 ? texID : fallbackID);
            shader.setInt(uniformName, currentTextureUnit);
            currentTextureUnit++;
        };

        bindMap(material.albedoTexture, "uAlbedoMap", defaultWhiteTexture);
        bindMap(material.emissiveTexture, "uEmissiveMap", defaultWhiteTexture);
        bindMap(material.roughnessTexture, "uRoughnessMap",
                defaultWhiteTexture);
        bindMap(material.metallicTexture, "uMetallicMap", defaultWhiteTexture);
        bindMap(material.normalTexture, "uNormalMap", defaultNormalTexture);

        // NEW: Shadow map goes into the next free slot (unit 5)
        glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP,
                      shadowCubeMap); // cube map, not TEXTURE_2D
        shader.setInt("uShadowCubeMap", currentTextureUnit);
        currentTextureUnit++;

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

void Rasterizer::resize(const int newWidth, const int newHeight) {
    // If the new size is the same as the old one, do nothing
    if (newWidth == currentWidth && newHeight == currentHeight)
        return;

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
}

} // namespace Engine
