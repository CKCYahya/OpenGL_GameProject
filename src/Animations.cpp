#include "Animations.h"

Animations::Animations(int maxFrames, float animSpeed, int cols, int rows) {
  this->maxFrames = maxFrames;
  this->animSpeed = animSpeed;
  this->cols = cols;
  this->rows = rows;
  this->currentFrame = 0;
  this->animTimer = 0.0f;
}

Animations::~Animations() {}

void Animations::Update(float deltaTime) {
  animTimer += deltaTime;
  if (animTimer >= animSpeed) {
    currentFrame++;
    if (currentFrame >= maxFrames) {
      currentFrame = 0;
    }
    animTimer = 0.0f;
  }
}

void Animations::GetUVCoordinates(glm::vec2 &uv0, glm::vec2 &uv1) {
  int col = currentFrame % cols;
  int row = currentFrame / cols;

  float uvWidth = 1.0f / cols;
  float uvHeight = 1.0f / rows;

  float u0 = col * uvWidth;
  float v0 = (rows - 1 - row) * uvHeight;

  float u1 = u0 + uvWidth;
  float v1 = v0 + uvHeight;

  uv0 = glm::vec2(u0, v0);
  uv1 = glm::vec2(u1, v1);
}