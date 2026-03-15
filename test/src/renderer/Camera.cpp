#include "Camera.h"

namespace renderer {

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : position(position), yaw(yaw), pitch(pitch),
      front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(5.0f),
      mouseSensitivity(0.1f), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
  updateCameraVectors();
}

void Camera::processKeyboard(int direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (direction == 0) // FORWARD
    position += front * velocity;
  if (direction == 1) // BACKWARD
    position -= front * velocity;
  if (direction == 2) // LEFT
    position -= right * velocity;
  if (direction == 3) // RIGHT
    position += right * velocity;
  if (direction == 4) // UP
    position += worldUp * velocity;
  if (direction == 5) // DOWN
    position -= worldUp * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset,
                                  bool constrainPitch) {
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  glm::vec3 nfront;
  nfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  nfront.y = sin(glm::radians(pitch));
  nfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(nfront);

  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

} // namespace renderer
