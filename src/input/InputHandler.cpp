#include "InputHandler.h"

namespace input {

bool InputHandler::firstMouse = true;
float InputHandler::lastX = 640.0f;
float InputHandler::lastY = 360.0f;
renderer::Camera *InputHandler::currentCamera = nullptr;

void InputHandler::init(GLFWwindow *window, renderer::Camera *camera) {
  currentCamera = camera;
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::processInput(GLFWwindow *window, float deltaTime) {
  if (!currentCamera)
    return;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    currentCamera->processKeyboard(0, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    currentCamera->processKeyboard(1, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    currentCamera->processKeyboard(2, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    currentCamera->processKeyboard(3, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    currentCamera->processKeyboard(4, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    currentCamera->processKeyboard(5, deltaTime);
}

void InputHandler::mouseCallback(GLFWwindow *window, double xposIn,
                                 double yposIn) {
  if (!currentCamera)
    return;

  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  currentCamera->processMouseMovement(xoffset, yoffset);
}

} // namespace input
