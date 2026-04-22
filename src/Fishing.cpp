#include "../Header/Fishing.h"
#include "../Header/GameMap.h"
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
                     GameMap &gameMap) {
  // --- Check availability ---
  if (currentState == States::NOT_AVAILABLE ||
      currentState == States::AVAILABLE) {
    waterType = gameMap.checkWater(player.newRayEnd.x, player.newRayEnd.y);
    if (waterType == 2014 || waterType == 2015 || waterType == 2016) {
      currentState = States::AVAILABLE;
      if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isFishing == false) {
        // Start fishing - begin cast animation
        currentState = States::CASTING;
        player.state = State::FISHING;
        player.fishAnim->SetRange(0, 2, false); // Frames 0-2, one-shot
        timer = 2.0f + static_cast<float>(rand()) /
                           (static_cast<float>(RAND_MAX / (5.0f - 2.0f)));
        isFishing = true;
      } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE &&
                 isFishing == true) {
        isFishing = false;
      }
    } else {
      currentState = States::NOT_AVAILABLE;
      isFishing = false;
      if (player.state == State::FISHING)
        player.state = State::IDLE;
    }
  }

  // --- Casting state ---
  if (currentState == States::CASTING) {
    // Cancel if player moves
    if (player.state == State::MOVING) {
      std::cout << "You moved! Fishing cancelled." << std::endl;
      currentState = States::NOT_AVAILABLE;
      isFishing = false;
      return;
    }

    player.fishAnim->Update(dt);

    // Cast animation finished -> transition to WAITING
    if (player.fishAnim->finished) {
      currentState = States::WAITING;
      player.fishAnim->SetRange(3, 5, true); // Frames 3-5, loop
    }
  }

  // --- Waiting state ---
  if (currentState == States::WAITING) {
    // Cancel if player moves
    if (player.state == State::MOVING) {
      std::cout << "You moved! Fishing cancelled." << std::endl;
      currentState = States::NOT_AVAILABLE;
      isFishing = false;
      return;
    }

    player.fishAnim->Update(dt);
    timer -= dt;

    if (timer <= 0.0f) {
      std::cout << "Fish is biting! PRESS FISH BUTTON NOW!" << std::endl;
      currentState = States::CAUGHT;
      player.fishAnim->SetRange(6, 6, false); // Frame 6, one-shot
      timer = 1.5f; // Player has 1.5 seconds to react
    }
  }

  // --- Caught state ---
  else if (currentState == States::CAUGHT) {
    player.fishAnim->Update(dt);
    timer -= dt;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      Catch(player, itemList, gameMap);
      currentState = States::NOT_AVAILABLE;
      player.state = State::IDLE;
    } else if (timer <= 0.0f) {
      std::cout << "The fish got away..." << std::endl;
      currentState = States::NOT_AVAILABLE;
      player.state = State::IDLE;
    }
    isFishing = true;
  }
}
