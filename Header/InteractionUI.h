#ifndef INTERACTION_UI_H
#define INTERACTION_UI_H

#include "Camera.h"
#include "Fishing.h"
#include "Player.h"
#include "Shader.h"
#include "Window.h"
#include <imgui.h> // ImGui tipleri (ImVec2 vb.) için gerekli
#include <map>

// Forward Declarations (Header'ı hafifletir ve çakışmaları önler)

enum class PopupState { SHOW, HIDE };

class InteractionUI {
private:
  float lastTime;
  char message[256];

public:
  InteractionUI();
  ~InteractionUI();
  PopupState state;

  void showInteractionUI(Fishing &fishing, Player &player, bool nearItem);
  void Draw(Camera *camera, Player *player, Window *window);
};

#endif
