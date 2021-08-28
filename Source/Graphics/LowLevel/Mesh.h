#pragma once

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"

struct Mesh
{
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;
  VAO vao;
  
  unsigned instances;
  
  Mesh
  (
    std::vector<Vertex>& vertices,
    std::vector<GLuint>& indices,
    std::vector<Texture>& textures,
    unsigned instances = 1,
    std::vector<glm::mat4> instance_matrices = std::vector<glm::mat4>()
  );
  
  void Draw(
    Shader& shader,
    Camera& camera,
    glm::mat4 matrix = glm::mat4(1.0f),
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
    bool use_normal_map = true,
    bool use_displacement_map = true
  );
};