#ifndef NPC_H
#define NPC_H

#include "Animations.h"
#include "Camera.h"
#include "EBO.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "nlohmann/json.hpp"
#include <cstdlib>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

class GameMap;

enum class NpcState { IDLE, MOVING };
enum class NpcBehavior { WALKER, FISHER };
enum class FisherState { CASTING, WAITING, BITING, SUCCESS };

class Npc {
public:
  Npc(NpcBehavior behavior = NpcBehavior::WALKER);
  ~Npc();

  std::unique_ptr<VAO> vao;
  std::unique_ptr<VBO> vbo;
  std::unique_ptr<EBO> ebo;

  std::map<std::string, std::vector<std::unique_ptr<Texture>>> textures;
  std::unique_ptr<Animations> anim;

  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;

  NpcState state;
  NpcBehavior behavior;
  FisherState fisherState;
  int direction; // 0: Down, 1: Up, 2: Left, 3: Right
  float speed;
  glm::vec3 targetPosition;
  float idleTimer;
  float idleDuration;
  float fishingTimer;

  // A* pathfinding
  std::vector<glm::vec3> pathWaypoints;
  int currentWaypoint = 0;

  void Draw(Shader &shader, Camera &camera);
  void Update(float deltaTime, GameMap &gameMap);
  void PickRandomTarget(GameMap &gameMap);
  void LoadAssets(Shader &shader);
  void getAnimation(std::string animType, int direction);
  nlohmann::json ToJson();
  void FromJson(nlohmann::json j);
  void Reset();

  // A* pathfinding
  void FindPath(GameMap &gameMap);

private:
  glm::ivec2 WorldToTile(GameMap &gameMap, float x, float y);
  glm::vec3 TileToWorld(GameMap &gameMap, int tileX, int tiledRow);
};

// Walker NPC texture paths
inline std::vector<std::string> npcWalkTexturePaths = {
    "image/npc-walk-front.png", "image/npc-walk-back.png",
    "image/npc-walk-left.png", "image/npc-walk-right.png"};

inline std::vector<std::string> npcIdleTexturePaths = {
    "image/npc-down.png", "image/npc-up.png", "image/npc-left.png",
    "image/npc-right.png"};

inline std::map<std::string, std::vector<std::string>> npcTexturePaths = {
    {"walk", npcWalkTexturePaths}, {"idle", npcIdleTexturePaths}};

// Fisher NPC texture path
inline std::vector<std::string> npcFisherTexturePaths = {
    "image/npc-fisher.png"};

inline std::map<std::string, std::vector<std::string>> npcFisherTextureMap = {
    {"fishing", npcFisherTexturePaths}};

#endif