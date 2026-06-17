#include "Rasterizer.h"

#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Engine {

// Init the rasterizer
void Rasterizer::init() {

    // Compile the shader code
    shader = Shader("shaders/rasterizing/main/raster.vert",
                    "shaders/rasterizing/main/raster.frag");

    Logger::info("RENDERER", "Rasterizer initialized.");
}

// Render a frame
void Rasterizer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {
    shader.bind(); // glUseProgram under the hood

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);

    // Upload the camera uniforms
    shader.setMat4("uViewProjection", proj * view);
    shader.setVec3("uViewPos", camera.position);

    // Get all of the renderables from the scene
    auto renderables =
        activeScene.getMatchingEntities<TransformComponent, MeshComponent,
                                        MaterialComponent>();

    // Loop every one to draw
    for (EntityID id : renderables) {
        // Get the components from the entity that are used in rendering
        auto &mesh = activeScene.getComponent<MeshComponent>(id);
        auto &transform = activeScene.getComponent<TransformComponent>(id);
        auto &material = activeScene.getComponent<MaterialComponent>(id);

        // Calculate Model Matrix (Rasterizer handles the math)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        // Pass the model matrix to the shader
        shader.setMat4("uModel", model);

        // Set the material uniforms
        shader.setVec3("uAlbedo", material.albedo);
        shader.setVec3("uEmmissive", material.emmissive);
        shader.setFloat("uRoughness", material.roughness);
        shader.setFloat("uMetallic", material.metallic);

        if (material.albedoTexture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.albedoTexture);
            shader.setInt("uAlbedoMap",
                          0); // Tell shader the texture is in unit 0
            shader.setInt("uHasAlbedoMap", 1); // Boolean flag for the shader
        } else {
            shader.setInt("uHasAlbedoMap", 0);
        }

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
