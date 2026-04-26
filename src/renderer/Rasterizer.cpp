#pragma once

#include "Rasterizer.h"
#include "BufferManager.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

void Rasterizer::Render(glm::mat4 view, glm::mat4 proj) {
    m_Shader.Bind();

    // TODO: better asspect ratio fix
    // Calculate View-Projection matrix
    glm::mat4 vp = proj * view;
    m_Shader.SetMat4("u_ViewProjection", vp);

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
