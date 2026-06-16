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
    shader.setMat4("u_viewProjection", proj * view);
    shader.setVec3("u_viewPos", camera.position);

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
        shader.setMat4("u_model", model);

        // Set the material uniforms
        shader.setVec3("u_albedo", material.albedo);
        shader.setVec3("u_emmissive", material.emmissive);
        shader.setFloat("u_roughness", material.roughness);
        shader.setFloat("u_metallic", material.metallic);

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
