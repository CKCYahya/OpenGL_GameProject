#include "Menu.h"
#include "Camera.h"
#include "Items.h"
#include "Npc.h"
#include "Player.h"
#include "Texture.h"
#include "imgui.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>


using json = nlohmann::json;

Menu::Menu(Window *window, Shader *shader) {
  state = MenuState::MAIN_MENU;
  saveSlotCount = 3;
  GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f,
                        0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                        1.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};
  isGameStarted = false;

  vao = std::make_unique<VAO>();
  vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
  ebo = std::make_unique<EBO>(indices, sizeof(indices));
  vao->Bind();
  vbo->Bind();
  ebo->Bind();
  vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void *)0);
  vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat),
                  (void *)(3 * sizeof(GLfloat)));
  vao->Unbind();
  vbo->Unbind();
  ebo->Unbind();
}

Menu::~Menu() { menuTexture->Delete(); }

void Menu::LoadAssets(Shader *shader) {
  menuTexture = std::make_unique<Texture>(
      "image/menu.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
  menuTexture->texUnit(*shader, "tex0", 0);
  purseTexture = std::make_unique<Texture>(
      "image/Purse.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
}

void Menu::Draw(Window *window, Shader *shader, Player *player, Camera *camera,
                std::map<int, std::unique_ptr<Items>> *itemList, Npc *npc,
                Vendor *vendor) {
  shader->Activate();
  glUniform2f(glGetUniformLocation(shader->ID, "texScale"), 1.0f, 1.0f);
  glUniform2f(glGetUniformLocation(shader->ID, "texOffset"), 0.0f, 0.0f);

  glm::mat4 identity = glm::mat4(1.0f);
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "camMatrix"), 1, GL_FALSE,
                     glm::value_ptr(identity));
  glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(identity));
  if (state != MenuState::START && state != MenuState::VENDOR) {
    menuTexture->Bind();
    vao->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  } else {
    moneyDisplay(*player);
  }

  int currentWidth;
  int currentHeight;
  glfwGetWindowSize(window->getGLFWWindow(), &currentWidth, &currentHeight);

  float itemWidth = currentWidth / 7.0f;
  float itemHeight = currentHeight / 14.0f;
  float spacingY = 15.0f;

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.36f, 0.20f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        ImVec4(0.65f, 0.46f, 0.30f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                        ImVec4(0.40f, 0.24f, 0.12f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.94f, 0.83f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.14f, 0.05f, 1.0f));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, spacingY));

  ImGuiWindowFlags clearWindowFlags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

  // MAIN MENU
  if (state == MenuState::MAIN_MENU && !isGameStarted) {
    previousState = state;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
    ImGui::Begin("Menu", nullptr, clearWindowFlags);
    ImGui::SetWindowFontScale(1.8f);

    int buttonCount = 4;
    float totalHeight =
        (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
    ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));

    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Start", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::START;
      isGameStarted = true;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Load", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::LOAD;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Settings", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::SETTINGS;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Exit", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::EXIT;
    }
    ImGui::End();
  }

  // SETTINGS
  if (state == MenuState::SETTINGS) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
    ImGui::Begin("Settings", nullptr, clearWindowFlags);
    ImGui::SetWindowFontScale(1.8f);

    int buttonCount = 1;
    float totalHeight =
        (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
    ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));

    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    ImGui::SetNextItemWidth(itemWidth);
    std::string previewValue = std::to_string(resolutions[selected_res].width) +
                               "x" +
                               std::to_string(resolutions[selected_res].height);
    if (ImGui::BeginCombo("Resolution", previewValue.c_str())) {
      for (int i = 0; i < 4; i++) {
        bool is_selected = (selected_res == i);
        std::string itemValue = std::to_string(resolutions[i].width) + "x" +
                                std::to_string(resolutions[i].height);
        if (ImGui::Selectable(itemValue.c_str(), is_selected,
                              ImGuiSelectableFlags_None,
                              ImVec2(itemWidth, itemHeight))) {
          window->setResolution(resolutions[i]);
          selected_res = i;
        }
      }
      ImGui::EndCombo();
    }

    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
      state = (previousState == MenuState::PAUSE) ? MenuState::PAUSE
                                                  : MenuState::MAIN_MENU;
    }
    ImGui::End();
  }
  // SAVE AND LOAD
  if (state == MenuState::SAVE || state == MenuState::LOAD) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
    if (showSaveConfirm) {
      newSaveSection(player, camera, itemList, npc, vendor);
    } else {
      ImGui::Begin("SaveLoad", nullptr, clearWindowFlags);
      ImGui::SetWindowFontScale(1.8f);
      int buttonCount = saveSlotCount + 1;
      float totalHeight =
          (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
      ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));
      GetSaveNames();
      for (int i = 0; i < saveNames.size(); i++) {
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if (ImGui::Button(saveNames[i].c_str(),
                          ImVec2(itemWidth, itemHeight))) {
          if (state == MenuState::SAVE) {
            if (saveNames[i] == "New Save") {
              showSaveConfirm = true;
            } else {
              SaveGame(player, camera, itemList, npc, vendor,
                       std::string(saveNames[i]) + ".json");
              state = MenuState::PAUSE;
            }
          } else {
            ImGui::Text("Loading...");
            if (saveNames[i] == "Slot " + std::to_string(i + 1)) {
              ImGui::Text("Empty Slot");
            } else {
              if (saveNames[i] != "New Save") {
                LoadGame(player, camera, itemList, npc, vendor,
                         std::string(saveNames[i]) + ".json");
                ImGui::Text("Loaded!");
                state = MenuState::START;
                isGameStarted = true;
              }
            }
          }
        }
      }
      if (saveNames.empty() && state == MenuState::LOAD) {
        ImGui::Text("No Save Files");
      } else if (saveNames.empty() && state == MenuState::SAVE) {
        ImGui::Text("No Save Files");
      }
      // Back
      ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
      if (ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
        state = (previousState == MenuState::PAUSE) ? MenuState::PAUSE
                                                    : MenuState::MAIN_MENU;
      }
    }
    ImGui::End();
  }

  // PAUSE
  if (state == MenuState::PAUSE && isGameStarted) {
    previousState = state;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
    ImGui::Begin("Pause", nullptr, clearWindowFlags);
    ImGui::SetWindowFontScale(1.8f);

    int buttonCount = 6;
    float totalHeight =
        (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
    ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));

    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Continue", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::START;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Save", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::SAVE;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Load", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::LOAD;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Settings", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::SETTINGS;
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Back To Main Menu", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::MAIN_MENU;
      isGameStarted = false;
      player->Reset();
      camera->Reset();
      Items::Reset(*itemList);
      *itemList = Items::readJsonItems("newmap.json");
    }
    ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
    if (ImGui::Button("Exit", ImVec2(itemWidth, itemHeight))) {
      state = MenuState::EXIT;
    }
    ImGui::End();
  }

  if (state == MenuState::EXIT) {
    glfwSetWindowShouldClose(window->getGLFWWindow(), GLFW_TRUE);
  }

  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(5);
}

// SAVE GAME
void Menu::SaveGame(Player *player, Camera *camera,
                    std::map<int, std::unique_ptr<Items>> *itemList, Npc *npc,
                    Vendor *vendor, std::string filename) {
  json jsonfile;
  if (player == nullptr || camera == nullptr || itemList == nullptr) {
    std::cout << "Error: Player, camera, or item list is null!" << std::endl;
    return;
  }

  jsonfile["player"] = player->ToJson();
  jsonfile["camera"] = camera->ToJson();
  jsonfile["itemList"] = Items::ToJson(*itemList);
  jsonfile["vendor"] = vendor->ToJson();
  if (npc != nullptr) {
    jsonfile["npc"] = npc->ToJson();
  }
  std::ofstream ofs("saves/" + filename);
  ofs << jsonfile.dump(4, ' ', true);
  ofs.close();
}

// LOAD GAME
void Menu::LoadGame(Player *player, Camera *camera,
                    std::map<int, std::unique_ptr<Items>> *itemList, Npc *npc,
                    Vendor *vendor, std::string filename) {
  json jsonfile;
  std::ifstream ifs("saves/" + filename);
  if (!ifs.is_open()) {
    std::cerr << "Error: Could not open save file." << std::endl;
    return;
  }

  try {
    ifs >> jsonfile;
    ifs.close();
    player->FromJson(jsonfile["player"]);
    camera->FromJson(jsonfile["camera"]);
    Items::FromJson(*itemList, jsonfile["itemList"]);
    vendor->FromJson(jsonfile["vendor"]);
    if (npc != nullptr && jsonfile.contains("npc")) {
      npc->FromJson(jsonfile["npc"]);
    }
  } catch (json::parse_error &e) {
    std::cerr << "JSON Load Error: " << e.what() << std::endl;
  }
}

void Menu::GetSaveNames() {
  saveNames.clear();
  std::string path = "saves/";

  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directory(path);
  }
  saveSlotCount = 0;
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.path().extension() == ".json") {
      saveNames.push_back(entry.path().stem().string());
      saveSlotCount++;
    }
  }
  if (saveNames.size() < 3) {
    saveNames.push_back("New Save");
  }
}

void Menu::newSaveSection(Player *player, Camera *camera,
                          std::map<int, std::unique_ptr<Items>> *itemList,
                          Npc *npc, Vendor *vendor) {
  ImGui::Begin("Save Confirm");
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15.0f));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.36f, 0.20f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        ImVec4(0.65f, 0.46f, 0.30f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                        ImVec4(0.40f, 0.24f, 0.12f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.94f, 0.83f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.14f, 0.05f, 1.0f));
  ImGui::Text("Please enter a save name");
  ImGui::InputText("Save Name", saveName, 100);
  ImGui::Spacing();

  if (ImGui::Button("Save")) {
    if (saveName[0] != '\0') {
      SaveGame(player, camera, itemList, npc, vendor,
               std::string(saveName) + ".json");
      showSaveConfirm = false;
      state = MenuState::SAVE;
      saveName[0] = '\0';
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel")) {
    showSaveConfirm = false;
    saveName[0] = '\0';
  }
  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(5);
}

void Menu::vendorMenu(Player &player, float currentWidth, float currentHeight) {
  float popupWidth = currentWidth * 0.3f;
  if (popupWidth < 250.0f)
    popupWidth = 250.0f;

  float itemHeight = 50.0f;
  float spacingY = 15.0f;
  int buttonCount = 5;
  float popupHeight =
      (itemHeight * buttonCount) + (spacingY * (buttonCount + 2)) + 100.0f;

  ImGui::SetNextWindowPos(ImVec2(currentWidth / 2.0f, currentHeight / 2.0f),
                          ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight));

  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.95f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.84f, 0.0f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoScrollbar;

  if (ImGui::Begin("VendorPopup", nullptr, flags)) {
    ImGui::SetCursorPosY(spacingY);
    float titleWidth = ImGui::CalcTextSize("VENDOR").x;
    ImGui::SetCursorPosX((popupWidth - titleWidth) / 2.0f);
    ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "VENDOR");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, spacingY));
    float buttonWidth = popupWidth * 0.8f;
    float startPosX = (popupWidth - buttonWidth) / 2.0f;

    // --- BUTON 1: SELL ---
    ImGui::SetCursorPosX(startPosX);
    if (ImGui::Button("Sell All Fish", ImVec2(buttonWidth, itemHeight))) {
      Vendor::Sell(player);
    }

    // --- BUTON 2: UPGRADE ---
    ImGui::SetCursorPosX(startPosX);
    if (ImGui::Button("Upgrade Trade", ImVec2(buttonWidth, itemHeight))) {
      Vendor::UpgradeTrade(player);
    }

    ImGui::SetCursorPosX(startPosX);
    if (ImGui::Button("Upgrade Fishing", ImVec2(buttonWidth, itemHeight))) {
      Vendor::UpgradeFishing(player);
    }

    ImGui::SetCursorPosX(startPosX);
    if (ImGui::Button("Upgrade Boots", ImVec2(buttonWidth, itemHeight))) {
      Vendor::UpgradeBoots(player);
    }

    // --- BUTON 3: BACK ---
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
    ImGui::SetCursorPosX(startPosX);
    if (ImGui::Button("Back", ImVec2(buttonWidth, itemHeight))) {
      state = MenuState::START;
    }
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::End();
  }
  ImGui::PopStyleVar(3);
  ImGui::PopStyleColor(2);
}

void Menu::moneyDisplay(Player &player) {
  ImGui::SetNextWindowBgAlpha(0.35f);

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
      ImGuiWindowFlags_NoMove;

  if (ImGui::Begin("MoneyHUD", NULL, window_flags)) {
    ImGui::Image((void *)(intptr_t)purseTexture->ID, ImVec2(64, 64),
                 ImVec2(0, 1), ImVec2(1, 0));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "%d", player.money);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::End();
  }
}
