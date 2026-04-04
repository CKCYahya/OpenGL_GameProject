#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "../libs/glad/include/glad/glad.h"
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

#include "../Header/Camera.h"
#include "../Header/EBO.h"
#include "../Header/NewMap.h"
#include "../Header/Shader.h"
#include "../Header/Texture.h"
#include "../Header/VAO.h"
#include "../Header/VBO.h"
#include "../libs/imgui/imgui.h"
class Player;
struct MapTile {
  int gid;
};

struct InstanceData {
  glm::mat4 model;
  glm::vec2 texOffset;
};

class GameMap {
public:
  int MAP_WIDTH_TILES;
  int MAP_HEIGHT_TILES;
  float tileSize;
  float worldWidth;
  float worldHeight;

  std::unique_ptr<Texture> atlasTexture;

  std::vector<MapTile> mapCache;    // Visual Layer
  std::vector<bool> collisionCache; // Collision Layer (true = blocked)
  std::vector<int> waterCache;      // Water Layer (true = water)
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
  int checkWater(float x, float y);

private:
  GLuint GenerateMinimapTexture();
  void SetupMesh();
};

#endif
