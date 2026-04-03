#ifndef PANEL_CLASS_H
#define PANEL_CLASS_H
#include "../libs/glad/include/glad/glad.h"
#include "../libs/glm/glm.hpp"
#include "../libs/imgui/imgui.h"
#include <map>
#include <memory>
class Window;
class Items;
class Player;
class Panel {
public:
  int PANEL_WIDTH;
  int PANEL_HEIGHT;

  GLuint panelID;

  Panel();
  ~Panel();

  void Update(Window &window, Player &player,
              std::map<int, std::unique_ptr<Items>> &itemList);

private:
  float slotSize;
  int slotCount;
  float bottomMargin;
};
#endif