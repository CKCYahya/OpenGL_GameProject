#ifndef ITEMS_H
#define ITEMS_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <map>
#include <memory>
#include <string>

#include "nlohmann/json.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Camera.h"

using json = nlohmann::json;
class Panel;
class Player;
class GameMap;
class Texture;
class Shader;
class Window;
class VAO;
class VBO;
class EBO;

class Items {
public:
  int ID;
  int atlasIndex;
  std::shared_ptr<Texture> itemTexture;
  std::unique_ptr<VAO> vao;
  std::unique_ptr<VBO> vbo;
  std::unique_ptr<EBO> ebo;
  std::string name;
  int slotIndex;
  int value;
  glm::vec3 position;
  bool isActive = true;
  static std::vector<glm::vec2> atlasWH;
  static std::vector<std::shared_ptr<Texture>> loadedAtlases;
  float uOffset, vOffset;
  Items(const std::string &name, const glm::vec3 &position, int itemID);
  ~Items();

  void Load(Shader &shader, const char *texturePath);
  void Draw(Shader &shader, Player &player);
  void updateItemPosition(glm::vec3 position);
  bool isItemInRange(const Player &player);
  static std::map<int, std::unique_ptr<Items>>
  readJsonItems(const char *jsonItems);
  static void loadAtlas(Shader &shader);
  static void drawAtlas(Shader &shader,
                        std::map<int, std::unique_ptr<Items>> &itemList,
                        int winWidth, int winHeight, Camera &camera);
  static Items *searchItems(std::map<int, std::unique_ptr<Items>> &itemList,
                            int slot);
  static glm::vec2 CalculateUV(int itemID, int atlasIndex);
  static int GetAtlasIndex(int itemID);
  static void AddItem(Player &player, int itemID, std::string itemName);
};

extern std::vector<std::string> images;
#endif // ITEMS_H