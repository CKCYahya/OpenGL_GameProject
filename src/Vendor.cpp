#include "Vendor.h"
#include "Fishing.h"
#include "GLFW/glfw3.h"

UpgradeLevel Vendor::tradeLevel = UpgradeLevel::LVL1;
UpgradeLevel Vendor::fishingLevel = UpgradeLevel::LVL1;
UpgradeLevel Vendor::bootLevel = UpgradeLevel::LVL1;
bool Vendor::hasUpdated = false;

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

void Vendor::UpgradeTrade(Player &player) {
  if (tradeLevel == UpgradeLevel::LVL1 && player.money >= 1000) {
    tradeLevel = UpgradeLevel::LVL2;
    player.money -= 1000;
    hasUpdated = true;
  } else if (tradeLevel == UpgradeLevel::LVL2 && player.money >= 2000) {
    tradeLevel = UpgradeLevel::LVL3;
    player.money -= 2000;
    hasUpdated = true;
  } else if (tradeLevel == UpgradeLevel::LVL3 && player.money >= 3000) {
    tradeLevel = UpgradeLevel::MAX;
    player.money -= 3000;
    hasUpdated = true;
  } else if (tradeLevel == UpgradeLevel::MAX) {
    std::cout << "Max trade level reached!" << std::endl;
  } else {
    std::cout << "Not enough money" << std::endl;
  }
}
void Vendor::UpgradeFishing(Player &player) {
  if (fishingLevel == UpgradeLevel::LVL1 && player.money >= 1000) {
    fishingLevel = UpgradeLevel::LVL2;
    player.money -= 1000;
  } else if (fishingLevel == UpgradeLevel::LVL2 && player.money >= 2000) {
    fishingLevel = UpgradeLevel::LVL3;
    player.money -= 2000;
  } else if (fishingLevel == UpgradeLevel::LVL3 && player.money >= 3000) {
    fishingLevel = UpgradeLevel::MAX;
    player.money -= 3000;
  } else if (fishingLevel == UpgradeLevel::MAX) {
    std::cout << "Max fishing level reached!" << std::endl;
  } else {
    std::cout << "Not enough money" << std::endl;
  }
}

void Vendor::UpgradeBoots(Player &player) {
  if (bootLevel == UpgradeLevel::LVL1 && player.money >= 1000) {
    bootLevel = UpgradeLevel::LVL2;
    player.money -= 1000;
    player.speed += 50;
  } else if (bootLevel == UpgradeLevel::LVL2 && player.money >= 2000) {
    bootLevel = UpgradeLevel::LVL3;
    player.money -= 2000;
    player.speed += 50;
  } else if (bootLevel == UpgradeLevel::LVL3 && player.money >= 3000) {
    bootLevel = UpgradeLevel::MAX;
    player.money -= 3000;
    player.speed += 50;
  } else if (player.speed == 300.0f) {
    std::cout << "Max speed reached!" << std::endl;
  } else {
    std::cout << "Not enough money" << std::endl;
  }
}

void Vendor::ToJson(json &j) {
  j["tradeLevel"] = tradeLevel;
  j["fishingLevel"] = fishingLevel;
  j["bootLevel"] = bootLevel;
}

void Vendor::FromJson(json &j) {
  tradeLevel = j["tradeLevel"].get<UpgradeLevel>();
  fishingLevel = j["fishingLevel"].get<UpgradeLevel>();
  bootLevel = j["bootLevel"].get<UpgradeLevel>();
}