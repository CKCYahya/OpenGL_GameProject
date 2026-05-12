#ifndef NEWMAP_H
#define NEWMAP_H

#include "glad/glad.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using json = nlohmann::json;

class NewMap {
public:
  NewMap(const char *filename);

  int getWidth() const { return mapWidth; }
  int getHeight() const { return mapHeight; }
  int getTileSize() const { return tileSize; }

  // Visual Layer
  const std::vector<int> &getTileIDs() const { return tileIDs; }
  const std::vector<int> &getTileIDs2() const { return tileIDs2; }

  // Collision Layer
  const std::vector<int> &getCollisionIDs() const { return collisionIDs; }

  const std::vector<unsigned char> &getMinimapColors() const {
    return minimapColors;
  }

private:
  int mapWidth;
  int mapHeight;
  int tileSize;

  std::vector<int> tileIDs;      // Visual GIDs
  std::vector<int> tileIDs2;     // Visual GIDs
  std::vector<int> collisionIDs; // Collision GIDs

  std::vector<unsigned char> minimapColors;

  void ParseMap(const json &mapData);
  void ParseTilesets(const json &mapData);
};
#endif // NEWMAP_H