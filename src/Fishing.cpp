#include "../Header/Fishing.h"
#include "../Header/GameMap.h"
#include "Player.h"
#include <iostream>

void Fishing::Catch(Player &player,
                    std::map<int, std::unique_ptr<Items>> &itemList,
                    GameMap &gameMap) {
  auto it = fishingLootTable.find(waterType);

  if (it == fishingLootTable.end()) {
    return;
  }

  int chances = rand() % 100;

  for (const auto &loot : it->second) {
    if (chances < loot.maxChance) {
      Items::AddItem(player, loot.itemID, loot.itemName);
      break;
    }
  }
}

void Fishing::Update(GLFWwindow *window, float dt, Player &player,
                     std::map<int, std::unique_ptr<Items>> &itemList,
                     GameMap &gameMap, Vendor &vendor) {
  // Update the lock state
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
    hasReleasedKey = true;
  }

  bool isFDown = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
  bool canAction = (isFDown && hasReleasedKey);

  // --- Check availability ---
  if (currentState == States::NOT_AVAILABLE ||
      currentState == States::AVAILABLE) {
    waterType = gameMap.checkWater(player.newRayEnd.x, player.newRayEnd.y);
    if (waterType == 2014 || waterType == 2015 || waterType == 2016) {
      currentState = States::AVAILABLE;
      if (canAction && player.state == State::IDLE) {
        // Start fishing
        hasReleasedKey = false; // Lock it!
        currentState = States::CASTING;
        player.state = State::FISHING;
        player.fishAnim->SetRange(0, 2, false); // Frames 0-2, one-shot
        float levelFactor = static_cast<float>(Vendor::fishingLevel);
        float minWait = 4.0f / levelFactor;
        float maxWait = 7.0f / levelFactor;

        timer =
            minWait + static_cast<float>(rand()) /
                          (static_cast<float>(RAND_MAX / (maxWait - minWait)));
      }
    } else {
      currentState = States::NOT_AVAILABLE;
      if (player.state == State::FISHING)
        player.state = State::IDLE;
    }
  }

  // --- Casting state ---
  else if (currentState == States::CASTING) {
    if (isMoving(player))
      return;

    if (canAction) {
      std::cout << "Fishing cancelled by user." << std::endl;
      hasReleasedKey = false; // Lock to prevent immediate restart
      player.state = State::IDLE;
      currentState = States::NOT_AVAILABLE;
    } else {
      player.fishAnim->Update(dt);

      // Cast animation finished -> transition to WAITING
      if (player.fishAnim->finished) {
        currentState = States::WAITING;
        player.fishAnim->SetRange(3, 5, true); // Frames 3-5, loop
      }
    }
  }

  // --- Waiting state ---
  else if (currentState == States::WAITING) {
    if (isMoving(player))
      return;

    if (canAction) {
      std::cout << "Fishing cancelled by user." << std::endl;
      hasReleasedKey = false;
      player.state = State::IDLE;
      currentState = States::NOT_AVAILABLE;
    } else {
      player.fishAnim->Update(dt);
      timer -= dt;

      if (timer <= 0.0f) {
        std::cout << "Fish is biting! PRESS FISH BUTTON NOW!" << std::endl;
        currentState = States::CAUGHT;
        player.fishAnim->SetRange(6, 6, false); // Frame 6, one-shot
        timer = 1.5f; // Player has 1.5 seconds to react
      }
    }
  }

  // --- Caught state ---
  else if (currentState == States::CAUGHT) {
    player.fishAnim->Update(dt);
    timer -= dt;

    if (canAction) {
      Catch(player, itemList, gameMap);
      hasReleasedKey = false;
      currentState = States::NOT_AVAILABLE;
      player.state = State::IDLE;
    } else if (timer <= 0.0f) {
      std::cout << "The fish got away..." << std::endl;
      currentState = States::NOT_AVAILABLE;
      player.state = State::IDLE;
    }
  }

  isFishing = (player.state == State::FISHING);
}

bool Fishing::isMoving(Player &player) {
  if (player.state == State::MOVING) {
    std::cout << "You moved! Fishing cancelled." << std::endl;
    currentState = States::NOT_AVAILABLE;
    return true;
  }
  return false;
}
