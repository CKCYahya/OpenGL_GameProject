// InteractionUI.cpp
#include "InteractionUI.h"
#include "Camera.h"
#include "Menu.h"
#include "Player.h"
#include "Window.h"
#include "glm/glm.hpp"
#include "imgui.h"

InteractionUI::InteractionUI() {
  state = PopupState::HIDE;
  lastTime = 0.0f;
}

InteractionUI::~InteractionUI() {}

void InteractionUI::showInteractionUI(Fishing &fishing, Player &player,
                                      bool &nearVendor, bool nearItem) {
  if (nearItem || player.state == State::FULL_INVENTORY ||
      fishing.currentState == States::AVAILABLE ||
      fishing.currentState == States::CAUGHT || nearVendor) {

    if (state == PopupState::HIDE) {
      lastTime = glfwGetTime();
    }

    if (player.state == State::FULL_INVENTORY) {
      strcpy_s(message, "Inventory is full");
    } else if (fishing.currentState == States::AVAILABLE) {
      strcpy_s(message, "F: Start fishing");
    } else if (fishing.currentState == States::CAUGHT) {
      strcpy_s(message, "F: Catch the fish");
    } else if (nearItem) {
      strcpy_s(message, "E: Pick up");
    } else if (nearVendor) {
      strcpy_s(message, "E: Trade");
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
  float elapsed = currentTime - lastTime;
  if (elapsed < 0.5f)
    return;

  // Fade-in alpha (0 -> 1 over 0.3 seconds after the 0.5s delay)
  float fadeAlpha = (elapsed - 0.5f) / 0.3f;
  if (fadeAlpha > 1.0f)
    fadeAlpha = 1.0f;

  // World-to-screen conversion
  float viewX = (player->Position.x + player->size / 2.0f) - camera->Position.x;
  float viewY = (player->Position.y + player->size) - camera->Position.y;

  float screenX = viewX / camera->zoom;
  float screenY = (float)camera->height - (viewY / camera->zoom);
  screenY -= 30.0f;

  ImVec2 boxPosition = ImVec2(screenX, screenY);

  // Style: rounded dark semi-transparent background
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 8.0f));
  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        ImVec4(0.0f, 0.0f, 0.0f, 0.6f * fadeAlpha));
  ImGui::PushStyleColor(ImGuiCol_Border,
                        ImVec4(1.0f, 1.0f, 1.0f, 0.15f * fadeAlpha));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

  ImGui::SetNextWindowPos(boxPosition, ImGuiCond_Always, ImVec2(0.5f, 1.0f));
  ImGui::SetNextWindowSize(ImVec2(0, 0));

  ImGui::Begin("ItemInteraction", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoSavedSettings);

  ImGui::SetWindowFontScale(1.3f);

  // Text shadow
  ImVec2 cursorPos = ImGui::GetCursorPos();
  ImGui::SetCursorPos(ImVec2(cursorPos.x + 1.0f, cursorPos.y + 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Text,
                        ImVec4(0.0f, 0.0f, 0.0f, 0.5f * fadeAlpha));
  ImGui::Text("%s", message);
  ImGui::PopStyleColor();

  // Actual text
  ImGui::SetCursorPos(cursorPos);
  ImGui::PushStyleColor(ImGuiCol_Text,
                        ImVec4(1.0f, 1.0f, 1.0f, 1.0f * fadeAlpha));
  ImGui::Text("%s", message);
  ImGui::PopStyleColor();

  ImGui::SetWindowFontScale(1.0f);
  ImGui::End();

  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(2);
}
