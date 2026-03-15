#include "World.h"
#include <iostream>

namespace world {

World::World() {}
World::~World() {}

void World::update(glm::vec3 playerPos, int renderDistance) {
  glm::ivec3 playerChunkPos = worldToChunkPos(
      glm::ivec3(floor(playerPos.x), floor(playerPos.y), floor(playerPos.z)));

  for (int x = -renderDistance; x <= renderDistance; x++) {
    for (int z = -renderDistance; z <= renderDistance; z++) {
      glm::ivec3 targetPos =
          playerChunkPos + glm::ivec3(x * CHUNK_SIZE_X, 0, z * CHUNK_SIZE_Z);
      targetPos.y = 0; // Chunks always start at y=0 for now

      if (chunks.find(targetPos) == chunks.end()) {
        loadChunk(targetPos);
      }
    }
  }

  unloadFarChunks(playerChunkPos, renderDistance);
}

void World::loadChunk(glm::ivec3 chunkPos) {
  auto chunk = std::make_shared<Chunk>(chunkPos);
  chunk->generateTerrain();
  chunks[chunkPos] = chunk;
}

void World::unloadFarChunks(glm::ivec3 playerChunkPos, int renderDistance) {
  auto it = chunks.begin();
  while (it != chunks.end()) {
    glm::ivec3 pos = it->first;
    int dx = abs(pos.x - playerChunkPos.x) / CHUNK_SIZE_X;
    int dz = abs(pos.z - playerChunkPos.z) / CHUNK_SIZE_Z;

    if (dx > renderDistance + 1 || dz > renderDistance + 1) {
      it = chunks.erase(it);
    } else {
      ++it;
    }
  }
}

Block World::getBlock(glm::ivec3 worldPos) {
  glm::ivec3 chunkPos = worldToChunkPos(worldPos);
  if (chunks.count(chunkPos)) {
    glm::ivec3 localPos = worldToLocalPos(worldPos);
    return chunks[chunkPos]->getBlock(localPos.x, localPos.y, localPos.z);
  }
  return {BlockType::Air};
}

void World::setBlock(glm::ivec3 worldPos, Block block) {
  glm::ivec3 chunkPos = worldToChunkPos(worldPos);
  if (chunks.count(chunkPos)) {
    glm::ivec3 localPos = worldToLocalPos(worldPos);
    chunks[chunkPos]->setBlock(localPos.x, localPos.y, localPos.z);
  }
}

glm::ivec3 World::worldToChunkPos(glm::ivec3 worldPos) {
  return {(worldPos.x >= 0)
              ? (worldPos.x / CHUNK_SIZE_X) * CHUNK_SIZE_X
              : ((worldPos.x - CHUNK_SIZE_X + 1) / CHUNK_SIZE_X) * CHUNK_SIZE_X,
          0,
          (worldPos.z >= 0) ? (worldPos.z / CHUNK_SIZE_Z) * CHUNK_SIZE_Z
                            : ((worldPos.z - CHUNK_SIZE_Z + 1) / CHUNK_SIZE_Z) *
                                  CHUNK_SIZE_Z};
}

glm::ivec3 World::worldToLocalPos(glm::ivec3 worldPos) {
  glm::ivec3 chunkPos = worldToChunkPos(worldPos);
  return worldPos - chunkPos;
}

} // namespace world
