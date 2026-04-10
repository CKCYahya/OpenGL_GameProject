#include "Window.h"
#include <iostream>

Window::Window(const char *title, int width, int height) {
  if (!glfwInit()) {
    std::cout << "GLFW baslatilamadi!" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (m_Window == NULL) {
    std::cout << "Pencere olusturulamadi!" << std::endl;
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(m_Window);

  // Register the resize callback
  glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "GLAD yuklenemedi!" << std::endl;
    return;
  }

  glViewport(0, 0, width, height);
}

Window::~Window() {
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

bool Window::ShouldClose() { return glfwWindowShouldClose(m_Window); }

void Window::SwapBuffers() { glfwSwapBuffers(m_Window); }

void Window::PollEvents() { glfwPollEvents(); }

// Static callback function to handle window resizing
void Window::FramebufferSizeCallback(GLFWwindow *window, int width,
                                     int height) {
  // Adjust the viewport to the new window dimensions
  glViewport(0, 0, width, height);
}

void Window::setResolution(Resolution resolution) {
  glfwSetWindowSize(m_Window, resolution.width, resolution.height);
}
