#pragma once

#include <cstdint>

namespace world {

enum class BlockType : uint8_t {
  Air = 0,
  Stone = 1,
  Grass = 2,
  Dirt = 3,
  Wood = 4,
  Leaves = 5,
  Bedrock = 6,
  Water = 7,
  Sand = 8,
  Count
};

struct Block {
  BlockType type = BlockType::Air;

  bool isTransparent() const {
    return type == BlockType::Air || type == BlockType::Leaves ||
           type == BlockType::Water;
  }

  bool isSolid() const { return type != BlockType::Air; }
};

static const int CHUNK_SIZE_X = 16;
static const int CHUNK_SIZE_Y = 256;
static const int CHUNK_SIZE_Z = 16;

} // namespace world
