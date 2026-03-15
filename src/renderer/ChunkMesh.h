#pragma once

#include "renderer/Renderer.h"
#include "world/Chunk.h"
#include <vector>

namespace renderer {

class ChunkMesh {
public:
  ChunkMesh(VulkanContext &context, VkCommandPool commandPool);
  ~ChunkMesh();

  void updateMesh(const world::Chunk &chunk);
  void draw(VkCommandBuffer commandBuffer, VkPipelineLayout layout);

private:
  struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
  };

  VulkanContext &context;
  VkCommandPool commandPool;

  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

  uint32_t indexCount = 0;

  void buildMeshData(const world::Chunk &chunk, MeshData &data);
  void addFace(MeshData &data, glm::vec3 pos, int faceIndex,
               world::BlockType type);

  void createBuffers(const MeshData &data);
};

} // namespace renderer
