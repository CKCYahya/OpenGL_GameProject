#include "Menu.h"
#include <iostream>

Menu::Menu(Window* window, Shader* shader) {
    state = MenuState::MAIN_MENU;
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLfloat indices[] = {
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
    menuTexture = std::make_unique<Texture>("image/menu.png", GL_RGBA);
    menuTexture->texUnit(shader, "tex0", 0);
}

void Menu::Draw(Shader* shader, Window* window) {
    shader->Activate();
    glUniform2f(glGetUniformLocation(shader->ID, "texScale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(shader->ID, "texOffset"), 0.0f, 0.0f);
    menuTexture->Bind();
    vao->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    int currentWidth;
    int currentHeight;
    glfwGetWindowSize(window->getGLFWWindow(), &currentWidth, &currentHeight);

    float itemWidth = currentWidth / 5;
    float itemHeight = currentHeight / 7;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(currentWidth, currentHeight));
    if (state == MenuState::MAIN_MENU && !isGameStarted) {
    previousState = state;
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
    if (ImGui::Button("Start", ImVec2(itemWidth, itemHeight))) {
        state = MenuState::START;
        isGameStarted = true;
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
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
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

    if (state == MenuState::PAUSE && isGameStarted) {
        previousState = state;
        ImGui::Begin("Pause", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::SetCursorPosX(currentWidth / 2 - itemWidth / 2);
        if(ImGui::Button("Continue", ImVec2(itemWidth, itemHeight))) {
            state = MenuState::START;
        }
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
        window->Close();
    }
}
