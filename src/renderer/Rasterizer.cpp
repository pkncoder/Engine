#include "Rasterizer.h"

#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Engine {

// Init the rasterizer
void Rasterizer::init() {

    // Compile the shader code
    shader = Shader("shaders/rasterizing/main/raster.vert",
                    "shaders/rasterizing/main/raster.frag");

    /* --- TODO: TEMP --- */

    // Create a 1x1 White Texture (For Albedo, Emissive)
    glGenTextures(1, &defaultWhiteTexture);
    glBindTexture(GL_TEXTURE_2D, defaultWhiteTexture);
    unsigned char whitePixel[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 whitePixel);

    // Create a 1x1 black texture (for roughness / metallic)
    glGenTextures(1, &defaultBlackTexture);
    glBindTexture(GL_TEXTURE_2D, defaultBlackTexture);
    unsigned char blackPixel[] = {0, 0, 0, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 blackPixel);

    // Create a 1x1 Flat Normal Texture (For Normal Maps)
    // Flat normal is pointing straight up: RGB(128, 128, 255)
    glGenTextures(1, &defaultNormalTexture);
    glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
    unsigned char flatNormalPixel[] = {128, 128, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 flatNormalPixel);

    Logger::info("RENDERER", "Rasterizer initialized.");
}

// Render a frame
void Rasterizer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {
    shader.bind(); // glUseProgram under the hood

    const glm::mat4 view = camera.getViewMatrix();
    const glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);

    // Upload the camera uniforms
    shader.setMat4("uViewProjection", proj * view);
    shader.setVec3("uCameraPos", camera.position);
    shader.setFloat("uFOV", camera.fov);

    // Get all of the renderables from the scene
    const auto renderables =
        activeScene.getMatchingEntities<TransformComponent, MeshComponent,
                                        MaterialComponent>();

    // Loop every one to draw
    for (EntityID id : renderables) {
        // Get the components from the entity that are used in rendering
        const auto &mesh = activeScene.getComponent<MeshComponent>(id);

        const auto &transform =
            activeScene.getComponent<TransformComponent>(id);
        const auto &material = activeScene.getComponent<MaterialComponent>(id);

        // Calculate Model Matrix (Rasterizer handles the math)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        // Pass the model matrix to the shader
        shader.setMat4("uModel", model);
        shader.setInt("uIsBumpMap", material.isBumpMap);

        // Set the material uniforms
        shader.setVec3("uAlbedo", material.albedo);
        shader.setVec3("uEmmissive", material.emmissive);
        shader.setFloat("uRoughness", material.roughness);
        shader.setFloat("uMetallic", material.metallic);

        // Keep track of which texture slot we are currently filling
        int currentTextureUnit = 0;

        auto bindMap = [&](GLuint texID, const std::string &uniformName,
                           GLuint fallbackID) {
            glActiveTexture(GL_TEXTURE0 + currentTextureUnit);

            // If the texture exists, bind it. Otherwise, bind the fallback
            // dummy texture.
            glBindTexture(GL_TEXTURE_2D, texID != 0 ? texID : fallbackID);

            // Tell the shader which slot to look in (ONLY ONE UNIFORM!)
            shader.setInt(uniformName, currentTextureUnit);

            // Increment the slot for the next texture map
            currentTextureUnit++;
        };

        bindMap(material.albedoTexture, "uAlbedoMap", defaultWhiteTexture);
        bindMap(material.emissiveTexture, "uEmissiveMap", defaultBlackTexture);
        bindMap(material.roughnessTexture, "uRoughnessMap",
                defaultWhiteTexture);
        bindMap(material.metallicTexture, "uMetallicMap", defaultWhiteTexture);

        bindMap(material.normalTexture, "uNormalMap", defaultNormalTexture);

        // Give the vertex array
        glBindVertexArray(mesh.vao);

        // Draw this with GL_TRIANGLES + the index count
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }

    // Unbind when done
    glBindVertexArray(0);
}

void Rasterizer::shutdown(){};

} // namespace Engine
