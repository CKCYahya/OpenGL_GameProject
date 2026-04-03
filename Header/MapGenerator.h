#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include "../libs/glm/glm.hpp"
#include <vector>

enum TileType { TILE_WATER, TILE_SAND, TILE_SOIL, TILE_GRASS };

class MapGenerator {
public:
  // Generate a TileType based on world coordinates (x, y)
  // Seed allows for different map variations
  static TileType getTileType(int x, int y, int seed);

  // Helper to get specific variation index (0 or 1) for the given tile
  // coordinate
  static int getTextureVariant(int x, int y);
};

#endif
