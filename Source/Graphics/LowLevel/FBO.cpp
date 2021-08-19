#include "FBO.h"

#include <stdio.h>

FBO::FBO(unsigned width_, unsigned height_) : width(width_), height(height_)
{
}

void FBO::Initialize(GLenum tex_type_, bool ignore_color)
{
  glGenFramebuffers(1, &depth_map_fbo);
  
  SetUp_(tex_type_);
  
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  
  if(ignore_color)
  {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
  
  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(result != GL_FRAMEBUFFER_COMPLETE)
    printf("FBO::SetUp_ has an incomplete framebuffer: %d\n", result);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::ChangeType(GLenum tex_type_)
{
  DeleteTex_();
  SetUp_(tex_type_);
}

void FBO::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
}

void FBO::BindTex(unsigned unit)
{
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(tex_type, depth_map_tex);
}

void FBO::Delete()
{
  if(tex_type != -1)
    DeleteTex_();
  glDeleteFramebuffers(1, &depth_map_fbo);
}

void FBO::SetUp_(GLenum tex_type_)
{
  tex_type = tex_type_;
  glGenTextures(1, &depth_map_tex);
  glBindTexture(tex_type, depth_map_tex);
  
  if(tex_type == GL_TEXTURE_CUBE_MAP)
  {
    for(unsigned i = 0; i < 6; ++i)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  depth_map_tex, 0);
  }
  else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, depth_map_tex, 0);
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::DeleteTex_()
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  if(tex_type == GL_TEXTURE_CUBE_MAP)
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  0, 0);
  else
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, 0, 0);
  glDeleteTextures(1, &depth_map_tex);
  tex_type = -1;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}