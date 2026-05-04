#ifndef VENDOR_H
#define VENDOR_H

#include "Camera.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <memory>


class Vendor {
public:
  Vendor();
  ~Vendor();

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

#endif