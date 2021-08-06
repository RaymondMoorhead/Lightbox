#pragma once

#include <map>
#include <glm/glm.hpp>
#include <glad/glad.h>

class GraphicsController;
class Object;
struct Shader;
struct Light;

struct ShadowMap
{
  unsigned depth_map_fbo;
  unsigned depth_map_tex;
  GLenum tex_type = -1;
  
  void Initialize(GLenum tex_type);
  void ChangeType(GLenum tex_type);
  void DrawMap(GraphicsController* g_control, std::map<std::string, Object*>& objects, Light* light);
  void BindTex(unsigned unit);
  void DeleteTex();
  
  static const unsigned width;
  static const unsigned height;
  
  private:
    void SetUp_(GLenum tex_type);
};