#include "Npc.h"
#include "GameMap.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

// --- A* helper ---
struct AStarNode {
  int x, y;
  float g, f;
  bool operator>(const AStarNode &o) const { return f > o.f; }
};

static float heuristic(int x1, int y1, int x2, int y2) {
  return (float)(std::abs(x1 - x2) + std::abs(y1 - y2));
}

// -------------------------------------------------------
Npc::Npc(NpcBehavior behavior) : behavior(behavior) {
  position = glm::vec3(0.0f, 0.0f, 0.0f);
  scale = glm::vec3(32.0f, 32.0f, 0.0f);
  rotation = glm::vec3(0.0f, 0.0f, 0.0f);

  state = NpcState::IDLE;
  fisherState = FisherState::CASTING;
  direction = 0;
  speed = 150.0f;
  idleTimer = 0.0f;
  idleDuration =
      1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
  targetPosition = position;
  fishingTimer = 2.0f;

  if (behavior == NpcBehavior::FISHER) {
    // Fisher: 8 frames, 0.15f speed, 4 cols, 2 rows
    anim = std::make_unique<Animations>(8, 0.15f, 4, 2);
    anim->SetRange(0, 2, false);
  } else {
    // Walker: 8 frames, 0.1f speed, 4 cols, 2 rows
    anim = std::make_unique<Animations>(8, 0.1f, 4, 2);
  }

  // Setup Mesh
  GLfloat vertices[] = {scale.x,  scale.y,  0.0f, 1.0f, 1.0f,
                        scale.x,  -scale.y, 0.0f, 1.0f, 0.0f,
                        -scale.x, -scale.y, 0.0f, 0.0f, 0.0f,
                        -scale.x, scale.y,  0.0f, 0.0f, 1.0f};
  GLuint indices[] = {0, 1, 2, 0, 2, 3};

  vao = std::make_unique<VAO>();
  vao->Bind();
  vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
  ebo = std::make_unique<EBO>(indices, sizeof(indices));
  vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void *)0);
  vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(float),
                  (void *)(3 * sizeof(float)));
  vao->Unbind();
  vbo->Unbind();
  ebo->Unbind();
}

Npc::~Npc() {
  for (auto &pair : textures) {
    for (auto &tex : pair.second) {
      if (tex)
        tex->Delete();
    }
  }
}

void Npc::LoadAssets(Shader &shader) {
  auto &paths =
      (behavior == NpcBehavior::FISHER) ? npcFisherTextureMap : npcTexturePaths;

  for (auto &texturePath : paths) {
    std::vector<std::unique_ptr<Texture>> texts;
    for (auto &path : texturePath.second) {
      texts.push_back(std::make_unique<Texture>(
          path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
      texts.back()->texUnit(shader, "tex0", 0);
    }
    this->textures[texturePath.first] = std::move(texts);
  }
}

void Npc::getAnimation(std::string animType, int direction) {
  if (textures.count(animType) && (int)textures[animType].size() > direction &&
      textures[animType][direction])
    textures[animType][direction]->Bind();
}

void Npc::Draw(Shader &shader, Camera &camera) {
  if (behavior == NpcBehavior::FISHER) {
    // Fisher: always render fishing animation
    glm::vec2 uv0, uv1;
    anim->GetUVCoordinates(uv0, uv1);
    glUniform2f(glGetUniformLocation(shader.ID, "texScale"), uv1.x - uv0.x,
                uv1.y - uv0.y);
    glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), uv0.x, uv0.y);
    // Fisher has only 1 texture (direction 0)
    if (textures.count("fishing") && !textures["fishing"].empty())
      textures["fishing"][0]->Bind();
  } else {
    // Walker
    if (state == NpcState::MOVING) {
      glm::vec2 uv0, uv1;
      anim->GetUVCoordinates(uv0, uv1);
      glUniform2f(glGetUniformLocation(shader.ID, "texScale"), uv1.x - uv0.x,
                  uv1.y - uv0.y);
      glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), uv0.x, uv0.y);
      getAnimation("walk", direction);
    } else {
      glUniform2f(glGetUniformLocation(shader.ID, "texScale"), 1.0f, 1.0f);
      glUniform2f(glGetUniformLocation(shader.ID, "texOffset"), 0.0f, 0.0f);
      getAnimation("idle", direction);
    }
  }

  vao->Bind();

  glm::mat4 model = glm::mat4(1.0f);
  // Z-depth based on the base Y position (feet)
  float base_y = position.y - scale.y;
  float zDepth = -base_y * 0.001f;
  model = glm::translate(model, glm::vec3(position.x, position.y, zDepth));
  model =
      glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model =
      glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(model));

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  vao->Unbind();
}

// -------------------------------------------------------
// UPDATE
// -------------------------------------------------------
void Npc::Update(float deltaTime, GameMap &gameMap) {
  if (behavior == NpcBehavior::FISHER) {
    anim->Update(deltaTime);
    fishingTimer -= deltaTime;

    switch (fisherState) {
    case FisherState::CASTING:
      if (anim->finished) {
        fisherState = FisherState::WAITING;
        anim->SetRange(3, 3, true);
        fishingTimer = 4.0f + static_cast<float>(rand()) /
                                  (static_cast<float>(RAND_MAX / 6.0f));
      }
      break;
    case FisherState::WAITING:
      if (fishingTimer <= 0) {
        fisherState = FisherState::BITING;
        anim->SetRange(4, 6, true);
        fishingTimer = 2.0f;
      }
      break;
    case FisherState::BITING:
      if (fishingTimer <= 0) {
        fisherState = FisherState::SUCCESS;
        anim->SetRange(7, 7, false);
        fishingTimer = 3.0f;
      }
      break;
    case FisherState::SUCCESS:
      if (fishingTimer <= 0) {
        fisherState = FisherState::CASTING;
        anim->SetRange(0, 2, false);
        fishingTimer = 2.0f;
      }
      break;
    }
    return;
  }

  // --- WALKER behavior ---
  if (state == NpcState::IDLE) {
    anim->currentFrame = 0;
    idleTimer += deltaTime;

    if (idleTimer >= idleDuration) {
      PickRandomTarget(gameMap);
      FindPath(gameMap);
      state = NpcState::MOVING;
      currentWaypoint = 0;
      idleTimer = 0.0f;
    }
  } else if (state == NpcState::MOVING) {
    anim->Update(deltaTime);

    if (pathWaypoints.empty() || currentWaypoint >= (int)pathWaypoints.size()) {
      // No path or path finished
      state = NpcState::IDLE;
      idleDuration = 1.0f + static_cast<float>(rand()) /
                                (static_cast<float>(RAND_MAX / 2.0f));
      return;
    }

    glm::vec3 waypoint = pathWaypoints[currentWaypoint];
    glm::vec3 dir = waypoint - position;
    float distance = glm::length(dir);

    if (distance < 3.0f) {
      // Reached this waypoint, advance
      currentWaypoint++;
      if (currentWaypoint >= (int)pathWaypoints.size()) {
        position = waypoint;
        state = NpcState::IDLE;
        idleDuration = 1.0f + static_cast<float>(rand()) /
                                  (static_cast<float>(RAND_MAX / 2.0f));
      }
      return;
    }

    dir = glm::normalize(dir);

    // Determine facing direction
    if (std::abs(dir.x) > std::abs(dir.y)) {
      direction = (dir.x > 0) ? 3 : 2;
    } else {
      direction = (dir.y > 0) ? 1 : 0;
    }

    position += dir * speed * deltaTime;
  }
}

// -------------------------------------------------------
// A* PATHFINDING
// -------------------------------------------------------
glm::ivec2 Npc::WorldToTile(GameMap &gameMap, float x, float y) {
  float localX = x + gameMap.worldWidth / 2.0f;
  float localY = y + gameMap.worldHeight / 2.0f;
  int tileX = (int)(localX / gameMap.tileSize);
  int tileY = (int)(localY / gameMap.tileSize);
  int tiledRow = gameMap.MAP_HEIGHT_TILES - 1 - tileY;

  tileX = std::max(0, std::min(tileX, gameMap.MAP_WIDTH_TILES - 1));
  tiledRow = std::max(0, std::min(tiledRow, gameMap.MAP_HEIGHT_TILES - 1));

  return glm::ivec2(tileX, tiledRow);
}

glm::vec3 Npc::TileToWorld(GameMap &gameMap, int tileX, int tiledRow) {
  int curY = gameMap.MAP_HEIGHT_TILES - 1 - tiledRow;
  float xPos = (tileX * gameMap.tileSize) - (gameMap.worldWidth / 2.0f) +
               gameMap.tileSize / 2.0f;
  float yPos = (curY * gameMap.tileSize) - (gameMap.worldHeight / 2.0f) +
               gameMap.tileSize / 2.0f;
  return glm::vec3(xPos, yPos, 0.0f);
}

void Npc::FindPath(GameMap &gameMap) {
  pathWaypoints.clear();
  currentWaypoint = 0;

  int mapW = gameMap.MAP_WIDTH_TILES;
  int mapH = gameMap.MAP_HEIGHT_TILES;

  glm::ivec2 start = WorldToTile(gameMap, position.x, position.y);
  glm::ivec2 goal = WorldToTile(gameMap, targetPosition.x, targetPosition.y);

  // Check if goal is blocked — if so, find nearest unblocked tile
  int goalIdx = goal.y * mapW + goal.x;
  if (goalIdx >= 0 && goalIdx < (int)gameMap.collisionCache.size() &&
      gameMap.collisionCache[goalIdx]) {
    // Goal is blocked, skip pathfinding
    return;
  }

  // Also check water
  if (goalIdx >= 0 && goalIdx < (int)gameMap.waterCache.size() &&
      gameMap.waterCache[goalIdx] != 0) {
    return;
  }

  if (start == goal) {
    pathWaypoints.push_back(TileToWorld(gameMap, goal.x, goal.y));
    return;
  }

  // A* on tile grid
  auto key = [mapW](int x, int y) { return y * mapW + x; };

  std::priority_queue<AStarNode, std::vector<AStarNode>,
                      std::greater<AStarNode>>
      openSet;
  std::unordered_map<int, float> gScore;
  std::unordered_map<int, int> cameFrom; // maps key -> key

  int startKey = key(start.x, start.y);
  int goalKey = key(goal.x, goal.y);

  gScore[startKey] = 0.0f;
  openSet.push(
      {start.x, start.y, 0.0f, heuristic(start.x, start.y, goal.x, goal.y)});

  // Direction offsets (4-directional)
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};

  bool found = false;
  int iterations = 0;
  const int maxIterations = 5000;

  while (!openSet.empty() && iterations < maxIterations) {
    iterations++;
    AStarNode current = openSet.top();
    openSet.pop();

    int curKey = key(current.x, current.y);

    if (curKey == goalKey) {
      found = true;
      break;
    }

    // Skip if we already processed a better path to this node
    if (gScore.count(curKey) && current.g > gScore[curKey])
      continue;

    for (int d = 0; d < 4; d++) {
      int nx = current.x + dx[d];
      int ny = current.y + dy[d];

      if (nx < 0 || nx >= mapW || ny < 0 || ny >= mapH)
        continue;

      int nIdx = ny * mapW + nx;
      if (gameMap.collisionCache[nIdx])
        continue;
      if (gameMap.waterCache[nIdx] != 0)
        continue;

      int nKey = key(nx, ny);
      float tentG = current.g + 1.0f;

      if (!gScore.count(nKey) || tentG < gScore[nKey]) {
        gScore[nKey] = tentG;
        cameFrom[nKey] = curKey;
        float f = tentG + heuristic(nx, ny, goal.x, goal.y);
        openSet.push({nx, ny, tentG, f});
      }
    }
  }

  if (!found)
    return;

  // Reconstruct path
  std::vector<glm::ivec2> tilePath;
  int curKey = goalKey;
  while (curKey != startKey) {
    int ty = curKey / mapW;
    int tx = curKey % mapW;
    tilePath.push_back(glm::ivec2(tx, ty));
    if (!cameFrom.count(curKey))
      break;
    curKey = cameFrom[curKey];
  }
  std::reverse(tilePath.begin(), tilePath.end());

  // Convert to world positions, skip every few tiles for smoother movement
  for (auto &tile : tilePath) {
    pathWaypoints.push_back(TileToWorld(gameMap, tile.x, tile.y));
  }
}

// -------------------------------------------------------
void Npc::PickRandomTarget(GameMap &gameMap) {
  float halfSize = scale.x / 2.0f;
  float minX = -gameMap.worldWidth / 2.0f + halfSize;
  float maxX = gameMap.worldWidth / 2.0f - halfSize;
  float minY = -gameMap.worldHeight / 2.0f + halfSize;
  float maxY = gameMap.worldHeight / 2.0f - halfSize;

  float randomX = minX + static_cast<float>(rand()) /
                             (static_cast<float>(RAND_MAX / (maxX - minX)));
  float randomY = minY + static_cast<float>(rand()) /
                             (static_cast<float>(RAND_MAX / (maxY - minY)));

  targetPosition = glm::vec3(randomX, randomY, position.z);
}

nlohmann::json Npc::ToJson() {
  nlohmann::json j;
  j["position"]["x"] = position.x;
  j["position"]["y"] = position.y;
  j["position"]["z"] = position.z;
  j["targetPosition"]["x"] = targetPosition.x;
  j["targetPosition"]["y"] = targetPosition.y;
  j["targetPosition"]["z"] = targetPosition.z;
  j["direction"] = direction;
  j["state"] = static_cast<int>(state);
  j["idleTimer"] = idleTimer;
  j["idleDuration"] = idleDuration;
  j["behavior"] = static_cast<int>(behavior);
  return j;
}

void Npc::FromJson(nlohmann::json j) {
  if (j.contains("position")) {
    position.x = j["position"]["x"];
    position.y = j["position"]["y"];
    position.z = j["position"]["z"];
  }
  if (j.contains("targetPosition")) {
    targetPosition.x = j["targetPosition"]["x"];
    targetPosition.y = j["targetPosition"]["y"];
    targetPosition.z = j["targetPosition"]["z"];
  }
  if (j.contains("direction"))
    direction = j["direction"];
  if (j.contains("state"))
    state = static_cast<NpcState>(j["state"]);
  if (j.contains("idleTimer"))
    idleTimer = j["idleTimer"];
  if (j.contains("idleDuration"))
    idleDuration = j["idleDuration"];
}

void Npc::Reset() {
  position = glm::vec3(0.0f, 0.0f, 0.0f);
  targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
  direction = 0;
  state = NpcState::IDLE;
  idleTimer = 0.0f;
  idleDuration = 1.0f;
  pathWaypoints.clear();
  currentWaypoint = 0;
}