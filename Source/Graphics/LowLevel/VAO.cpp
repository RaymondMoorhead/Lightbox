#include "VAO.h"  

VAO::VAO()
{
  glGenVertexArrays(1, &id);
}

void VAO::LinkAttribute(VBO& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset)
{
  vbo.Bind();
  
  glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  
  vbo.Unbind();
}
  
void VAO::Bind()
{
  glBindVertexArray(id);
}

void VAO::Unbind()
{
  glBindVertexArray(0);
}

void VAO::Delete()
{
  glDeleteVertexArrays(1, &id);
}