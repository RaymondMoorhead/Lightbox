#include "ShadowMap.h"
#include "../GraphicsController.h"
#include "../Object.h"
#include "../Light.h"

const unsigned ShadowMap::shadow_width = 1024;
const unsigned ShadowMap::shadow_height = 1024;

ShadowMap::ShadowMap() : FBO(shadow_width, shadow_height)
{
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