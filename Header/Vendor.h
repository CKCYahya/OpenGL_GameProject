#ifndef VENDOR_H
#define VENDOR_H

#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"

class Vendor {
public:
  Vendor();
  ~Vendor();

  void Update(GLFWwindow *window, int direction, int currentFrame,
              glm::vec3 playerPos, glm::vec3 endWorld);
  static void Sell(Player &player);
  static void Upgrade(Items &item, Player &player);

private:
  glm::vec3 startPos;
  glm::vec3 endPos;
  bool hasReleasedKey = false;
};

#endif