#ifndef LINE_H
#define LINE_H

#include "Camera.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <memory>

class Player;

class Line {
public:
  Line();
  ~Line();

  // Update the line endpoints based on animation frame and direction
  // rodTipWorld: world-space rod tip position (player pos + offset)
  // endWorld: world-space end position (where the line drops into water)
  void Update(int direction, int currentFrame, glm::vec3 playerPos,
              glm::vec3 endWorld);
  void Draw(Shader &shader, Camera &camera);

private:
  glm::vec3 startPos;
  glm::vec3 endPos;

  std::unique_ptr<VAO> lineVAO;
  std::unique_ptr<VBO> lineVBO;
};

// fishing-down.png (972x605, frame 243x302)
// Character faces down, rod swings from upper area to lower-left
const glm::vec3 rodTipOffsetsDown[8] = {
    glm::vec3(-8.0f, 22.0f, 0.0f),    // F0: rod upright, tip upper-left
    glm::vec3(-15.0f, 28.0f, 0.0f),   // F1: casting swing back
    glm::vec3(-22.0f, 12.0f, 0.0f),   // F2: cast forward
    glm::vec3(-25.0f, -8.0f, 0.0f),   // F3: rod extended lower-left (waiting)
    glm::vec3(-28.0f, -12.0f, 0.0f),  // F4: rod slightly lower
    glm::vec3(-25.0f, -8.0f, 0.0f),   // F5: rod back (loop with F3)
    glm::vec3(-18.0f, 8.0f, 0.0f),    // F6: rod pulling up (bite!)
    glm::vec3(-5.0f, 25.0f, 0.0f)};   // F7: celebration

// fishing-up.png (972x605, frame 243x302)
// Character faces up/away, rod extends behind to upper-right
const glm::vec3 rodTipOffsetsUp[8] = {
    glm::vec3(8.0f, 22.0f, 0.0f),     // F0: rod upright behind
    glm::vec3(15.0f, 32.0f, 0.0f),    // F1: casting swing up
    glm::vec3(22.0f, 38.0f, 0.0f),    // F2: cast over head
    glm::vec3(25.0f, 42.0f, 0.0f),    // F3: rod extended up-right (waiting)
    glm::vec3(28.0f, 45.0f, 0.0f),    // F4: rod slightly higher
    glm::vec3(25.0f, 42.0f, 0.0f),    // F5: rod back (loop with F3)
    glm::vec3(12.0f, 28.0f, 0.0f),    // F6: rod pulling (bite!)
    glm::vec3(0.0f, 20.0f, 0.0f)};    // F7: celebration

// fishing-left.png (1122x543, frame 280x271)
// Character faces left, rod extends to the left
const glm::vec3 rodTipOffsetsLeft[8] = {
    glm::vec3(-10.0f, 22.0f, 0.0f),   // F0: rod upright-left
    glm::vec3(-20.0f, 30.0f, 0.0f),   // F1: casting swing left
    glm::vec3(-30.0f, 20.0f, 0.0f),   // F2: cast extended left
    glm::vec3(-32.0f, 5.0f, 0.0f),    // F3: rod down-left (waiting)
    glm::vec3(-35.0f, 0.0f, 0.0f),    // F4: rod slightly lower
    glm::vec3(-32.0f, 5.0f, 0.0f),    // F5: rod back (loop with F3)
    glm::vec3(-22.0f, 18.0f, 0.0f),   // F6: rod pulling (bite!)
    glm::vec3(-8.0f, 28.0f, 0.0f)};   // F7: celebration

// fishing-right.png (1024x495, frame 256x247)
// Character faces right, rod extends to the right
const glm::vec3 rodTipOffsetsRight[8] = {
    glm::vec3(10.0f, 22.0f, 0.0f),    // F0: rod upright-right
    glm::vec3(20.0f, 30.0f, 0.0f),    // F1: casting swing right
    glm::vec3(30.0f, 20.0f, 0.0f),    // F2: cast extended right
    glm::vec3(32.0f, 5.0f, 0.0f),     // F3: rod down-right (waiting)
    glm::vec3(35.0f, 0.0f, 0.0f),     // F4: rod slightly lower
    glm::vec3(32.0f, 5.0f, 0.0f),     // F5: rod back (loop with F3)
    glm::vec3(22.0f, 18.0f, 0.0f),    // F6: rod pulling (bite!)
    glm::vec3(8.0f, 28.0f, 0.0f)};    // F7: celebration

#endif