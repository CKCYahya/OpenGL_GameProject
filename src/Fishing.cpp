#include "../Header/Fishing.h"
#include <iostream>

void Fishing::fishing(Player &player,
                   std::map<int, std::unique_ptr<Items>> &itemList) {

        if (player.slots[player.selectedSlot].itemID == 0 && player.state == State::IDLE) {
            std::cout << "Fishing action triggered!" << std::endl;
            // Here you would implement the actual fishing logic, such as:
            // - Check if player is near water
            // - Start a fishing mini-game or timer
            // - Determine if a fish is caught and what type
            // - Add the caught fish to the player's inventory
        } else {
            std::cout << "You need a fishing rod to fish!" << std::endl;
        }
    }

bool Fishing::isPlayerNearWater(const Player &player, const NewMap &gameMap) {
    // This function checks if the player is near water tiles in the game map.
    // You would implement this by checking the player's position against the
    // waterCache in the GameMap to see if any water tiles are within a certain
    // radius of the player.
    // Check if there are any water tiles at certain distances from the player and its direction
    
return false; // Placeholder return value, replace with actual logic
}