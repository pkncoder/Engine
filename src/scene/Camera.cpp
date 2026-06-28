#include "Camera.h"

#include "../services/Logger.h"
#include "../services/Timer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

// Constructor
Camera::Camera(glm::vec3 _position, glm::vec3 _up, glm::vec3 _right, float _yaw,
               float _pitch, float _fov, float _sensitivity,
               float _movementSpeed)
    : position(_position), up(_up), right(_right), yaw(_yaw), pitch(_pitch),
      fov(_fov), sensitivity(_sensitivity), movementSpeed(_movementSpeed) {

    this->worldUp = up;

    // Intialize the camera vectors
    updateCameraVectors();

    Logger::info("SCENE", "Camera initialized.");
}

// Calc (short for calculate) the view matrix
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

// Calc (short for calculate) the projection matrix
glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
}

// Camera movement
void Camera::processMovement(Camera_Movement direction,
                             float movementModifier) {

    // Get deltaTime from the timer
    float deltaTime = Engine::Timer::getDeltaTime();

    // Calculate the velocity of the camera
    float velocity = movementSpeed * deltaTime * movementModifier;

    // Parse movement
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    if (direction == UP)
        position += up * velocity;
    if (direction == DOWN)
        position -= up * velocity;
}

// Pitch and yaw modifications
void Camera::processLookingDirectionMovement(float xoffset, float yoffset,
                                             bool constrainPitch) {

    // Change the x & y offset
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Modify pitch and yaw
    yaw += xoffset;
    pitch += yoffset;

    // Make pitch not go above 90deg
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update the camera vectors with new pitch & yaw
    updateCameraVectors();
}

// Update the front, right, and up vectors
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;

    // Do the math for the new front direction w/ yaw&pitch
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Update the front, right, and up vectors
    this->front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

} // namespace Engine
