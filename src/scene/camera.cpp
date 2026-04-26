#include "Camera.h"

// Constructor
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(SENSITIVITY),
      MovementSpeed(SPEED), Zoom(ZOOM) {

    // Set values
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    // Intialize the camera vectors
    updateCameraVectors();
}

// Calc (short for calculate) the view matrix
glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

// Calc (short for calculate) the projection matrix
glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 100.0f);
}

// Camera movement
// TODO: Change name for simply moving the camera
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {

    // Calculate the velocity of the camera
    float velocity = MovementSpeed * deltaTime;

    // Parse movement
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

// Mouse movement
// TODO: Change name to abstract the looking direction
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  bool constrainPitch) {

    // Change the x & y offset
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    // Modify pitch and yaw
    Yaw += xoffset;
    Pitch += yoffset;

    // Make pitch not go above 90deg
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update the camera vectors with new pitch & yaw
    updateCameraVectors();
}

// Update the front, right, and up vectors
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;

    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
