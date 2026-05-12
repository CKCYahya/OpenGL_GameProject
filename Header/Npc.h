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
#include <string>
#include <vector>


class GameMap;

enum class NpcState { IDLE, MOVING };

class Npc {
public:
  Npc();
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
  int direction; // 0: Down, 1: Up, 2: Left, 3: Right
  float speed;
  glm::vec3 targetPosition;
  float idleTimer;
  float idleDuration;

  void Draw(Shader &shader, Camera &camera);
  void Update(float deltaTime, GameMap &gameMap);
  void PickRandomTarget(GameMap &gameMap);
  void LoadAssets(Shader &shader);
  void getAnimation(std::string animType, int direction);
  nlohmann::json ToJson();
  void FromJson(nlohmann::json j);
  void Reset();
};

// Default NPC texture paths (you can adjust these as needed)
inline std::vector<std::string> npcWalkTexturePaths = {
    "image/npc-walk-front.png", "image/npc-walk-back.png",
    "image/npc-walk-left.png", "image/npc-walk-right.png"};

inline std::vector<std::string> npcIdleTexturePaths = {
    "image/npc-down.png", "image/npc-up.png", "image/npc-left.png",
    "image/npc-right.png"};

inline std::map<std::string, std::vector<std::string>> npcTexturePaths = {
    {"walk", npcWalkTexturePaths}, {"idle", npcIdleTexturePaths}};

#endif