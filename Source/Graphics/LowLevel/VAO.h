#pragma once

#include <glad/glad.h>
#include "VBO.h"

struct VAO
{
  GLuint id;
  VAO();
  
  void LinkAttribute(VBO& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset);
  
  void Bind();
  void Unbind();
  void Delete();
};