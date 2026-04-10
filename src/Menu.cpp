#include "Menu.h"
#include "Player.h"
#include "Camera.h"
#include "Items.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

Menu::Menu(Window* window, Shader* shader) {
    state = MenuState::MAIN_MENU;
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    isGameStarted = false;

    vao = std::make_unique<VAO>();
    vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
    ebo = std::make_unique<EBO>(indices, sizeof(indices));
    vao->Bind();
    vbo->Bind();
    ebo->Bind();
    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);
    vao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    vao->Unbind();
    vbo->Unbind();
    ebo->Unbind();

}

Menu::~Menu() {
    menuTexture->Delete();

}

void Menu::LoadAssets(Shader* shader) {
    menuTexture = std::make_unique<Texture>("image/menu.png", GL_TEXTURE_2D,
                                      GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    menuTexture->texUnit(*shader, "tex0", 0);
}

void Menu::Draw(Window* window, Shader* shader, Player* player, Camera* camera, std::map<int, std::unique_ptr<Items>>* itemList) {
    shader->Activate();
    glUniform2f(glGetUniformLocation(shader->ID, "texScale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(shader->ID, "texOffset"), 0.0f, 0.0f);

    glm::mat4 identity = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(identity));
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(identity));

    menuTexture->Bind();
    vao->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    int currentWidth;
    int currentHeight;
    glfwGetWindowSize(window->getGLFWWindow(), &currentWidth, &currentHeight);

    float itemWidth = currentWidth / 7.0f;    
    float itemHeight = currentHeight / 14.0f; 
    float spacingY = 15.0f;                   

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.36f, 0.20f, 1.0f));       
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.46f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.40f, 0.24f, 0.12f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.94f, 0.83f, 1.0f));          
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.14f, 0.05f, 1.0f));       
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);   
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 3.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, spacingY)); 

    ImGuiWindowFlags clearWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | 
                                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    if (state == MenuState::MAIN_MENU && !isGameStarted) {
        previousState = state;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
        ImGui::Begin("Menu", nullptr, clearWindowFlags);
        ImGui::SetWindowFontScale(1.8f); 
        
        int buttonCount = 4;
        float totalHeight = (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
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

    if (state == MenuState::SETTINGS) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
        ImGui::Begin("Settings", nullptr, clearWindowFlags);
        ImGui::SetWindowFontScale(1.8f); 
        
        int buttonCount = 1;
        float totalHeight = (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
        ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));

        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        ImGui::SetNextItemWidth(itemWidth);
        std::string previewValue = std::to_string(resolutions[selected_res].width) + "x" + std::to_string(resolutions[selected_res].height);
        if(ImGui::BeginCombo("Resolution", previewValue.c_str())) {
            for(int i = 0; i < 4; i++) {
                bool is_selected = (selected_res == i);
                std::string itemValue = std::to_string(resolutions[i].width) + "x" + std::to_string(resolutions[i].height);
                if(ImGui::Selectable(itemValue.c_str(), is_selected, ImGuiSelectableFlags_None, ImVec2(itemWidth, itemHeight))) {
                    window->setResolution(resolutions[i]);
                    selected_res = i;
                }
            }
            ImGui::EndCombo();

        }

        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
            state = (previousState == MenuState::PAUSE) ? MenuState::PAUSE : MenuState::MAIN_MENU;
        }
        ImGui::End();
    }

    if (state == MenuState::SAVE || state == MenuState::LOAD) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
        ImGui::Begin("SaveLoad", nullptr, clearWindowFlags);
        ImGui::SetWindowFontScale(1.8f); 
        
        int buttonCount = 4;
        float totalHeight = (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
        ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));
        GetSaveNames();
        for(int i = 0; i < saveNames.size(); i++) {
            ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
            if(ImGui::Button(saveNames[i].c_str(), ImVec2(itemWidth, itemHeight))) {
                if(state == MenuState::SAVE) {
                    if(ImGui::IsItemClicked()) { 
                        ImGui::InputText("Save Name", saveName, 100);
                        ImGui::Text("Saving...");
                        SaveGame(player, camera, itemList, std::string(saveName) + ".json"); 
                        ImGui::Text("Saved!");
                    }
                }
                else { 
                    ImGui::Text("Loading...");
                    LoadGame(player, camera, itemList, std::string(saveName) + ".json"); 
                    ImGui::Text("Loaded!");
                }
            }
        }
        // Back
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
            state = (previousState == MenuState::PAUSE) ? MenuState::PAUSE : MenuState::MAIN_MENU;
        }
        ImGui::End();
    }

    if (state == MenuState::PAUSE && isGameStarted) {
        previousState = state;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
        ImGui::Begin("Pause", nullptr, clearWindowFlags);
        ImGui::SetWindowFontScale(1.8f); 
        
        int buttonCount = 6;
        float totalHeight = (itemHeight * buttonCount) + (spacingY * (buttonCount - 1));
        ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));

        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Continue", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::START;
        }
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Save", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::SAVE;
        }
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Load", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::LOAD;
        }
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Settings", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::SETTINGS;
        }
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Back To Main Menu", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::MAIN_MENU;
            isGameStarted = false;
        }
        ImGui::SetCursorPosX(currentWidth / 2.0f - itemWidth / 2.0f);
        if(ImGui::Button("Exit", ImVec2(itemWidth, itemHeight))) {
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

void Menu::SaveGame(Player* player, Camera* camera, std::map<int, std::unique_ptr<Items>>* itemList, std::string filename){
    json jsonfile;
    if(player == nullptr || camera == nullptr || itemList == nullptr) {
        std::cout << "Error: Player, camera, or item list is null!" << std::endl;
        return;
    }
    jsonfile["player"] = player->ToJson();
    jsonfile["camera"] = camera->ToJson();
    jsonfile["itemList"] = Items::ToJson(*itemList);
    std::ofstream ofs("saves/" + filename);
    ofs << jsonfile.dump(4, ' ', true);
    ofs.close();
}

void Menu::LoadGame(Player* player, Camera* camera, std::map<int, std::unique_ptr<Items>>* itemList, std::string filename){
    json jsonfile;
    std::ifstream ifs("saves/" + filename);
    ifs >> jsonfile;
    ifs.close();

}

void Menu::GetSaveNames(){
    saveNames.clear();
    if(!std::filesystem::exists("saves")) {
        std::filesystem::create_directory("saves");
    }
    if(std::filesystem::is_empty("saves")) {
        for(int i = 1; i <= 3; i++) {
            saveNames.push_back("Slot " + std::to_string(i));
        }
    }
    else {
        for(const auto& entry : std::filesystem::directory_iterator("saves")) {
            if(entry.path().extension() == ".json") {
                saveNames.push_back(entry.path().filename().string());
            }
        }
    }
}

