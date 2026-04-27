#include "Rasterizer.h"

#include "BufferManager.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace Engine {

// Init the rasterizer
void Rasterizer::init() {

    // Compile the shader code
    shader =
        Shader("shaders/raster/viewport.vert", "shaders/raster/viewport.frag");

    // TODO: temp
    std::vector<TempVertex> triangle = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    // TODO: temp
    testVAO = BufferManager::createSimpleMesh(triangle);
}

// Render a frame
void Rasterizer::render(const Camera &camera, float aspectRatio) {
    shader.bind(); // Bind the shader code

    // Get the view & projection matricies from the camera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);

    // Set the view projection matrix as a uniform
    shader.setMat4("u_ViewProjection", proj * view);

    // Hardcoded draw for Phase II test
    // TODO: temp
    glBindVertexArray(testVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Rasterizer::shutdown() {};

} // namespace Engine
