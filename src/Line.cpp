#include "Line.h"
#include "Camera.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Line::Line() {
  startPos = glm::vec3(0.0f);
  endPos = glm::vec3(0.0f);

  // Create VAO/VBO once with placeholder data (2 vertices * 3 floats)
  GLfloat vertices[6] = {0.0f};

  lineVAO = std::make_unique<VAO>();
  lineVAO->Bind();

  lineVBO = std::make_unique<VBO>(vertices, sizeof(vertices));
  lineVAO->LinkAttrib(*lineVBO, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat),
                      (void *)0);

  lineVAO->Unbind();
  lineVBO->Unbind();
}

Line::~Line() {}

void Line::Update(int direction, int currentFrame, glm::vec3 playerPos,
                  glm::vec3 endWorld) {
  // Get the rod tip offset for this frame and direction
  glm::vec3 offset(0.0f);
  switch (direction) {
  case 0: // Down
    offset = rodTipOffsetsDown[currentFrame];
    break;
  case 1: // Up
    offset = rodTipOffsetsUp[currentFrame];
    break;
  case 2: // Left
    offset = rodTipOffsetsLeft[currentFrame];
    break;
  case 3: // Right
    offset = rodTipOffsetsRight[currentFrame];
    break;
  }

  // Rod tip in world space = player position + frame offset
  startPos = playerPos + offset;
  endPos = endWorld;

  // Update VBO data on GPU
  GLfloat vertices[] = {startPos.x, startPos.y, startPos.z,
                        endPos.x,   endPos.y,   endPos.z};

  lineVBO->Bind();
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  lineVBO->Unbind();
}

void Line::Draw(Shader &shader, Camera &camera) {
  shader.Activate();

  // Line is already in world space, so model = identity
  glm::mat4 model = glm::mat4(1.0f);

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camMatrix"), 1, GL_FALSE,
                     glm::value_ptr(camera.projection * camera.view));
  glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE,
                     glm::value_ptr(model));

  // Set line color (white-ish fishing line)
  glUniform4f(glGetUniformLocation(shader.ID, "lineColor"), 0.9f, 0.9f, 0.9f,
              1.0f);

  lineVAO->Bind();
  glDrawArrays(GL_LINES, 0, 2);
  lineVAO->Unbind();
}
