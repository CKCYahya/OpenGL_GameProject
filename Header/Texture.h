#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include "../libs/glad/include/glad/glad.h"
#include "../libs/glfw/include/glfw/glfw3.h"
#include "../libs/stb/stb_image.h"

#include "../Header/Shader.h"

class Texture {
public:
  GLuint ID;
  GLenum type;
  int width, height, channels;
  Texture(const char *image, GLenum texType, GLenum slot, GLenum format,
          GLenum pixelType);

  // Assigns a texture unit to a texture
  void texUnit(Shader &shader, const char *uniform, GLuint unit);
  // Binds a texture
  void Bind();
  // Unbinds a texture
  void Unbind();
  // Deletes a texture
  void Delete();
};
#endif