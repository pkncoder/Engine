#include "CameraSystem.h"

#include "../../scene/components/CameraComponent.h"
#include "../../scene/components/TransformComponent.h"

namespace Engine {

void CameraSystem::update(entt::registry &registry) {
    auto view = registry.view<TransformComponent, CameraComponent>();

    for (auto entity : view) {
        auto &transform = view.get<TransformComponent>(entity);
        auto &camera = view.get<CameraComponent>(entity);

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
