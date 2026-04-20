#include "MapGenerator.h"
#include "glm/gtc/noise.hpp"

constexpr float scale = 0.05f;

TileType MapGenerator::getTileType(int x, int y, int seed) {

  // Offset by seed to change map
  float xCoord = x * scale + seed * 100.0f;
  float yCoord = y * scale + seed * 100.0f;

  // Use Simplex Noise from GLM (returns -1.0 to 1.0)
  float noiseValue = glm::simplex(glm::vec2(xCoord, yCoord));

  // Define Thresholds for Biomes
  // We want: Water -> Sand -> Soil -> Grass
  // Adjust these thresholds to change rarity/abundance

  if (noiseValue < -0.3f) {
    return TILE_WATER;
  } else if (noiseValue < -0.1f) {
    return TILE_SAND;
  } else if (noiseValue < 0.2f) {
    return TILE_SOIL;
  } else {
    return TILE_GRASS;
  }
}

int MapGenerator::getTextureVariant(int x, int y) {
  // Simple deterministic hash for variation (0 or 1)
  // Avoid using noise here to prevent clustering of variations
  // pseudo-random but deterministic
  unsigned int hash = (x * 73856093) ^ (y * 19349663);
  return hash % 2;
}
