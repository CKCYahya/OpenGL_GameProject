#include "../Header/Texture.h"
#include <iostream>
Texture::Texture(const char *image, GLenum texType, GLenum slot, GLenum format,
                 GLenum pixelType) {
  // Assigns the type of the texture ot the texture object
  type = texType;

  // Stores the width, height, and the number of color channels of the image

  // Reads the image from a file and stores it in bytes
  unsigned char *bytes = stbi_load(image, &width, &height, &channels, 0);
  if (!bytes) {
    std::cout << "Failed to load texture: " << image << std::endl;
    // std::cout << stbi_failure_reason() << std::endl; // If header available
    // Create a 1x1 magenta texture to indicate error?
    static unsigned char magenta[] = {255, 0, 255, 255};
    bytes = magenta;
    width = 1;
    height = 1;
    channels = 4;
    format = GL_RGBA;
    pixelType = GL_UNSIGNED_BYTE;
  } else {
    std::cout << "Loaded texture: " << image << " (" << width << "x" << height
              << ")" << std::endl;
  }

  // Generates an OpenGL texture object
  glGenTextures(1, &ID);
  // Assigns the texture to a Texture Unit
  glActiveTexture(slot);
  glBindTexture(texType, ID);

  // Configures the type of algorithm that is used to make the image smaller or
  // bigger
  glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Configures the way the texture repeats (if it does at all)
  glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
  // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

  // Assigns the image to the OpenGL Texture object
  glTexImage2D(texType, 0, GL_RGBA, width, height, 0, format, pixelType, bytes);
  // Generates MipMaps
  glGenerateMipmap(texType);

  // Deletes the image data as it is already in the OpenGL Texture object
  stbi_image_free(bytes);

  // Unbinds the OpenGL Texture object so that it can't accidentally be modified
  glBindTexture(texType, 0);
}

void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit) {
  // Gets the location of the uniform
  GLuint texUni = glGetUniformLocation(shader.ID, uniform);
  // Shader needs to be activated before changing the value of a uniform
  shader.Activate();
  // Sets the value of the uniform
  glUniform1i(texUni, unit);
}

void Texture::Bind() { glBindTexture(type, ID); }

void Texture::Unbind() { glBindTexture(type, 0); }

void Texture::Delete() { glDeleteTextures(1, &ID); }