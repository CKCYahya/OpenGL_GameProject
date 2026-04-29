#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "Texture.h"
#include <glm/glm.hpp>

class Animations {
public:
  int currentFrame;
  int maxFrames;
  float animTimer;
  float animSpeed;

  int cols;
  int rows;

  // Frame range support
  int startFrame;
  int endFrame;
  bool looping;
  bool finished;

  Animations(int maxFrames, float animSpeed, int cols, int rows);
  ~Animations();
  void Update(float deltaTime);
  void GetUVCoordinates(glm::vec2 &uv0, glm::vec2 &uv1);
  void SetRange(int start, int end, bool loop);
};

#endif // ANIMATIONS_H