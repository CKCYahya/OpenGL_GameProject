#include "../Header/Shader.h"

// Reads a text file and outputs a string with everything in the text file
// Shader.cpp

std::string get_file_contents(const char *filename) {
  std::ifstream in(filename, std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    if (contents.size() >= 3 && (unsigned char)contents[0] == 0xEF &&
        (unsigned char)contents[1] == 0xBB &&
        (unsigned char)contents[2] == 0xBF) {
      contents.erase(0, 3);
    }

    return (contents);
  }
  throw(errno);
}

// Constructor that build the Shader Program from 2 different shaders

Shader::Shader(const char *vertexFile, const char *fragmentFile) {
  std::string vertexCode = get_file_contents(vertexFile);
  std::string fragmentCode = get_file_contents(fragmentFile);

  const char *vertexSource = vertexCode.c_str();
  const char *fragmentSource = fragmentCode.c_str();

  int success;
  char infoLog[512];

  // --- Vertex Shader ---
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "HATA::SHADER::VERTEX::DERLEME_BASARISIZ\n"
              << infoLog << std::endl;
  }

  // --- Fragment Shader ---
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "HATA::SHADER::FRAGMENT::DERLEME_BASARISIZ\n"
              << infoLog << std::endl;
  }

  // --- Shader Program ---
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "HATA::SHADER::PROGRAM::LINKING_BASARISIZ\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

// Activates the Shader Program
void Shader::Activate() { glUseProgram(ID); }

// Deletes the Shader Program
void Shader::Delete() { glDeleteProgram(ID); }