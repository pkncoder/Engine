#include "GPUStructs.h"
#include "Rasterizer.h"

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

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
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
    addShaderPass("ShadowPass", "shaders/shadow.vert", "shaders/shadow.frag");
    addShaderPass("RasterPass", "shaders/raster.vert", "shaders/raster.frag");

    // Allocate persistent UBO
    cameraUBO =
        BufferManager::createBuffer("CameraUBO", BufferType::UniformBuffer,
                                    BufferUsage::Dynamic, sizeof(CameraData));

    // NOTE: In modern OpenGL, binding UBOs via layout(std140, binding = 0) in
    // the shader is preferred over manual block lookup, but your BufferManager
    // map layout goes here.
    BufferManager::mapUBOLayout(cameraUBO, 0, "CameraUBO",
                                {"uView", "uProjection"});

    Logger::info("RENDERER", "Rasterizer initialized.");
}

void Rasterizer::prepare(EngineState &state) {
    // input: list of render packets (mesh, material, modelMatrix)
    // output: list of draw commands within each shader pass

    BufferManager::setUBOValue(cameraUBO, "uView", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.view));
    BufferManager::setUBOValue(cameraUBO, "uProjection", sizeof(glm::mat4),
                               glm::value_ptr(cameraData.projection));
    BufferManager::pushBuffer(cameraUBO, frameIndex);

    // TODO: generate shadowFBO?

    // Create the render passes
    ShaderPass shadowPass = shaderPasses[0];
    ShaderPass mainPass = shaderPasses[0];

    // Loop through the render packets
    for (const auto &packet : renderPackets) {

        GPUMesh *gpuMesh =
            GPUResourceManager::getOrUploadMesh(packet.meshHandle);
        CPUMaterialData *cpuMaterial = engineContext.getAsset()
                                           ->getMaterial(packet.materialHandle)
                                           .get(); // TEMP

        // Map material textures to GLuints safely
        auto getTexID = [&](const std::string &key) -> GLuint {
            auto ittr = cpuMaterial->textureMaps.find(key);
            if (ittr != cpuMaterial->textureMaps.end()) {
                GPUTexture *tex =
                    GPUResourceManager::getOrUploadTexture(ittr->second);
                return tex ? tex->textureID : 0;
            }
            return 0;
        };

        /* ----------------- Shadow Pass ----------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand;

            shadowPass.fbo = shadowFBO;

            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;

            // TODO: textures? (alpha map)
            drawCommand->textures[2] = getTexID("alpha");

            shadowPass.commands.push_back(drawCommand);
        }

        /* ----------------- Main Pass ---------------- */
        {
            std::shared_ptr<RasterDrawCommand> drawCommand;

            mainPass.fbo = 0; // Main screen

            drawCommand->vao = gpuMesh->vao;
            drawCommand->indexCount = gpuMesh->indexCount;
            drawCommand->modelMatrix = packet.modelMatrix;

            // TODO: textures?
            drawCommand->textures[0] = getTexID("albedo");
            drawCommand->textures[1] = getTexID("emmissive");
            drawCommand->textures[2] = getTexID("alpha");
            drawCommand->textures[3] = getTexID("roughness");
            drawCommand->textures[4] = getTexID("metallic");
            drawCommand->textures[5] = getTexID("normal");
            drawCommand->textures[6] = getTexID("bump");

            // TODO: Uniform
            if (drawCommand->textures[6] == 1 &&
                drawCommand->textures[5] == 0) {
                // shader.setInt("uIsBumpMap", 1);

            } else {
                // shader.setInt("uIsBumpMap", 0);
            }

            mainPass.commands.push_back(drawCommand);
        }
    }
}

void Rasterizer::dispatch(EngineState &state) {
    for (const ShaderPass &pass : shaderPasses) {

        glBindFramebuffer(GL_FRAMEBUFFER, pass.fbo);
        glViewport(0, 0, currentWidth, currentHeight); // TODO: per-pass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(pass.shader.ID);

        // GLuint currentShader = 0;
        GLuint currentVAO = 0;

        for (const std::shared_ptr<DrawCommand> cmd : pass.commands) {

            const auto command =
                std::static_pointer_cast<RasterDrawCommand>(cmd);

            // Upload Model Matrix (Ideally this is in a UBO/TBO too, not a
            // uniform)
            // glUniformMatrix4fv(0, 1, GL_FALSE,
            //                    glm::value_ptr(command->modelMatrix));

            pass.shader.setMat4("uModel", command->modelMatrix);

            if (currentVAO != command->vao) {
                glBindVertexArray(command->vao);
                currentVAO = command->vao;
            }

            glDrawElements(GL_TRIANGLES, command->indexCount, GL_UNSIGNED_INT,
                           0);
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
