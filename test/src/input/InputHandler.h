#pragma once

#include "renderer/Camera.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <glm/glm.hpp>


namespace input {

class InputHandler {
public:
  static void init(GLFWwindow *window, renderer::Camera *camera);
  static void processInput(GLFWwindow *window, float deltaTime);

  static bool firstMouse;
  static float lastX;
  static float lastY;
  static renderer::Camera *currentCamera;

private:
  static void mouseCallback(GLFWwindow *window, double xposIn, double yposIn);
};

} // namespace input
