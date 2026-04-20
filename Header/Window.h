#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Window {
public:
  Window(const char *title, int width, int height);
  ~Window();

  bool ShouldClose();
  void SwapBuffers();
  void PollEvents();
  GLFWwindow *getGLFWWindow() const { return m_Window; }

  // Public resize callback to be called from GLFW
  static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);

private:
  GLFWwindow *m_Window;
  
};

#endif