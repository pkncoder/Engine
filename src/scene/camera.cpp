#include "Camera.h"

// Constructor
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), mouseSensitivity(SENSITIVITY),
      movementSpeed(SPEED), zoom(ZOOM) {

    // Set values
    this->position = position;
    worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;

    // Intialize the camera vectors
    updateCameraVectors();
}

// Calc (short for calculate) the view matrix
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

// Calc (short for calculate) the projection matrix
glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(zoom), aspectRatio, 0.1f, 100.0f);
}

// Camera movement
void Camera::processMovement(Camera_Movement direction, float deltaTime) {

    // Calculate the velocity of the camera
    float velocity = movementSpeed * deltaTime;

    // Parse movement
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
}

// Pitch and yaw modifications
void Camera::processAngleMovement(float xoffset, float yoffset,
                                  bool constrainPitch) {

    // Change the x & y offset
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

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

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    this->front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
