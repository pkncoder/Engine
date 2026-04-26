#pragma once

#include "Rasterizer.h"
#include "BufferManager.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

// Init the rasterizer
void Rasterizer::Init() {

    // Compile the shader code
    m_Shader =
        Shader("shaders/raster/viewport.vert", "shaders/raster/viewport.frag");

    // TODO: temp
    std::vector<Vertex> triangle = {{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                    {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    // TODO: temp
    m_TestVAO = BufferManager::CreateSimpleMesh(triangle);
}

// Render a frame
void Rasterizer::Render(const Camera &camera, float aspectRatio) {
    m_Shader.Bind(); // Bind the shader code

    // Get the view & projection matricies from the camera
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix(aspectRatio);

    // Set the view projection matrix as a uniform
    m_Shader.SetMat4("u_ViewProjection", proj * view);

    // Hardcoded draw for Phase II test
    // TODO: temp
    glBindVertexArray(m_TestVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

// TODO: Fillout
void Rasterizer::Shutdown() {};
