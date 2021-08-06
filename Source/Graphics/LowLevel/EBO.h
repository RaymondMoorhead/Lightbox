#pragma once

#include <glad/glad.h>
#include <vector>

struct EBO
{
  GLuint id;
  EBO(std::vector<GLuint>& indices);
  
  void Bind();
  void Unbind();
  void Delete();
};