#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"
#include "EBO.h"
#include "NewMap.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "imgui.h"
class Player;
struct MapTile {
  int gid;
};

struct InstanceData {
  glm::mat4 model;
  glm::vec2 texOffset;
  float isWater = 0.0f;
};

class GameMap {
public:
  int MAP_WIDTH_TILES;
  int MAP_HEIGHT_TILES;
  float tileSize;
  float worldWidth;
  float worldHeight;
  glm::vec2 tempTexOffset;

  std::unique_ptr<Texture> atlasTexture;

  std::vector<MapTile> mapCache;    // Visual Layer
  std::vector<bool> collisionCache; // Collision Layer (true = blocked)
  std::vector<bool> waterCache;     // Water Layer (true = water)
  GLuint minimapTexID;

  std::unique_ptr<VAO> mapVAO;
  std::unique_ptr<VBO> mapVBO;
  std::unique_ptr<EBO> mapEBO;

  std::unique_ptr<VBO> instanceVBO;
  std::vector<InstanceData> instanceDataArray;

  std::unique_ptr<NewMap> newMap;

  GameMap(const char *mapFile);
  ~GameMap();

  void LoadTextures(Shader &shader);
  void Draw(Shader &shader, Camera &camera);
  void DrawMinimap(glm::vec3 playerPos, Camera &camera, int winWidth,
                   int winHeight);

  // Helper to check collision
  bool checkCollision(float x, float y);
  bool checkWater(float x, float y);

private:
  GLuint GenerateMinimapTexture();
  void SetupMesh();
};

#endif
