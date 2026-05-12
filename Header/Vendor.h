#ifndef VENDOR_H
#define VENDOR_H

#include "Camera.h"
#include "Items.h"
#include "Player.h"
#include "Shader.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
enum class UpgradeLevel { LVL1 = 1, LVL2 = 2, LVL3 = 3, MAX = 4 };

class Vendor {
public:
  static bool hasUpdated;
  Vendor();
  ~Vendor();
  static UpgradeLevel tradeLevel;
  static UpgradeLevel fishingLevel;
  static UpgradeLevel bootLevel;
  void Update(GLFWwindow *window, int direction, int currentFrame,
              glm::vec3 playerPos, glm::vec3 endWorld);
  static void Sell(Player &player);
  static void UpgradeTrade(Player &player);
  static void UpgradeFishing(Player &player);
  static void UpgradeBoots(Player &player);
  static json ToJson();
  static void FromJson(json &j);

private:
  glm::vec3 startPos;
  glm::vec3 endPos;
  bool hasReleasedKey = false;
};

#endif