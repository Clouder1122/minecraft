#include "ChunkMesh.h"
#include "VulkanUtil.h"
#include <array>

namespace renderer {

// Face directions: 0:+X, 1:-X, 2:+Y, 3:-Y, 4:+Z, 5:-Z
static const glm::ivec3 DIRECTION_VECTORS[] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

static const glm::vec3 FACE_VERTICES[6][4] = {
    {{1, 1, 1}, {1, 1, 0}, {1, 0, 0}, {1, 0, 1}}, // +X
    {{0, 1, 0}, {0, 1, 1}, {0, 0, 1}, {0, 0, 0}}, // -X
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}, // +Y
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}, // -Y
    {{0, 1, 1}, {0, 0, 1}, {1, 0, 1}, {1, 1, 1}}, // +Z
    {{1, 1, 0}, {1, 0, 0}, {0, 0, 0}, {0, 1, 0}}  // -Z
};

static const glm::vec2 FACE_UVS[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

ChunkMesh::ChunkMesh(VulkanContext &context, VkCommandPool commandPool)
    : context(context), commandPool(commandPool) {}

ChunkMesh::~ChunkMesh() {
  if (vertexBuffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(context.device, vertexBuffer, nullptr);
    vkFreeMemory(context.device, vertexBufferMemory, nullptr);
    vkDestroyBuffer(context.device, indexBuffer, nullptr);
    vkFreeMemory(context.device, indexBufferMemory, nullptr);
  }
}

void ChunkMesh::updateMesh(const world::Chunk &chunk) {
  MeshData data;
  buildMeshData(chunk, data);
  createBuffers(data);
}

void ChunkMesh::buildMeshData(const world::Chunk &chunk, MeshData &data) {
  for (int y = 0; y < world::CHUNK_SIZE_Y; y++) {
    for (int z = 0; z < world::CHUNK_SIZE_Z; z++) {
      for (int x = 0; x < world::CHUNK_SIZE_X; x++) {
        world::Block block = chunk.getBlock(x, y, z);
        if (!block.isSolid())
          continue;

        for (int f = 0; f < 6; f++) {
          glm::ivec3 neighborPos = glm::ivec3(x, y, z) + DIRECTION_VECTORS[f];
          world::Block neighbor =
              chunk.getBlock(neighborPos.x, neighborPos.y, neighborPos.z);

          if (neighbor.isTransparent()) {
            addFace(data, glm::vec3(x, y, z), f, block.type);
          }
        }
      }
    }
  }
}

void ChunkMesh::addFace(MeshData &data, glm::vec3 pos, int faceIndex,
                        world::BlockType type) {
  uint32_t startIndex = static_cast<uint32_t>(data.vertices.size());

  float light = 1.0f;
  if (faceIndex == 2)
    light = 1.0f; // Top
  else if (faceIndex == 3)
    light = 0.4f; // Bottom
  else
    light = 0.7f; // Sides

  for (int i = 0; i < 4; i++) {
    Vertex v;
    v.pos = pos + FACE_VERTICES[faceIndex][i];
    v.texCoord = FACE_UVS[i]; // Atlas logic here later
    v.light = light;
    data.vertices.push_back(v);
  }

  data.indices.push_back(startIndex + 0);
  data.indices.push_back(startIndex + 1);
  data.indices.push_back(startIndex + 2);
  data.indices.push_back(startIndex + 2);
  data.indices.push_back(startIndex + 3);
  data.indices.push_back(startIndex + 0);
}

void ChunkMesh::createBuffers(const MeshData &data) {
  if (data.vertices.empty())
    return;

  if (vertexBuffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(context.device, vertexBuffer, nullptr);
    vkFreeMemory(context.device, vertexBufferMemory, nullptr);
    vkDestroyBuffer(context.device, indexBuffer, nullptr);
    vkFreeMemory(context.device, indexBufferMemory, nullptr);
  }

  VkDeviceSize vertexSize = sizeof(Vertex) * data.vertices.size();
  VkDeviceSize indexSize = sizeof(uint32_t) * data.indices.size();
  indexCount = static_cast<uint32_t>(data.indices.size());

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  // Vertex Buffer
  VulkanUtil::createBuffer(context, vertexSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           stagingBuffer, stagingBufferMemory);
  void *vData;
  vkMapMemory(context.device, stagingBufferMemory, 0, vertexSize, 0, &vData);
  memcpy(vData, data.vertices.data(), (size_t)vertexSize);
  vkUnmapMemory(context.device, stagingBufferMemory);

  VulkanUtil::createBuffer(
      context, vertexSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
  VulkanUtil::copyBuffer(context, commandPool, stagingBuffer, vertexBuffer,
                         vertexSize);

  vkDestroyBuffer(context.device, stagingBuffer, nullptr);
  vkFreeMemory(context.device, stagingBufferMemory, nullptr);

  // Index Buffer
  VulkanUtil::createBuffer(context, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           stagingBuffer, stagingBufferMemory);
  void *iData;
  vkMapMemory(context.device, stagingBufferMemory, 0, indexSize, 0, &iData);
  memcpy(iData, data.indices.data(), (size_t)indexSize);
  vkUnmapMemory(context.device, stagingBufferMemory);

  VulkanUtil::createBuffer(
      context, indexSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
  VulkanUtil::copyBuffer(context, commandPool, stagingBuffer, indexBuffer,
                         indexSize);

  vkDestroyBuffer(context.device, stagingBuffer, nullptr);
  vkFreeMemory(context.device, stagingBufferMemory, nullptr);
}

void ChunkMesh::draw(VkCommandBuffer commandBuffer, VkPipelineLayout layout) {
  if (vertexBuffer == VK_NULL_HANDLE)
    return;

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}

} // namespace renderer
