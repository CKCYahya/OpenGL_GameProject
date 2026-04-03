#include "../Header/Panel.h"
#include "../Header/Items.h"
#include "../Header/Player.h"
#include "../Header/Window.h"
#include <iostream>
#include <string>

Panel::Panel() {
  slotSize = 100.0f;
  slotCount = 5;
  bottomMargin = 50.0f;
}

void Panel::Update(Window &window, Player &player,
                   std::map<int, std::unique_ptr<Items>> &itemList) {
  int currentWidth, currentHeight;
  glfwGetWindowSize(window.getGLFWWindow(), &currentWidth, &currentHeight);

  float itemSpace = ImGui::GetStyle().ItemSpacing.x;
  float PANEL_WIDTH = (slotSize * slotCount) + (itemSpace * (slotCount - 1));
  float PANEL_HEIGHT = (slotSize + bottomMargin);

  float startX = (currentWidth - PANEL_WIDTH) * 0.5f;
  float startY = (currentHeight - PANEL_HEIGHT);

  // Pencere ayarları
  ImGui::SetNextWindowPos(ImVec2(startX, startY), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.35f);

  ImGui::Begin("Inventory Panel", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

  for (int i = 0; i < slotCount; i++) {
    ImGui::PushID(i);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    if (player.slots[i].atlasID != 0) {
      ImTextureID texID = (ImTextureID)(intptr_t)player.slots[i].atlasID;
      if (ImGui::ImageButton(std::to_string(i).c_str(), texID,
                             ImVec2(slotSize, slotSize), player.slots[i].uv0,
                             player.slots[i].uv1)) {
        player.selectedSlot = i;
        std::cout << "Slot " << i << " (Esya) tiklandi!" << std::endl;
      }
    } else {
      // Boss ise normal Button
      if (ImGui::Button("##Empty", ImVec2(slotSize, slotSize))) {
        player.selectedSlot = i;
        std::cout << "Slot " << i << " (Bos) tiklandi!" << std::endl;
      }
    }
    ImGui::PopStyleVar();
    ImGui::PopID();

    if (i < slotCount - 1) {
      ImGui::SameLine();
    }
  }

  ImGui::End();
}

Panel::~Panel() {
  // Cleanup resources if needed
}
