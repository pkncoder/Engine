#pragma once

#include "../core/states/CameraState.h"

#include <glm/glm.hpp>

namespace Engine {

// Defines several possible options for camera movement.
// Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values

class Camera {
  public:
    // Constructors
    Camera() = default;
    Camera(CameraState &state);

    // Calculates and returns the view matrix
    glm::mat4 getViewMatrix() const;

    // Calculates and returns the projection matrix based on aspect ratio
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    // Processes input received from any keyboard-like input system
    void processMovement(CameraState &state, Camera_Movement direction,
                         float movementModifier = 1.0);

    // Processes the change to where the camera is looking
    void processLookingDirectionMovement(CameraState &state, float xoffset,
                                         float yoffset,
                                         bool constrainPitch = true);

  public:
    // Camera position
    glm::vec3 position;

    // World positioning
    glm::vec3 worldUp;

    // Camera vectors
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

    // Camera Angles
    float yaw;
    float pitch;
    float fov;

    // Camera options
    float sensitivity;
    float movementSpeed;

    // Camera states
    float cameraDirty = false;

  private:
    // Update the front, right, and up camera vectors
    void updateCameraVectors();
};

} // namespace Engine
