#pragma once

#include <glm/ext/vector_float3.hpp>
namespace Engine {

struct CameraComponent {
    float fov = 60.0f;
    float nearPlane = 0.0001f;
    float farPlane = 1000.0f;

    float yaw = 0.0f;
    float pitch = 0.0f;

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
};

}
