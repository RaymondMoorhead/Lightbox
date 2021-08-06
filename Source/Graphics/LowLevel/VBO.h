#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

struct Vertex
{
  Vertex
  (
    glm::vec3 position,
    glm::vec3 normal,
    glm::vec3 color,
    glm::vec2 uv
  );
  
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 uv;
};

struct VBO
{
  GLuint id;
  VBO(std::vector<Vertex>& vertices);
  VBO(std::vector<glm::mat4>& matrices);
  
  void Bind();
  void Unbind();
  void Delete();
};