// InteractionUI.cpp
#include "InteractionUI.h"
#include "Camera.h"
#include "Player.h"
#include "Window.h"
#include "glm/glm.hpp"
#include "imgui.h"

InteractionUI::InteractionUI(Player &player, Camera &camera) {
  state = PopupState::HIDE;
  lastTime = 0.0f;
}

InteractionUI::~InteractionUI() {}

void InteractionUI::showInteractionUI(Player &player) {
  if ((player.state == State::INTERACTING ||
       player.state == State::FULL_INVENTORY ||
       player.state == State::FISHING)) {

    if (state == PopupState::HIDE) {
      lastTime = glfwGetTime();
    }

    if (player.state == State::INTERACTING) {
      strcpy_s(message, "E: Pick up");
    } else if (player.state == State::FULL_INVENTORY) {
      strcpy_s(message, "Inventory is full");
    } else if (player.state == State::FISHING) {
      strcpy_s(message, "F: Start fishing");
    }
    state = PopupState::SHOW;
  } else {
    state = PopupState::HIDE;
  }
}

void InteractionUI::Draw(Camera *camera, Player *player, Window *window) {
  if (state == PopupState::HIDE)
    return;

  float currentTime = glfwGetTime();
  if (currentTime - lastTime >= 0.5f) {

    float viewX =
        (player->Position.x + player->size / 2.0f) - camera->Position.x;
    float viewY = (player->Position.y + player->size) - camera->Position.y;

    float screenX = viewX / camera->zoom;
    float screenY = (float)camera->height - (viewY / camera->zoom);

    screenY -= 20.0f;

    ImVec2 boxPosition = ImVec2(screenX, screenY);

    ImGui::SetNextWindowPos(boxPosition, ImGuiCond_Always, ImVec2(0.5f, 1.0f));
    ImGui::SetNextWindowSize(ImVec2(0, 0));

    ImGui::Begin("ItemInteraction", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::Text(message);
    ImGui::PopStyleColor();

    ImGui::End();
  }
}
