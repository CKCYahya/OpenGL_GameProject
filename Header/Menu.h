#ifndef MENU_H
#define MENU_H

#include "EBO.h"
#include "Shader.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>

class Player;
class Camera;
class Items;

enum class MenuState { MAIN_MENU, START, SETTINGS, PAUSE, SAVE, LOAD, EXIT };

class Menu {
public:
  char saveName[100] = "";
  std::vector<std::string> saveNames;
  MenuState state;
  MenuState previousState;
  bool isGameStarted;
  std::unique_ptr<Texture> menuTexture;
  std::unique_ptr<VAO> vao;
  std::unique_ptr<VBO> vbo;
  std::unique_ptr<EBO> ebo;
  int saveSlotCount;
  bool showSaveConfirm = false;
  Menu(Window *window, Shader *shader);
  ~Menu();
  void LoadAssets(Shader *shader);
  void Draw(Window *window, Shader *shader, Player *player, Camera *camera,
            std::map<int, std::unique_ptr<Items>> *itemList);
  void SaveGame(Player *player, Camera *camera,
                std::map<int, std::unique_ptr<Items>> *itemList,
                std::string filename);
  void LoadGame(Player *player, Camera *camera,
                std::map<int, std::unique_ptr<Items>> *itemList,
                std::string filename);
  void GetSaveNames();
  void newSaveSection(Player *player, Camera *camera,
                      std::map<int, std::unique_ptr<Items>> *itemList);
};

#endif