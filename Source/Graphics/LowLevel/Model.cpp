#include "Model.h"
#include <stdio.h>

Model::Model
(
  const char* file,
  unsigned instances,
  std::vector<glm::mat4> instance_matrices
) : file_(file),
    instances_(instances),
    instance_matrices_(instance_matrices)
{
  std::string text = GetFileContents(file_);
      
  if(text.empty())
    throw "Model file is either empty or does not exist";

  json_ = nlohmann::json::parse(text);

  GetData_();

  TraverseNode_(0);
}

Model::Model
(
  std::vector<Vertex>& vertices,
  std::vector<GLuint>& indices,
  std::vector<Texture>& textures,
  unsigned instances,
  std::vector<glm::mat4> instance_matrices
)
{
  meshes_.push_back(Mesh(vertices, indices, textures, instances, instance_matrices));
  matrices_meshes_.push_back(glm::mat4(1.0f));
  for(auto it = textures.begin(); it != textures.end(); ++it)
  {
    if(strcmp(it->type ,"normal") != 0)
      has_normal_map_ = true;
    else if(strcmp(it->type ,"displacement") != 0)
      has_displacement_map_ = true;
  }
}

void Model::Draw
(
  Shader& shader,
  Camera& camera,
  glm::vec3 translation,
  glm::quat rotation,
  glm::vec3 scale,
  bool use_normal_map,
  bool use_displacement_map
)
{
  for(unsigned i = 0; i < meshes_.size(); ++i)
    meshes_[i].Draw(shader, camera, matrices_meshes_[i], translation, rotation, scale, use_normal_map && has_normal_map_, use_displacement_map && has_displacement_map_);
}

bool Model::HasNormalMap()
{
  return has_normal_map_;
}

bool Model::HasDisplacementMap()
{
  return has_displacement_map_;
}

void Model::LoadMesh_(unsigned mesh_index)
{

  unsigned position_accessor_index =  json_["meshes"][mesh_index]["primitives"][0]["attributes"]["POSITION"];
  unsigned normal_accessor_index =    json_["meshes"][mesh_index]["primitives"][0]["attributes"]["NORMAL"];
  unsigned uv_accessor_index =        json_["meshes"][mesh_index]["primitives"][0]["attributes"]["TEXCOORD_0"];
  unsigned indices_accessor_index =   json_["meshes"][mesh_index]["primitives"][0]["indices"];

  std::vector<float> pos_vec = GetFloats_(json_["accessors"][position_accessor_index]);
  std::vector<glm::vec3> positions = GroupFloatsVec3_(pos_vec);

  std::vector<float> normal_vec = GetFloats_(json_["accessors"][normal_accessor_index]);
  std::vector<glm::vec3> normals = GroupFloatsVec3_(normal_vec);

  std::vector<float> uv_vec = GetFloats_(json_["accessors"][uv_accessor_index]);
  std::vector<glm::vec2> uvs = GroupFloatsVec2_(uv_vec);

  std::vector<Vertex> vertices = AssembleVertices_(positions, normals, uvs);
  std::vector<GLuint> indices = GetIndices_(json_["accessors"][indices_accessor_index]);
  std::vector<Texture> textures = GetTextures_();

  meshes_.push_back(Mesh(vertices, indices, textures, instances_, instance_matrices_));
}

void Model::TraverseNode_(unsigned next_node, glm::mat4 matrix)
{
  nlohmann::json node = json_["nodes"][next_node];
  
  glm::vec3 translation(0.0f, 0.0f, 0.0f);
  if(node.find("translation") != node.end())
  {
    float translation_values[3];
    for(unsigned i = 0; i < node["translation"].size(); ++i)
      translation_values[i] = node["translation"][i];
    translation = glm::make_vec3(translation_values);
  }
  
  glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
  if(node.find("rotation") != node.end())
  {
    float rotation_values[4] = 
    {
      node["rotation"][3],
      node["rotation"][0],
      node["rotation"][1],
      node["rotation"][2]
    };
    rotation = glm::make_quat(rotation_values);
  }
  
  glm::vec3 scale(1.0f, 1.0f, 1.0f);
  if(node.find("scale") != node.end())
  {
    float scale_values[3];
    for(unsigned i = 0; i < node["scale"].size(); ++i)
      scale_values[i] = node["scale"][i];
    scale = glm::make_vec3(scale_values);
  }
  
  glm::mat4 matrix_node(1.0f);
  if(node.find("matrix") != node.end())
  {
    float matrix_values[16];
    for(unsigned i = 0; i < node["matrix"].size(); ++i)
      matrix_values[i] = node["matrix"][i];
    matrix_node = glm::make_mat4(matrix_values);
  }
  
  glm::mat4 trans(1.0f);
  glm::mat4 rot(1.0f);
  glm::mat4 sca(1.0f);
  
  trans = glm::translate(trans, translation);
  rot = glm::mat4_cast(rotation);
  sca = glm::scale(sca, scale);
  
  glm::mat4 matrix_next_node = matrix * matrix_node * trans * rot * sca;
  
  if(node.find("mesh") != node.end())
  {
    translations_meshes_.push_back(translation);
    rotations_meshes_.push_back(rotation);
    scales_meshes_.push_back(scale);
    matrices_meshes_.push_back(matrix_next_node);
    
    LoadMesh_(node["mesh"]);
  }
  
  if(node.find("children") != node.end())
  {
    for(unsigned i = 0; i < node["children"].size(); ++i)
      TraverseNode_(node["children"][i], matrix_next_node);
  }
}

void Model::GetData_()
{
  std::string uri = json_["buffers"][0]["uri"];
  
  std::string file_str(file_);
  std::string file_directory = file_str.substr(0, file_str.find_last_of('/') + 1);

  std::string raw_bytes = GetFileContents((file_directory + uri).c_str());

  data_.assign(raw_bytes.begin(), raw_bytes.end());
}

std::vector<float> Model::GetFloats_(nlohmann::json accessor)
{
  std::vector<float> float_vec;
  
  unsigned buff_view_indices = accessor.value("bufferView", 1);
  unsigned count = accessor["count"];
  unsigned acc_byte_offset = accessor.value("byteOffset", 0);
  std::string type = accessor["type"];
  
  nlohmann::json buffer_view = json_["bufferViews"][buff_view_indices];
  unsigned byte_offset = buffer_view["byteOffset"];

  unsigned num_per_vert;
  if(type == "SCALAR") num_per_vert = 1;
  else if(type == "VEC2") num_per_vert = 2;
  else if(type == "VEC3") num_per_vert = 3;
  else if(type == "VEC4") num_per_vert = 4;
  else
  {
    printf("Model::GetData_ Type is invalid, exiting\n");
    return std::vector<float>();
  }

  unsigned data_start = byte_offset + acc_byte_offset;
  unsigned data_size = count * 4 * num_per_vert;
  for(unsigned i = data_start; i < data_start + data_size;)
  {
    unsigned char bytes[] = { data_[i++], data_[i++], data_[i++], data_[i++]};

    float value;
    std::memcpy(&value, bytes, sizeof(float));
    float_vec.push_back(value);
  }
  
  return float_vec;
}

std::vector<GLuint> Model::GetIndices_(nlohmann::json accessor)
{
  std::vector<GLuint> indices;
  
  unsigned buff_view_indices = accessor.value("bufferView", 0);
  unsigned count = accessor["count"];
  unsigned acc_byte_offset = accessor.value("byteOffset", 0);
  unsigned component_type = accessor["componentType"];
  
  nlohmann::json buffer_view = json_["bufferViews"][buff_view_indices];
  unsigned byte_offset = buffer_view["byteOffset"];
  
  unsigned data_start = byte_offset + acc_byte_offset;
  if(component_type == 5125)
  {
    for(unsigned i = data_start; i < byte_offset + acc_byte_offset + count * 4;)
    {
      unsigned char bytes[] = { data_[i++], data_[i++], data_[i++], data_[i++]};
      unsigned value;
      std::memcpy(&value, bytes, sizeof(unsigned));
      indices.push_back((GLuint)value);
    }
  }
  else if(component_type == 5123)
  {
    for(unsigned i = data_start; i < byte_offset + acc_byte_offset + count * 2;)
    {
      unsigned char bytes[] = { data_[i++], data_[i++]};
      unsigned short value;
      std::memcpy(&value, bytes, sizeof(unsigned short));
      indices.push_back((GLuint)value);
    }
  }
  // why would this be different than the above? Why bother shoving
  // the data into a signed value and then convert it to unsigned?
  else if(component_type == 5122)
  {
    for(unsigned i = data_start; i < byte_offset + acc_byte_offset + count * 2;)
    {
      unsigned char bytes[] = { data_[i++], data_[i++]};
      short value;
      std::memcpy(&value, bytes, sizeof(short));
      indices.push_back((GLuint)value);
    }
  }
  
  return indices;
}

std::vector<Texture> Model::GetTextures_()
{
  std::vector<Texture> textures;
  
  std::string file_str = std::string(file_);
  std::string file_dir = file_str.substr(0, file_str.find_last_of('/') + 1);
  
  for(unsigned i = 0; i < json_["images"].size(); ++i)
  {
    std::string tex_path = json_["images"][i]["uri"];
    
    bool skip = false;
    for(size_t j = 0; j < loaded_tex_name_.size(); ++j)
      if(loaded_tex_name_[j] == tex_path)
      {
        textures.push_back(loaded_tex_[j]);
        skip = true;
        break;
      }
    if(skip)
      continue;
    
    // Textures 0-19 are reserved for the shadow maps
    if(tex_path.find("baseColor") != std::string::npos || tex_path.find("diffuse") != std::string::npos)
    {
      Texture diffuse = Texture((file_dir + tex_path).c_str(), "diffuse", loaded_tex_.size() + 20);
      textures.push_back(diffuse);
      loaded_tex_.push_back(diffuse);
      loaded_tex_name_.push_back(tex_path);
    }
    else if(tex_path.find("metallicRoughness") != std::string::npos || tex_path.find("specular") != std::string::npos)
    {
      Texture specular = Texture((file_dir + tex_path).c_str(), "specular", loaded_tex_.size() + 20);
      textures.push_back(specular);
      loaded_tex_.push_back(specular);
      loaded_tex_name_.push_back(tex_path);
    }
  }
  
  return textures;
}

std::vector<Vertex> Model::AssembleVertices_(
  std::vector<glm::vec3> positions,
  std::vector<glm::vec3> normals,
  std::vector<glm::vec2> uvs
)
{
  std::vector<Vertex> vertices;
  for(int i = 0; i < positions.size(); ++i)
  {
    vertices.push_back
    (
      Vertex
      (
        positions[i],
        normals[i],
        glm::vec3(1.0f, 1.0f, 1.0f),
        uvs[i]
      )
    );
  }
  return vertices;
}

std::vector<glm::vec2> Model::GroupFloatsVec2_(std::vector<float> float_vec)
{
  std::vector<glm::vec2> vectors;
  for(size_t i = 0; i < float_vec.size();)
    vectors.push_back(glm::vec2(float_vec[i++], float_vec[i++]));
  return vectors;
}

std::vector<glm::vec3> Model::GroupFloatsVec3_(std::vector<float> float_vec)
{
  std::vector<glm::vec3> vectors;
  for(size_t i = 0; i < float_vec.size();)
    vectors.push_back(glm::vec3(float_vec[i++], float_vec[i++], float_vec[i++]));
  return vectors;
}

std::vector<glm::vec4> Model::GroupFloatsVec4_(std::vector<float> float_vec)
{
  std::vector<glm::vec4> vectors;
  for(size_t i = 0; i < float_vec.size();)
    vectors.push_back(glm::vec4(float_vec[i++], float_vec[i++], float_vec[i++], float_vec[i++]));
  return vectors;
}