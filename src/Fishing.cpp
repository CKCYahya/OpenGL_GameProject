#include "../Header/Fishing.h"
#include <iostream>

void Fishing::Catch(Player &player,
                   std::map<int, std::unique_ptr<Items>> &itemList,
                   GameMap &gameMap) {
    int chances = rand() % 100;
    int itemID = -1;
    if(waterType == 2014){
        if(chances < 25){
            itemID = 86;
        }
        else{
            itemID = 87;
        }
    } else if(waterType == 2015){
        if(chances < 50){
            itemID = 88;
        }
        else{
            itemID = 89;
        }
    } else if(waterType == 2016){
        if(chances < 5){
            itemID = 90;
        }
        else{
            itemID = 87;
        }
    }
    for(auto& item : player.slots){
        if(item.itemID == itemID && item.itemID != -1){
            item.count += 1;
            std::cout << "You caught a " << itemID << "!" << std::endl;
            return;
        }
    }
    for(auto& item : player.slots){
        if(item.itemID == -1){
            item.itemID = itemID;
            item.count = 1;
            std::cout << "You caught a " << itemID << "!" << std::endl;
            return;
        } 
    }
    std::cout << "Inventory is full!" << std::endl;
    return;   

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

