#include "GameMap.h"

GameMap::GameMap(const char *mapFile) {
  // Load Map Data
  newMap = std::make_unique<NewMap>(mapFile);

  MAP_WIDTH_TILES = newMap->getWidth();
  MAP_HEIGHT_TILES = newMap->getHeight();
  tileSize = (float)newMap->getTileSize();

  worldWidth = MAP_WIDTH_TILES * tileSize;
  worldHeight = MAP_HEIGHT_TILES * tileSize;

  // Tiled stores data as 1D array row-by-row
  const auto &tiles = newMap->getTileIDs();
  mapCache.resize(tiles.size());
  for (size_t i = 0; i < tiles.size(); ++i) {
    mapCache[i].gid = tiles[i];
  }

  // Fill Collision Cache
  const auto &colData = newMap->getCollisionIDs();
  collisionCache.resize(colData.size());
  waterCache.resize(colData.size());
  for (size_t i = 0; i < colData.size(); ++i) {
    // If GID > 0, it's a collision tile
    collisionCache[i] = (colData[i] == 2000);
    if (colData[i] == 2014 || colData[i] == 2015 || colData[i] == 2016) {
      waterCache[i] = colData[i];
    } else {
      waterCache[i] = 0;
    }
  }

  atlasTexture =
      std::make_unique<Texture>("image/texture.png", GL_TEXTURE_2D, GL_TEXTURE0,
                                GL_RGBA, GL_UNSIGNED_BYTE);

  // Generate Minimap
  minimapTexID = GenerateMinimapTexture();

  // Setup Mesh
  SetupMesh();
}

GameMap::~GameMap() {
  if (atlasTexture)
    atlasTexture->Delete();
}

void GameMap::SetupMesh() {
  float halfSize = tileSize / 2.0f;

  GLfloat backgroundVertices[] = {
      // Pos                  // Tex
      halfSize,  halfSize,  0.0f, 1.0f, 1.0f, // Top Right
      halfSize,  -halfSize, 0.0f, 1.0f, 0.0f, // Bottom Right
      -halfSize, -halfSize, 0.0f, 0.0f, 0.0f, // Bottom Left
      -halfSize, halfSize,  0.0f, 0.0f, 1.0f  // Top Left
  };

  GLuint indices[] = {0, 1, 2, 0, 2, 3};

  mapVAO = std::make_unique<VAO>();
  mapVAO->Bind();

  mapVBO =
      std::make_unique<VBO>(backgroundVertices, sizeof(backgroundVertices));
  mapEBO = std::make_unique<EBO>(indices, sizeof(indices));

  mapVAO->LinkAttrib(*mapVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
  mapVAO->LinkAttrib(*mapVBO, 1, 2, GL_FLOAT, 5 * sizeof(float),
                     (void *)(3 * sizeof(float)));

  if (!atlasTexture) {
    mapVAO->Unbind();
    mapVBO->Unbind();
    mapEBO->Unbind();
    return;
  }

  float atlasWidth = (float)atlasTexture->width;
  float atlasHeight = (float)atlasTexture->height;
  int cols = (int)(atlasWidth / 32.0f);
  int totalRows = (int)(atlasHeight / 32.0f);

  instanceDataArray.clear();
  instanceDataArray.reserve(MAP_WIDTH_TILES * MAP_HEIGHT_TILES);

  for (int curY = 0; curY < MAP_HEIGHT_TILES; curY++) {
    for (int curX = 0; curX < MAP_WIDTH_TILES; curX++) {
      float xPos = (curX * tileSize) - (worldWidth / 2.0f) + tileSize / 2.0f;
      float yPos = (curY * tileSize) - (worldHeight / 2.0f) + tileSize / 2.0f;

      int tiledRow = MAP_HEIGHT_TILES - 1 - curY;
      int tileIndex = tiledRow * MAP_WIDTH_TILES + curX;

      if (tileIndex < 0 || tileIndex >= mapCache.size())
        continue;

      int gid = mapCache[tileIndex].gid;
      if (gid == 0)
        continue; // Empty tile

      int localID = gid - 1;
      int col = localID % cols;
      int row = localID / cols;

      int texRow = totalRows - 1 - row;
      float uOffset = (col * 32.0f) / atlasWidth;
      float vOffset = (texRow * 32.0f) / atlasHeight;

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));

      InstanceData inst;
      inst.model = model;
      inst.texOffset = glm::vec2(uOffset, vOffset);
      if (waterCache[tileIndex] != 0) {
        inst.isWater = 1.0f;
      }
      instanceDataArray.push_back(inst);
    }
  }

  // Create Instance VBO
  instanceVBO =
      std::make_unique<VBO>((GLfloat *)instanceDataArray.data(),
                            instanceDataArray.size() * sizeof(InstanceData));

  // Configure attributes
  instanceVBO->Bind();

  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(2 + i);
    glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                          (void *)(i * sizeof(glm::vec4)));
    glVertexAttribDivisor(2 + i, 1);
  }

  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)(sizeof(glm::mat4)));
  glVertexAttribDivisor(6, 1);

  glEnableVertexAttribArray(7);
  glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
                        (void *)(sizeof(glm::mat4) + sizeof(glm::vec2)));
  glVertexAttribDivisor(7, 1);

  instanceVBO->Unbind();
  mapVAO->Unbind();
  mapVBO->Unbind();
  mapEBO->Unbind();
}

void GameMap::LoadTextures(Shader &shader) {
  // Just bind the atlas to unit 0
  if (atlasTexture) {
    atlasTexture->texUnit(shader, "tex0", 0);
  }
}

void GameMap::Draw(Shader &shader, Camera &camera) {
  shader.Activate();

  if (atlasTexture)
    atlasTexture->Bind();

  if (!atlasTexture)
    return;

  // Use atlasTexture width/height dynamically instead of hardcoding
  float atlasWidth = (float)atlasTexture->width;
  float atlasHeight = (float)atlasTexture->height;

  glm::vec2 texScale(32.0f / atlasWidth, 32.0f / atlasHeight);

  glUniform2fv(glGetUniformLocation(shader.ID, "texScale"), 1,
               glm::value_ptr(texScale));

  // Draw instances statically
  if (!instanceDataArray.empty()) {
    static float lastUpdateTime = 0.0f;
    float currentTime = glfwGetTime();
    if (currentTime - lastUpdateTime >= 0.5f) {
      lastUpdateTime = currentTime;
      bool firstEncountered = false;
      for (int i = 0; i < (int)instanceDataArray.size() - 1; i++) {
        if (instanceDataArray[i].isWater == 1.0f) {
          if (firstEncountered == false) {
            temp = instanceDataArray[i].texOffset;
            firstEncountered = true;
          }
          if (instanceDataArray[i + 1].isWater != 1.0f) {
            instanceDataArray[i].texOffset = temp;
          }
          instanceDataArray[i].texOffset = instanceDataArray[i + 1].texOffset;
        }
      }
      instanceVBO->Bind();
      glBufferSubData(GL_ARRAY_BUFFER, 0,
                      instanceDataArray.size() * sizeof(InstanceData),
                      instanceDataArray.data());
      instanceVBO->Unbind();
    }
    mapVAO->Bind();
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            (GLsizei)instanceDataArray.size());
    mapVAO->Unbind();
  }
}

GLuint GameMap::GenerateMinimapTexture() {
  int w = MAP_WIDTH_TILES;
  int h = MAP_HEIGHT_TILES;
  const auto &colors = newMap->getMinimapColors();

  std::vector<unsigned char> flippedData(colors.size());
  int rowStride = w * 3;
  for (int y = 0; y < h; ++y) {
    // Source row y (0 is Top)
    // Dest row (h-1-y) (0 is Bottom)
    // We want Top of Map (Row 0) to be at Top of Texture (Row h-1).
    const unsigned char *srcRow = &colors[y * rowStride];
    unsigned char *dstRow = &flippedData[(h - 1 - y) * rowStride];
    std::memcpy(dstRow, srcRow, rowStride);
  }

  GLuint texID;
  glGenTextures(1, &texID);
  glBindTexture(GL_TEXTURE_2D, texID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Fix alignment for 3-channel (RGB) textures where width is not divisible by
  // 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE,
               flippedData.data());
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  return texID;
}

void GameMap::DrawMinimap(glm::vec3 playerPos, Camera &camera, int winWidth,
                          int winHeight) {
  // Calculate window coords for minimap
  const float PAD = 10.0f;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;

  ImGui::SetNextWindowPos(ImVec2(10, work_pos.y + work_size.y - 10),
                          ImGuiCond_Always, ImVec2(0.0f, 1.0f));
  if (ImGui::Begin(
          "MinimapOverlay", NULL,
          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
              ImGuiWindowFlags_NoSavedSettings |
              ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
    ImVec2 mapSize(200, 200);
    ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::Image((void *)(intptr_t)minimapTexID, mapSize, ImVec2(0, 1),
                 ImVec2(1, 0));

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // Player Dot
    // Map World Range [-W/2, W/2]
    float normPX = (playerPos.x + worldWidth / 2.0f) / worldWidth;
    float normPY =
        1.0f - ((playerPos.y + worldHeight / 2.0f) / worldHeight); // Top is 0

    ImVec2 playerScreenPos =
        ImVec2(p.x + normPX * mapSize.x, p.y + normPY * mapSize.y);
    draw_list->AddCircleFilled(playerScreenPos, 3.0f, IM_COL32(255, 0, 0, 255));

    // Camera Rect
    float worldLeft = camera.Position.x;
    float worldRight = camera.Position.x + winWidth * camera.zoom;
    float worldBot = camera.Position.y;
    float worldTop = camera.Position.y + winHeight * camera.zoom;

    auto ToUI = [&](float wx, float wy) {
      return ImVec2(p.x + ((wx + worldWidth / 2.0f) / worldWidth) * mapSize.x,
                    p.y + (1.0f - ((wy + worldHeight / 2.0f) / worldHeight)) *
                              mapSize.y);
    };

    ImVec2 pMin = ToUI(worldLeft, worldTop);
    ImVec2 pMax = ToUI(worldRight, worldBot);

    draw_list->AddRect(pMin, pMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.5f);
  }

  ImGui::End();
}

bool GameMap::checkCollision(float x, float y) {
  float localX = x + worldWidth / 2.0f;
  float localY = y + worldHeight / 2.0f;

  if (localX < 0 || localX >= worldWidth || localY < 0 ||
      localY >= worldHeight) {
    return true;
  }

  int tileX = (int)(localX / tileSize);
  int tileY = (int)(localY / tileSize);

  int tiledRow = MAP_HEIGHT_TILES - 1 - tileY;
  int index = tiledRow * MAP_WIDTH_TILES + tileX;

  if (index >= 0 && index < collisionCache.size()) {
    return collisionCache[index];
  }
  return false;
}

int GameMap::checkWater(float x, float y) {
  float localX = x + worldWidth / 2.0f;
  float localY = y + worldHeight / 2.0f;

  if (localX < 0 || localX >= worldWidth || localY < 0 ||
      localY >= worldHeight) {
    return 0; // Out of bounds is not water
  }

  int tileX = (int)(localX / tileSize);
  int tileY = (int)(localY / tileSize);

  int tiledRow = MAP_HEIGHT_TILES - 1 - tileY;
  int index = tiledRow * MAP_WIDTH_TILES + tileX;

  if (index >= 0 && index < waterCache.size()) {
    return waterCache[index];
  }
  return 0;
}
