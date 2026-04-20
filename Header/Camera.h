#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Shader.h"

enum CameraMode { CAMERA_FREE, CAMERA_LOCKED };

class Camera {
public:
  glm::vec3 Position;
  glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

  int width;
  int height;

  float speed = 0.1f;
  float sensitivity = 100.0f;
  float zoom = 1.0f;
  float maxZoom = 1.0f;

  CameraMode mode = CAMERA_FREE;

  // Constructor
  Camera(int width, int height, glm::vec3 position);

  // void updateMatrix(float FOVdeg, float nearPlane, float farPlane, Shader&
  // shader, const char* uniformName);
  void updateMatrix(float nearPlane, float farPlane, Shader &shader,
                    const char *uniformName);

  // Handle scroll input for zooming
  void ProcessScroll(float yoffset);

  void Inputs(GLFWwindow *window, float dt, glm::vec3 targetPos,
              glm::vec4 mapBounds);
};

#endif