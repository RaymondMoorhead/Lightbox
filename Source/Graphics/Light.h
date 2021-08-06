#pragma once

#include <glm/glm.hpp>
#include "LowLevel/ShadowMap.h"

struct Shader;

enum LightType
{
  Directional,
  Point,
  Spot
};

extern const char* LightTypeNames[];

struct Light
{
  Light();
  Light(glm::vec3 pos, glm::vec3 dir, glm::vec4 col, LightType type, float cone_outer = 0.90f, float cone_inner = 0.95f);
  
  void Initialize();
  void BindShadowMap(unsigned light_num);
  
  Light& operator=(const Light& rhs);
  
  glm::vec3 position        = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 direction       = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec4 color           = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  float cone_outer          = 0.90f;
  float cone_inner          = 0.95f;
  LightType type            = Directional;
  float intensity_quadratic = 0.5f;
  float intensity_linear    = 0.0f;
  ShadowMap shadow_map;
  
  static glm::mat4 ortho_projection;
  static glm::mat4 persp_projection;
  static void UpdateProjections(float near_plane, float far_plane, float aspect_ratio);
  
  void UpdateUniforms(Shader** shaders, unsigned light_num, unsigned num_shaders = 1);
  
  bool ImGuiDraw();
  
  // Shaders don't *easily* support arrays of
  // varying size, so as a quick hack we use
  // an array of specified size, and deactivate
  // undesired lights
  bool is_on;
};