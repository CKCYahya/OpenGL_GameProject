#ifndef FISHING_H
#define FISHING_H

#include "Items.h"
#include "Player.h"
#include "GameMap.h"
#include <map>
#include <memory>
#include <GLFW/glfw3.h>
#include <unordered_map>

struct FishLoot {
    int maxChance;
    int itemID;
    std::string itemName;
};

static const std::unordered_map<int, std::vector<FishLoot>> fishingLootTable = {
    {2014, { {25, 86, "palamut"}, {100, 87, "levrek"} }},
    {2015, { {50, 88, "istavrit"}, {100, 89, "uskumru"} }},
    {2016, { {35, 90, "lufer"},    {100, 87, "levrek"} }}
};

enum class States {
  AVAILABLE,
  NOT_AVAILABLE,
  FISHING,
  REELING,
  CAUGHT
};

class Fishing {
public:
  int waterType = -1;
  bool isFishing = false;
  States currentState = States::NOT_AVAILABLE;
  float timer = 0.0f;
  void Catch(Player &player, std::map<int, std::unique_ptr<Items>> &itemList, GameMap &gameMap);
  void Update(GLFWwindow* window,float dt, Player &player, std::map<int, std::unique_ptr<Items>> &itemList, GameMap &gameMap);
  
};

#endif // FISHING_H

