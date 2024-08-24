#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

unsigned int compileShader(const char *shaderSource, GLenum type,
                           const char *lable) {
  unsigned int shader;
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::" << lable << "::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  return shader;
}

unsigned int linkProgram(unsigned const int vertexShader,
                         unsigned const int fragmentShader) {
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  return shaderProgram;
}

unsigned int getProgram(const char *vertexShaderSource,
                        const char *fragmentShaderSource) {
  unsigned int vertexShader =
      compileShader(vertexShaderSource, GL_VERTEX_SHADER, "VERTEX");
  unsigned int fragmentShader =
      compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER, "FRAGMENT");

  unsigned int shaderProgram = linkProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}
