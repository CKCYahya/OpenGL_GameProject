#ifndef FISHING_H
#define FISHING_H

#include "Items.h"
#include "Player.h"
#include "GameMap.h"
#include <map>
#include <memory>
#include <GLFW/glfw3.h>


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

