#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
struct Resolution {
    int width;
    int height;
};
static const Resolution resolutions[] = { {1920, 1080}, {1280, 720}, {2560, 1440}, {3840, 2160} };
static int selected_res = 0;

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

  void setResolution(Resolution resolution);

private:

  GLFWwindow *m_Window;
  
};

#endif