#include "ShadowMap.h"
#include "../GraphicsController.h"
#include "../Object.h"
#include "../Light.h"

#include <stdio.h>

const unsigned ShadowMap::width = 1024;
const unsigned ShadowMap::height = 1024;

void ShadowMap::Initialize(GLenum tex_type_)
{
  glGenFramebuffers(1, &depth_map_fbo);
  
  SetUp_(tex_type_);
  
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  
  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(result != GL_FRAMEBUFFER_COMPLETE)
    printf("ShadowMap::SetUp_ has an incomplete framebuffer: %d\n", result);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::ChangeType(GLenum tex_type_)
{
  DeleteTex();
  SetUp_(tex_type_);
}

void ShadowMap::DrawMap(GraphicsController* g_control, std::map<std::string, Object*>& objects, Light* light)
{
  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  if(!light->is_on)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return;
  }
  
  glViewport(0, 0, width, height);
  
  Shader* shader_depth = g_control->GetShader(tex_type == GL_TEXTURE_CUBE_MAP ? DepthCube : DepthFlat);
  shader_depth->Activate();
  
  GLint loc;

  if(tex_type == GL_TEXTURE_CUBE_MAP)
  {
    glm::mat4 light_matrix[6];
    light_matrix[0] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
    light_matrix[1] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
    light_matrix[2] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f));
    light_matrix[3] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3( 0.0f, 0.0f,-1.0f));
    light_matrix[4] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
    light_matrix[5] = Light::persp_projection * glm::lookAt(light->position, light->position + glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3( 0.0f,-1.0f, 0.0f));
    
    for(unsigned i = 0; i < 6; ++i)
    {
      loc = glGetUniformLocation(shader_depth->id, ("shadow_matrices[" + std::to_string(i) + "]").c_str());
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(light_matrix[i]));
    }
    
    loc = glGetUniformLocation(shader_depth->id, "lightType");
    glUniform1i(loc, int(light->type));
    
    loc = glGetUniformLocation(shader_depth->id, "lightPos");
    glUniform3f(loc, light->position.x, light->position.y, light->position.z);
    
    loc = glGetUniformLocation(shader_depth->id, "far_plane");
    glUniform1f(loc, 100.0f);
  }
  else
  {
    glm::mat4 light_matrix = Light::ortho_projection * glm::lookAt(light->position, light->position + light->direction, glm::vec3( 0.0f, 1.0f, 0.0f));
    loc = glGetUniformLocation(shader_depth->id, "shadow_matrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(light_matrix));
  }
  
  for(auto it = objects.begin(); it != objects.end(); ++it)
    it->second->Draw(*(g_control->camera), shader_depth);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::BindTex(unsigned unit)
{
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(tex_type, depth_map_tex);
}

void ShadowMap::DeleteTex()
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

void ShadowMap::SetUp_(GLenum tex_type_)
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