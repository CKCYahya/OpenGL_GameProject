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
    float itemHeight = currentHeight / 12.0f;
    float totalHeight = (itemHeight * 4) + (ImGui::GetStyle().ItemSpacing.y * 3);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
    if (state == MenuState::MAIN_MENU && !isGameStarted) {
    previousState = state;
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));
    ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
    if (ImGui::Button("Start", ImVec2(itemWidth, itemHeight))) {
        state = MenuState::START;
        isGameStarted = true;
    }
    ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
    if (ImGui::Button("Load", ImVec2(itemWidth, itemHeight))) {
        state = MenuState::LOAD;
    }
    ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
    if (ImGui::Button("Settings", ImVec2(itemWidth, itemHeight))) {
        state = MenuState::SETTINGS;
    }
    ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
    if (ImGui::Button("Exit", ImVec2(itemWidth, itemHeight))) {
        state = MenuState::EXIT;
    }
    ImGui::End();
    }

    if (state == MenuState::SETTINGS) {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
            if(previousState == MenuState::PAUSE) {
                state = MenuState::PAUSE;
            } else {
                state = MenuState::MAIN_MENU;
            }
        }
        ImGui::End();
    }


    if (state == MenuState::SAVE || state == MenuState::LOAD) {
        ImGui::Begin("Save", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::SetCursorPosY((currentHeight / 2.0f) - (totalHeight / 2.0f));
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Save1", ImVec2(itemWidth, itemHeight))) {
            if(state == MenuState::SAVE) {
                SaveGame(player, camera, itemList, "save1.json");
            } else {
                LoadGame();
            }
        }
        if(ImGui::Button("Save2", ImVec2(itemWidth, itemHeight))) {
            if(state == MenuState::SAVE) {
                SaveGame(player, camera, itemList, "save2.json");
            } else {
                LoadGame();
            }
        }
        if(ImGui::Button("Save3", ImVec2(itemWidth, itemHeight))) {
            if(state == MenuState::SAVE) {
                SaveGame(player, camera, itemList, "save3.json");
            } else {
                LoadGame();
            }
        }
        if(ImGui::Button("Back", ImVec2(itemWidth, itemHeight))) {
            if(previousState == MenuState::PAUSE) {
                state = MenuState::PAUSE;
            } else {
                state = MenuState::MAIN_MENU;
            }
        }
        ImGui::End();
    }

    if (state == MenuState::PAUSE && isGameStarted) {
        previousState = state;
        ImGui::Begin("Pause", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Continue", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::START;
        }
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Save", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::SAVE;
        }
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Load", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::LOAD;
        }
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Settings", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::SETTINGS;
        }
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("BackToMainMenu", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::MAIN_MENU;
            isGameStarted = false;
        }
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Exit", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::EXIT;
        }
        ImGui::End();
    }

    if (state == MenuState::EXIT) {
        glfwSetWindowShouldClose(window->getGLFWWindow(), GLFW_TRUE);
    }
}

void Menu::SaveGame(Player* player, Camera* camera, std::map<int, std::unique_ptr<Items>>* itemList, const char* filename){
    json jsonfile;
    jsonfile["player"] = player->ToJson();
    jsonfile["camera"] = camera->ToJson();
    jsonfile["itemList"] = Items::ToJson(*itemList);
    std::ofstream ofs(filename);
    ofs << jsonfile.dump(4, ' ', true);
    ofs.close();
}

void Menu::LoadGame(){

}