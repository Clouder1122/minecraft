#pragma once

#include "Chunk.h"
#include <memory>
#include <mutex>
#include <unordered_map>


namespace world {

struct ChunkHash {
  size_t operator()(const glm::ivec3 &v) const {
    return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^
           (std::hash<int>()(v.z) << 2);
  }
};

class World {
public:
  World();
  ~World();

  void update(glm::vec3 playerPos, int renderDistance);

  std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ChunkHash> &
  getChunks() {
    return chunks;
  }

  Block getBlock(glm::ivec3 worldPos);
  void setBlock(glm::ivec3 worldPos, Block block);

  static glm::ivec3 worldToChunkPos(glm::ivec3 worldPos);
  static glm::ivec3 worldToLocalPos(glm::ivec3 worldPos);

private:
  std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, ChunkHash> chunks;
  std::mutex chunksMutex;

  void loadChunk(glm::ivec3 chunkPos);
  void unloadFarChunks(glm::ivec3 playerChunkPos, int renderDistance);
};

} // namespace world
