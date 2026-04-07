#ifndef MENU_H
#define MENU_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Window.h"
#include "Texture.h"

enum class MenuState {
    MAIN_MENU,
    START,
    SETTINGS,
    PAUSE,
    EXIT
};

class Menu {
public:
    MenuState state;
    MenuState previousState;
    bool isGameStarted;
    std::unique_ptr<Texture> menuTexture;
    std::unique_ptr<VAO> vao;
    std::unique_ptr<VBO> vbo;
    std::unique_ptr<EBO> ebo;
    Menu(Window* window, Shader* shader);
    ~Menu();
    void LoadAssets(Shader* shader);
    void Draw(Shader* shader);
};

#endif