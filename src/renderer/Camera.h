#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

class Camera {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  float yaw;
  float pitch;
  float movementSpeed;
  float mouseSensitivity;

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f,
         float pitch = 0.0f);

  glm::mat4 getViewMatrix() {
    return glm::lookAt(position, position + front, up);
  }

  void processKeyboard(int direction, float deltaTime);
  void processMouseMovement(float xoffset, float yoffset,
                            bool constrainPitch = true);

private:
  void updateCameraVectors();
};

} // namespace renderer
