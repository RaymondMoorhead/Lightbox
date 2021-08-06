#include "VBO.h"

Vertex::Vertex
(
  glm::vec3 position_,
  glm::vec3 normal_,
  glm::vec3 color_,
  glm::vec2 uv_
) : position(position_), normal(normal_), color(color_), uv(uv_)
{
}

VBO::VBO(std::vector<Vertex>& vertices)
{
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO(std::vector<glm::mat4>& matrices)
{
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), matrices.data(), GL_STATIC_DRAW);
}
  
void VBO::Bind()
{
  glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::Unbind()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
  glDeleteBuffers(1, &id);
}