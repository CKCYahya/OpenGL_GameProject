#include "../Header/Fishing.h"
#include <iostream>

void Fishing::Catch(Player &player,
                   std::map<int, std::unique_ptr<Items>> &itemList,
                   GameMap &gameMap) {
    auto it = fishingLootTable.find(waterType);
    
    if (it == fishingLootTable.end()) {
        return; 
    }

    int chances = rand() % 100;
    
    for (const auto& loot : it->second) {
        if (chances < loot.maxChance) {
            Items::AddItem(player, loot.itemID, loot.itemName);
            break;
        }
    }

} 



void Fishing::Update(GLFWwindow* window, float dt, Player &player, std::map<int, std::unique_ptr<Items>> &itemList, GameMap &gameMap) {
  if (currentState == States::NOT_AVAILABLE || currentState == States::AVAILABLE) {
    waterType = gameMap.checkWater(player.newRayEnd.x, player.newRayEnd.y);
    if(waterType == 2014 || waterType == 2015 || waterType == 2016){
        currentState = States::AVAILABLE;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isFishing == false) {
            currentState = States::FISHING;
            timer = 2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(5.0f - 2.0f)));
            isFishing = true;
        } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE &&
                    isFishing == true) {
            isFishing = false;
        }
    }
    else{
        currentState = States::NOT_AVAILABLE;
        isFishing = false;
    }
  }
  

  if (currentState == States::FISHING) {
    if(player.state == State::MOVING){
        std::cout << "You moved! Fishing cancelled." << std::endl;
        currentState = States::NOT_AVAILABLE;
        isFishing = false;
    }
    timer -= dt;
    if (timer <= 0.0f) {
      std::cout << "Fish is biting! PRESS FISH BUTTON NOW!" << std::endl;
      currentState = States::CAUGHT;
      timer = 1.5f; // Player has 1.5 seconds to react
    }
  } 
  else if (currentState == States::CAUGHT) {
    timer -= dt;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        Catch(player, itemList, gameMap);
        currentState = States::NOT_AVAILABLE;
    }
    else if (timer <= 0.0f) {
      std::cout << "The fish got away..." << std::endl;
      currentState = States::NOT_AVAILABLE;
    }
    isFishing = true;
  }
  return;

}


