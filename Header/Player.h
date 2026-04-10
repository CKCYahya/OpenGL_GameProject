#ifndef PLAYER_H
#define PLAYER_H

#include "../libs/glad/include/glad/glad.h"
#include "../libs/glfw/include/glfw/glfw3.h"
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"
#include "../libs/imgui/imgui.h"
#include "Items.h"
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

struct InventorySlot {
  int itemID = -1;    // -1 ise boş kabul edilir
  GLuint atlasID = 0; // Eşyanın görseli
  ImVec2 uv0;
  ImVec2 uv1;
  int count = 0; // Eşya adedi
};
enum class State { IDLE, MOVING, INTERACTING };

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
  glm::vec4 rayStart;
  glm::vec4 rayEnd;
  glm::vec3 rayDirection;
  glm::vec3 newRayStart;
  glm::vec3 newRayEnd;
  // Graphics
  std::unique_ptr<Texture> texUp;
  std::unique_ptr<Texture> texDown;
  std::unique_ptr<Texture> texLeft;
  std::unique_ptr<Texture> texRight;

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
};

#endif
