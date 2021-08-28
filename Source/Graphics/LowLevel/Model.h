#pragma once

#include <nlohmann/json.hpp>
#include "Mesh.h"

class Model
{
  public:
    // for loading from a file
    Model(const char* file, unsigned instances = 1, std::vector<glm::mat4> instance_matrices = std::vector<glm::mat4>());
    // for hard-coded additions
    Model
    (
      std::vector<Vertex>& vertices,
      std::vector<GLuint>& indices,
      std::vector<Texture>& textures,
      unsigned instances = 1,
      std::vector<glm::mat4> instance_matrices = std::vector<glm::mat4>()
    );
  
    void Draw
    (Shader& shader,
     Camera& camera,
     glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
     glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
     glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
     bool use_normal_map = true,
     bool use_displacement_map = true
     );
     
    bool HasNormalMap();
    bool HasDisplacementMap();
    
  private:
    const char* file_;
    std::vector<unsigned char> data_;
    nlohmann::json json_;
    unsigned instances_;
    bool has_normal_map_ = false;
    bool has_displacement_map_ = false;
    
    std::vector<Mesh> meshes_;
    std::vector<glm::vec3> translations_meshes_;
    std::vector<glm::quat> rotations_meshes_;
    std::vector<glm::vec3> scales_meshes_;
    std::vector<glm::mat4> matrices_meshes_;
    std::vector<glm::mat4> instance_matrices_;
    
    std::vector<std::string> loaded_tex_name_;
    std::vector<Texture> loaded_tex_;
    
    void LoadMesh_(unsigned mesh_index);
    
    void TraverseNode_(unsigned next_node, glm::mat4 matrix = glm::mat4(1.0f));
    
    void GetData_();
    std::vector<float> GetFloats_(nlohmann::json accessor);
    std::vector<GLuint> GetIndices_(nlohmann::json accessor);
    std::vector<Texture> GetTextures_();
    
    std::vector<Vertex> AssembleVertices_(
      std::vector<glm::vec3> positions,
      std::vector<glm::vec3> normals,
      std::vector<glm::vec2> uvs
    );
    
    std::vector<glm::vec2> GroupFloatsVec2_(std::vector<float> float_vec);
    std::vector<glm::vec3> GroupFloatsVec3_(std::vector<float> float_vec);
    std::vector<glm::vec4> GroupFloatsVec4_(std::vector<float> float_vec);
};