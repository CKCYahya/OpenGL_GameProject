#include "../Header/Fishing.h"
#include <iostream>

void Fishing::fishing(Player &player,
                   std::map<int, std::unique_ptr<Items>> &itemList,
                   GameMap &gameMap) {
    int waterType = gameMap.checkWater(player.rayStart.x, player.rayStart.y);

    if (player.slots[player.selectedSlot].itemID == 0 && player.state == State::IDLE) {
        std::cout << "Fishing action triggered!" << std::endl;
        if(waterType == 2014){
            
        } else if(waterType == 2015){
                
            } else if(waterType == 2016){
                
            }
        } 
    else {
        std::cout << "You need a fishing rod and stand still to fish!" << std::endl;
    }
}

