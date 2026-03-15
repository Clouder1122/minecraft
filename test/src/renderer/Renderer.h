#pragma once

#include "VulkanContext.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Camera.h"

namespace renderer {

struct Vertex {
  glm::vec3 pos;
  glm::vec2 texCoord;
  float light;

  static VkVertexInputBindingDescription getBindingDescription();
  static std::vector<VkVertexInputAttributeDescription>
  getAttributeDescriptions();
};

struct PushConstants {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

// Standard uniform buffer for lighting/environment if needed
struct UniformBufferObject {
  glm::vec3 globalLightDir;
  float ambientStrength;
};

class ChunkMesh;

class Renderer {
public:
  Renderer(GLFWwindow *window);
  ~Renderer();

  void drawFrame(Camera &camera, const std::vector<ChunkMesh *> &chunks);
  void waitIdle();

  VulkanContext &getContext() { return *context; }
  VkCommandPool getCommandPool() { return commandPool; }

private:
  std::unique_ptr<VulkanContext> context;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;

  // Descriptor Sets and Layouts
  VkDescriptorSetLayout descriptorSetLayout;
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  // Texture Atlas
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  // Uniform Buffers
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;
  const int MAX_FRAMES_IN_FLIGHT = 2;

  void createSwapChain();
  void createImageViews();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  void createCommandPool();
  void createDepthResources();
  void createTextureAtlas(); // Procedurally generates an atlas
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void createCommandBuffers();
  void createSyncObjects();
  void updateUniformBuffer(uint32_t currentImage);

  VkShaderModule createShaderModule(const std::vector<char> &code);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
};

} // namespace renderer
