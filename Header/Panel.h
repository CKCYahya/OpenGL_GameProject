#ifndef PANEL_CLASS_H
#define PANEL_CLASS_H
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "imgui.h"
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

  void Update(Window &window, Player &player);

private:
  float slotSize;
  int slotCount;
  float bottomMargin;
};
#endif