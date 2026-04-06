
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL

Camera::Camera(int width, int height, glm::vec3 position) {
  Camera::width = width;
  Camera::height = height;
  Position = position;
}

void Camera::updateMatrix(float nearPlane, float farPlane, Shader &shader,
                          const char *uniformName) {
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
  // 2D
  view = glm::translate(view, -Position);
  // projection = glm::perspective(glm::radians(FOVdeg), (float)width / height,
  // nearPlane, farPlane); 2D

  projection = glm::ortho(0.0f, (float)width * zoom, 0.0f, (float)height * zoom,
                          nearPlane, farPlane);

  glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniformName), 1, GL_FALSE,
                     glm::value_ptr(projection * view));
}

void Camera::ProcessScroll(float yoffset) {
  // Scroll up (positive) -> Zoom In (Decrease scale)
  // Scroll down (negative) -> Zoom Out (Increase scale)
  zoom -= yoffset * 0.1f;

  // Clamp Zoom: 0.2f (5x Zoom) to maxZoom
  if (zoom < 0.2f)
    zoom = 0.2f;
  if (zoom > maxZoom)
    zoom = maxZoom;
}

void Camera::Inputs(GLFWwindow *window, float dt, glm::vec3 targetPos,
                    glm::vec4 mapBounds) {
  // --- 2D MOVEMENT ---
  float velocity = speed * dt;

  if (mode == CAMERA_FREE) {
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_CAPTURED) {
      // --- RTS CAMERA MOVEMENT (Age of Empires Edge Pan) ---
      double mouseX, mouseY;
      glfwGetCursorPos(window, &mouseX, &mouseY);

      int width, height;
      glfwGetWindowSize(window, &width, &height);

      // Edge threshold (how close to edge to convert)
      float edge = 10.0f;

      // Pan Left
      if (mouseX < edge)
        Position.x -= velocity;

      // Pan Right
      if (mouseX > width - edge)
        Position.x += velocity;

      // Pan Up
      if (mouseY < edge)
        Position.y += velocity;

      // Pan Down
      if (mouseY > height - edge)
        Position.y -= velocity;
    }
  } else if (mode == CAMERA_LOCKED) {
    Position.x = targetPos.x - (width * zoom) / 2.0f;
    Position.y = targetPos.y - (height * zoom) / 2.0f;
  }

  // --- CLAMP CAMERA ---
  // mapBounds: x=minX, y=minY, z=maxX, w=maxY (World Coords)
  // We clamps Position (bottom-left of view) so that valid view area stays in
  // bounds. Max allowed Position is MapMax - ViewSize

  float viewWidth = width * zoom;
  float viewHeight = height * zoom;

  float minX = mapBounds.x;
  float minY = mapBounds.y;
  float maxX = mapBounds.z - viewWidth;
  float maxY = mapBounds.w - viewHeight;

  // Safety: if locked view is larger than map, center it (or just clamp to min)
  if (maxX < minX)
    maxX = minX;
  if (maxY < minY)
    maxY = minY;

  if (Position.x < minX)
    Position.x = minX;
  if (Position.x > maxX)
    Position.x = maxX;
  if (Position.y < minY)
    Position.y = minY;
  if (Position.y > maxY)
    Position.y = maxY;

}