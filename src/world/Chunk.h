#pragma once

#include "Block.h"
#include <glm/glm.hpp>
#include <vector>


namespace world {

class Chunk {
public:
  Chunk(glm::ivec3 position);
  ~Chunk();

  void generateTerrain();
  Block getBlock(int x, int y, int z) const;
  void setBlock(int x, int y, int z, Block type);

  glm::ivec3 getPosition() const { return position; }

  // Flags for meshing
  bool isDirty = true;

private:
  glm::ivec3 position; // World-space position (multiples of CHUNK_SIZE)
  std::vector<Block> blocks;

  size_t getIndex(int x, int y, int z) const {
    return x + (z * CHUNK_SIZE_X) + (y * CHUNK_SIZE_X * CHUNK_SIZE_Z);
  }
};

} // namespace world
