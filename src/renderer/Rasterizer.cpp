#pragma once

#include "Rasterizer.h"
#include "BufferManager.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

void Rasterizer::Render(const Camera &camera, float aspectRatio) {
    m_Shader.Bind();

    // The math happens here, hidden away from the main loop
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix(aspectRatio);

    m_Shader.SetMat4("u_ViewProjection", proj * view);

    // Hardcoded draw for Phase II test
    glBindVertexArray(m_TestVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Rasterizer::Init() {

    m_Shader =
        Shader("shaders/raster/viewport.vert", "shaders/raster/viewport.frag");

    std::vector<Vertex> triangle = {{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                    {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    m_TestVAO = BufferManager::CreateSimpleMesh(triangle);
}

void Rasterizer::Shutdown() {};
