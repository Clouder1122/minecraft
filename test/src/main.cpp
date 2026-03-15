#include "input/InputHandler.h"
#include "renderer/Camera.h"
#include "renderer/ChunkMesh.h"
#include "renderer/Renderer.h"
#include "world/World.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint> // Added
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>        // Added
#include <unordered_map> // Added
#include <vector>

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

class VoxelApp {
public:
  void run() {
    initWindow();
    initEngine();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window;
  std::unique_ptr<renderer::Renderer> renderer;
  std::unique_ptr<world::World> world;
  renderer::Camera camera;

  // Chunk meshes
  std::unordered_map<glm::ivec3, std::unique_ptr<renderer::ChunkMesh>,
                     world::ChunkHash>
      chunkMeshes;

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", nullptr, nullptr);
  }

  void initEngine() {
    renderer = std::make_unique<renderer::Renderer>(window);
    camera = renderer::Camera(glm::vec3(0.0f, 80.0f, 0.0f));
    input::InputHandler::init(window, &camera);

    world = std::make_unique<world::World>();
  }

  void updateMeshes() {
    auto &chunks = world->getChunks();

    // Remove meshes for unloaded chunks
    for (auto it = chunkMeshes.begin(); it != chunkMeshes.end();) {
      if (chunks.find(it->first) == chunks.end()) {
        it = chunkMeshes.erase(it);
      } else {
        ++it;
      }
    }

    // Add/update meshes for loaded chunks
    for (const auto &[pos, chunk] : chunks) {
      if (chunk->isDirty) {
        if (chunkMeshes.find(pos) == chunkMeshes.end()) {
          chunkMeshes[pos] = std::make_unique<renderer::ChunkMesh>(
              renderer->getContext(), renderer->getCommandPool());
        }
        chunkMeshes[pos]->updateMesh(*chunk);
        chunk->isDirty = false;
      }
    }
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      float currentFrame = static_cast<float>(glfwGetTime());
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      glfwPollEvents();
      input::InputHandler::processInput(window, deltaTime);

      // Print FPS
      static int frameCount = 0;
      static float timeAccumulator = 0.0f;
      frameCount++;
      timeAccumulator += deltaTime;
      if (timeAccumulator >= 1.0f) {
        std::string title =
            "Voxel Engine - " + std::to_string(frameCount) + " FPS";
        glfwSetWindowTitle(window, title.c_str());
        frameCount = 0;
        timeAccumulator = 0.0f;
      }

      world->update(camera.position,
                    2); // 2 chunks render distance for low-end testing
      updateMeshes();

      std::vector<renderer::ChunkMesh *> meshesToDraw;
      for (const auto &[pos, mesh] : chunkMeshes) {
        meshesToDraw.push_back(mesh.get());
      }

      renderer->drawFrame(camera, meshesToDraw);
    }
  }

  void cleanup() {
    renderer->waitIdle();
    chunkMeshes.clear();
    world.reset();
    renderer.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

int main() {
  VoxelApp app;
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
