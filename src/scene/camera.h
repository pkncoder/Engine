#pragma once

#include "../services/Timer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
  public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler Angles
    float yaw;
    float pitch;

    // Camera options
    float mouseSensitivity;
    float movementSpeed;
    float zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
           float pitch = PITCH);

    // Calculates and returns the view matrix
    glm::mat4 getViewMatrix() const;

    // Calculates and returns the projection matrix based on aspect ratio
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    // Processes input received from any keyboard-like input system
    void processMovement(Camera_Movement direction);

    // Processes input received from a mouse input system
    // TODO: Make better name
    void processAngleMovement(float xoffset, float yoffset,
                              bool constrainPitch = true);

  private:
    // Update the front, right, and up camera vectors
    void updateCameraVectors();
};
