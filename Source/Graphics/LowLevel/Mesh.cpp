#include "Mesh.h"

#include <string>

Mesh::Mesh
(
  std::vector<Vertex>& vertices_,
  std::vector<GLuint>& indices_,
  std::vector<Texture>& textures_,
  unsigned instances_,
  std::vector<glm::mat4> instance_matrices
) :
  vertices(vertices_),
  indices(indices_),
  textures(textures_),
  instances(instances_)
{
  vao.Bind();
  
  VBO instance_vbo(instance_matrices);
  VBO vbo(vertices);
  EBO ebo(indices);
  
  vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), nullptr);
  vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
  vao.LinkAttribute(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
  vao.LinkAttribute(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
  
  if(instances != 1)
  {
    instance_vbo.Bind();
    
    vao.LinkAttribute(instance_vbo, 4, 4, GL_FLOAT, sizeof(glm::mat4), nullptr);
    vao.LinkAttribute(instance_vbo, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
    vao.LinkAttribute(instance_vbo, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    vao.LinkAttribute(instance_vbo, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);
  }
  
  vao.Unbind();
  vbo.Unbind();
  instance_vbo.Unbind();
  ebo.Unbind();
}

void Mesh::Draw
(
    Shader& shader, Camera& camera, glm::mat4 matrix,
    glm::vec3 translation, glm::quat rotation, glm::vec3 scale,
    bool use_normal_map
)
{
  shader.Activate();
  vao.Bind();
  
  unsigned num_diffuse = 0;
  unsigned num_specular = 0;
  unsigned num_normal = 0;
  
  for(size_t i = 0; i < textures.size(); ++i)
  {
    std::string num;
    std::string type = textures[i].type;
    
    if(type == "diffuse")
      num = std::to_string(num_diffuse++);
    else if(type == "specular")
      num = std::to_string(num_specular++);
    else if(type == "normal")
      num = std::to_string(num_normal++);
    
    // GL_TEXTURE0-19 is reserved for the shadow maps
    textures[i].SetUniform(shader, (type + num).c_str(), i + 20);
    textures[i].Bind();
  }
  glUniform3f(glGetUniformLocation(shader.id, "camPos"), camera.position.x, camera.position.y, camera.position.z);
  camera.Matrix(shader, "camMatrix");
  glUniform1i(glGetUniformLocation(shader.id, "use_normal_map"), num_normal && use_normal_map);
  
  if(instances == 1)
  {
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);
    
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);
    
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
    
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  }
  else
  {
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instances);
  }
}