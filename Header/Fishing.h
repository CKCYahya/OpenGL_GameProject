#ifndef FISHING_H
#define FISHING_H

#include "Items.h"
#include "Player.h"
#include "GameMap.h"
#include <map>
#include <memory>

enum class States {
  IDLE,
  FISHING,
  REELING,
  CAUGHT
};

class Fishing {
public:
  void fishing(Player &player, std::map<int, std::unique_ptr<Items>> &itemList, GameMap &gameMap);
};

#endif // FISHING_H

