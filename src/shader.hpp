#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
  unsigned int programID;
  Shader(const char* vertPath, const char* fragPath);
  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;

};

Shader::Shader(const char* vertPath, const char* fragPath) {
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  // Ensure ifstream objects can throw exceptions
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // Open files
    vShaderFile.open(vertPath);
    fShaderFile.open(fragPath);
    std::stringstream vShaderStream, fShaderStream;

    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // Close file headers
    vShaderFile.close();
    fShaderFile.close();

    // Convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();

  // Compile shaders
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // Vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);

  // Print compiler errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);

  // Print compiler errors if any
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Shader program
  this->programID = glCreateProgram();
  glAttachShader(this->programID, vertex);
  glAttachShader(this->programID, fragment);
  glLinkProgram(this->programID);

  // Print linking errors (if any)
  glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  
  glDeleteShader(vertex);
  glDeleteShader(fragment);


}

void Shader::use() {
  glUseProgram(this->programID);
}

void Shader::setBool(const std::string &name, bool value) const {
  int location = glGetUniformLocation(this->programID, name.c_str());
  glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  int location = glGetUniformLocation(this->programID, name.c_str());
  glUniform1i(location, value);
}

void Shader::setFloat(const std::string &name, float value) const {
  int location = glGetUniformLocation(this->programID, name.c_str());
  glUniform1f(location, value);
}

#endif
