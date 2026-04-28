#include "Rasterizer.h"

#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"

namespace Engine {

// Init the rasterizer
void Rasterizer::init() {

    // Compile the shader code
    shader =
        Shader("shaders/raster/viewport.vert", "shaders/raster/viewport.frag");
}

// Render a frame
void Rasterizer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {
    shader.bind(); // glUseProgram under the hood

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);
    shader.setMat4("u_ViewProjection", proj * view);

    // 1. Ask ECS for all entities that have both a Transform and a Mesh
    auto renderables = activeScene.getView<Transform, MeshComponent>();

    // 2. Loop through and draw them
    for (EntityID id : renderables) {
        // Grab the data components
        auto &transform = activeScene.getComponent<Transform>(id);
        auto &mesh = activeScene.getComponent<MeshComponent>(id);

        // 3. Calculate Model Matrix (Rasterizer handles the math)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.Position);
        model *= glm::mat4_cast(transform.Rotation);
        model = glm::scale(model, transform.Scale);

        // 4. Set Uniforms specific to this entity
        shader.setMat4("u_Model", model);

        // 5. OpenGL Execution
        glBindVertexArray(mesh.VAO);

        // We use glDrawElements because we are using Indices (EBO)
        glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, 0);
    }

    // Unbind when done
    glBindVertexArray(0);
}

void Rasterizer::shutdown() {};

} // namespace Engine

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
