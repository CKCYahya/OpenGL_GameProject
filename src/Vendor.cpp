#include "Vendor.h"
#include "GLFW/glfw3.h"

Vendor::Vendor() {}
Vendor::~Vendor() {}

void Vendor::Update(GLFWwindow *window, int direction, int currentFrame,
                    glm::vec3 playerPos, glm::vec3 endWorld) {}

void Vendor::Sell(Player &player) {
  std::cout << "Enter sell zone" << std::endl;
  for (auto &item : player.slots) {
    if (item.count > 0) {
      player.money += (item.itemValue * item.count);
      item.count = 0;
      item.itemID = -1;
      item.itemName = "UNKNOWN";
      item.atlasID = 0;
      item.itemValue = 0;
      item.atlasIndex = -1;
      item.uOffset = 0.0f;
      item.vOffset = 0.0f;
      item.uv0 = ImVec2(0.0f, 0.0f);
      item.uv1 = ImVec2(0.0f, 0.0f);
    }
  }
}

void Vendor::Upgrade(Player &player) {}
