#include "Chunk.h"
#include <algorithm>

namespace world {

Chunk::Chunk(glm::ivec3 position) : position(position) {
  blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, {BlockType::Air});
}

Chunk::~Chunk() {}

void Chunk::generateTerrain() {
  for (int x = 0; x < CHUNK_SIZE_X; x++) {
    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
      // Simple hill based on sine waves for testing
      float worldX = (float)(position.x + x);
      float worldZ = (float)(position.z + z);
      int height =
          static_cast<int>(64 + 16 * sin(worldX * 0.1f) * cos(worldZ * 0.1f));

      for (int y = 0; y < CHUNK_SIZE_Y; y++) {
        if (y < height - 4) {
          setBlock(x, y, z, {BlockType::Stone});
        } else if (y < height - 1) {
          setBlock(x, y, z, {BlockType::Dirt});
        } else if (y == height - 1) {
          setBlock(x, y, z, {BlockType::Grass});
        } else {
          setBlock(x, y, z, {BlockType::Air});
        }
      }
    }
  }
  isDirty = true;
}

Block Chunk::getBlock(int x, int y, int z) const {
  if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 ||
      z >= CHUNK_SIZE_Z) {
    return {BlockType::Air};
  }
  return blocks[getIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, Block block) {
  if (x >= 0 && x < CHUNK_SIZE_X && y >= 0 && y < CHUNK_SIZE_Y && z >= 0 &&
      z < CHUNK_SIZE_Z) {
    blocks[getIndex(x, y, z)] = block;
    isDirty = true;
  }
}

} // namespace world
