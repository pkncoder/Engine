#include "CameraSystem.h"

#include "../../scene/components/CameraComponent.h"
#include "../../scene/components/TransformComponent.h"
#include "../../services/Input.h"
#include "../../services/Timer.h"

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>

namespace Engine {

void CameraSystem::update(entt::registry &registry) {
    float dt = Timer::getDeltaTime();
    glm::vec2 mouseDelta = Input::getMouseDelta();

    auto view = registry.view<TransformComponent, CameraComponent>();

    for (auto entity : view) {
        auto &transform = view.get<TransformComponent>(entity);
        auto &camera = view.get<CameraComponent>(entity);

        if (Input::isButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) { // Right click
            camera.yaw += mouseDelta.x * camera.sensitivity;
            camera.pitch += -mouseDelta.y * camera.sensitivity;
        }

        if (camera.constrainPitch) {
            if (camera.pitch > 89.0f)
                camera.pitch = 89.0f;
            if (camera.pitch < -89.0f)
                camera.pitch = -89.0f;
        }
        float velocity = camera.movementSpeed * dt;

        if (Input::isKeyHeld(GLFW_KEY_W))
            transform.position += camera.front * velocity;
        if (Input::isKeyHeld(GLFW_KEY_S))
            transform.position -= camera.front * velocity;
        if (Input::isKeyHeld(GLFW_KEY_A))
            transform.position -= camera.right * velocity;
        if (Input::isKeyHeld(GLFW_KEY_D))
            transform.position += camera.right * velocity;
        if (Input::isKeyHeld(GLFW_KEY_SPACE))
            transform.position += camera.worldUp * velocity;
        if (Input::isKeyHeld(GLFW_KEY_LEFT_SHIFT))
            transform.position -= camera.worldUp * velocity;

        // 1. Calculate new front vector based on yaw and pitch
        glm::vec3 front;
        front.x =
            cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        front.y = sin(glm::radians(camera.pitch));
        front.z =
            sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

        // 2. Update cached vectors
        camera.front = glm::normalize(front);
        camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
        camera.up = glm::normalize(glm::cross(camera.right, camera.front));
    }
}

} // namespace Engine
