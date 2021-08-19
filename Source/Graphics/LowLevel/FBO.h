#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

struct FBO
{
  unsigned width, height;
  unsigned depth_map_fbo;
  unsigned depth_map_tex;
  GLenum tex_type = -1;
  
  FBO(unsigned width, unsigned height);
  
  void Initialize(GLenum tex_type, bool ignore_color = false);
  void ChangeType(GLenum tex_type);
  void Bind();
  void BindTex(unsigned unit);
  void Delete();
  
  private:
    void SetUp_(GLenum tex_type);
    void DeleteTex_();
};