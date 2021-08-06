#pragma once

#include "Shader.h"

#include <glad/glad.h>

struct Texture
{
  GLuint id;
  const char* type;
  GLuint unit;
  
  Texture(const char* image_file, const char* tex_type, GLuint slot);
  
  void SetUniform(Shader& shader, const char* uniform_name, GLuint unit);
  
  void Bind();
  void Unbind();
  void Delete();
};