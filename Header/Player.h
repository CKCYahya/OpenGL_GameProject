#ifndef PLAYER_H
#define PLAYER_H

#include "Animations.h"
#include "Items.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Panel;
class GameMap;
class Items;
class Texture;
class Shader;
class VAO;
class VBO;
class EBO;
class Animations;

struct InventorySlot {
  int itemID = -1;
  std::string itemName = "UNKNOWN";
  GLuint atlasID = 0;
  int atlasIndex = -1;
  float uOffset = 0.0f;
  float vOffset = 0.0f;
  ImVec2 uv0;
  ImVec2 uv1;
  int count = 0;
};
enum class State { IDLE, MOVING, FISHING };

class Player {

public:
  // State
  State state = State::IDLE;
  glm::vec3 Position;
  float size;
  float speed;
  int direction;           // 0: Down, 1: Up, 2: Left, 3: Right
  glm::mat4 playerModel;   // Model matrix for the player
  float interactionRadius; // Interaction radius
  InventorySlot slots[5];  // 5 adet envanter slotu
  int slotAmount;
  int selectedSlot;
  float ray; // For interaction raycasting
  Animations *walkAnim;
  Animations *fishAnim;
  glm::vec3 handposition;
  glm::vec4 rayStart;
  glm::vec4 rayEnd;
  glm::vec3 rayDirection;
  glm::vec3 newRayStart;
  glm::vec3 newRayEnd;
  // Graphics
  std::map<std::string, std::vector<std::unique_ptr<Texture>>> textures;

  std::unique_ptr<VAO> vao;
  std::unique_ptr<VBO> vbo;
  std::unique_ptr<EBO> ebo;

  Player(glm::vec3 startPos, float size, float speed);
  ~Player();

  void LoadAssets(Shader &shader);
  void Update(GLFWwindow *window, float dt, GameMap &gameMap);
  void Draw(Shader &shader);
  ImTextureID Interact(Items &item);
  void dropItem(int selectedSlot,
                std::map<int, std::unique_ptr<Items>> &itemList);
  void drawItem(Shader &shader);
  void getAnimation(std::string animType, int direction);
};

// Texture paths ordered by direction: 0=Down, 1=Up, 2=Left, 3=Right
inline std::vector<std::string> fishingTexturePaths = {
    "image/fishing-down.png", "image/fishing-up.png", "image/fishing-left.png",
    "image/fishing-right.png"};

inline std::vector<std::string> walkTexturePaths = {
    "image/walk-down.png", "image/walk-up.png", "image/walk-left.png",
    "image/walk-right.png"};

inline std::vector<std::string> idleTexturePaths = {
    "image/down.png", "image/up.png", "image/left.png", "image/right.png"};

inline std::map<std::string, std::vector<std::string>> texturePaths = {
    {"fishing", fishingTexturePaths},
    {"walk", walkTexturePaths},
    {"idle", idleTexturePaths}};
#endif
