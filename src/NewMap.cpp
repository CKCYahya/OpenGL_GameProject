#include "NewMap.h"
#include <fstream>
#include <iostream>
#include <map>

NewMap::NewMap(const char *filename) {
  std::ifstream dataFile(filename);
  if (dataFile.is_open()) {
    try {
      json mapData = json::parse(dataFile);
      ParseMap(mapData);
      ParseTilesets(mapData);
      std::cout << "Successfully loaded " << filename << std::endl;
    } catch (json::parse_error &e) {
      std::cerr << "JSON Parse Error in " << filename << ": " << e.what()
                << std::endl;
    }
  } else {
    std::cerr << "Error: " << filename << " not found." << std::endl;
  }
}

void NewMap::ParseMap(const json &mapData) {
  // Basic map info
  mapWidth = mapData.value("width", 0);
  mapHeight = mapData.value("height", 0);
  tileSize = mapData.value("tileheight", 32);

  // Iterate through layers
  if (mapData.contains("layers")) {
    for (const auto &layer : mapData["layers"]) {
      std::string type = layer.value("type", "");
      std::string name = layer.value("name", "");

      if (type == "tilelayer") {
        if (name == "Tile Layer 1") {
          tileIDs = layer["data"].get<std::vector<int>>();
        } else if (name == "collisions") {
          collisionIDs = layer["data"].get<std::vector<int>>();
        }
      }
    }
  }
}

void NewMap::ParseTilesets(const json &mapData) {
  // Initialize minimap colors with default black
  minimapColors.assign(mapWidth * mapHeight * 3, 0);

  // Map to store Global Tile ID -> Tile Type Value
  // 0: Grass (#00FF00)
  // 1: Dirt (#964B00)
  // 2: Water (#0000FF)
  // 3: Cyan (#00FFFF)
  std::map<int, int> tileTypes;

  if (mapData.contains("tilesets")) {
    for (const auto &tileset : mapData["tilesets"]) {
      int firstGid = tileset.value("firstgid", 1);

      if (tileset.contains("tiles")) {
        // tiles is an array of objects
        for (const auto &tile : tileset["tiles"]) {
          int localID = tile.value("id", -1);
          if (localID == -1)
            continue;

          int globalID = firstGid + localID;

          if (tile.contains("properties")) {
            for (const auto &prop : tile["properties"]) {
              if (prop.value("name", "") == "tileType") {
                int val = prop.value("value", -1);
                if (val != -1) {
                  tileTypes[globalID] = val;
                }
              }
            }
          }
        }
      }
    }
  }

  // Colors based on User Logic
  // 0: Grass, 1: Dirt, 2: Water, 3: Cyan, Else: Black
  struct Color {
    unsigned char r, g, b;
  };
  Color palette[] = {
      {0, 255, 0},   // 0: Grass (Green)
      {150, 75, 0},  // 1: Dirt (Brown) - #964B00 = 150, 75, 0
      {0, 0, 255},   // 2: Water (Blue)
      {0, 255, 255}, // 3: Cyan
      {0, 0, 0}      // Default
  };

  // Populate minimapColors based on VISUAL layer (tileIDs)
  for (size_t i = 0; i < tileIDs.size(); ++i) {
    int gid = tileIDs[i];
    if (gid == 0)
      continue; // Empty

    int type = -1;
    if (tileTypes.find(gid) != tileTypes.end()) {
      type = tileTypes[gid];
    }

    Color c = palette[4]; // Default Black
    if (type >= 0 && type <= 3) {
      c = palette[type];
    } else {
      // Fallback for known GIDs that might not have properties?
      // Or keep black.
    }

    minimapColors[i * 3 + 0] = c.r;
    minimapColors[i * 3 + 1] = c.g;
    minimapColors[i * 3 + 2] = c.b;
  }
}
