#include "Light.h"

#include "LowLevel/Shader.h"
#include "GraphicsController.h"

#include <imgui/imgui.h>

glm::mat4 Light::ortho_projection;
glm::mat4 Light::persp_projection;

const char* LightTypeNames[] =
{
  "Directional",
  "Point",
  "Spot"
};

Light::Light() : is_on(false)
{
}

Light::Light(glm::vec3 pos, glm::vec3 dir, glm::vec4 col, LightType ltype, float cone_out, float cone_in) :
    position(pos), direction(dir), color(col), type(ltype), cone_outer(cone_out), cone_inner(cone_in), is_on(true)
{
}

void Light::Initialize()
{
  shadow_map.Initialize(type == Directional ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP, true);
}

Light& Light::operator=(const Light& rhs)
{
  position = rhs.position;
  direction = rhs.direction;
  color = rhs.color;
  cone_outer = rhs.cone_outer;
  cone_inner = rhs.cone_inner;
  type = rhs.type;
  is_on = rhs.is_on;
  
  // shadow_map is explicitly left out on purpose
  return *this;
}

void Light::BindShadowMap(unsigned light_num)
{
  if(type == Directional)
    shadow_map.BindTex(light_num * 2);
  else
    shadow_map.BindTex(light_num * 2 + 1);
}

void Light::UpdateProjections(float near_plane, float far_plane, float aspect_ratio)
{
  ortho_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  persp_projection = glm::perspective(glm::radians(90.0f), aspect_ratio, near_plane, far_plane);
}

void Light::UpdateUniforms(Shader** shaders, unsigned light_num, unsigned num_shaders)
{
  std::string num = std::to_string(light_num);
  GLint loc;
  
  for(unsigned i = 0; i < num_shaders; ++i)
  {
    shaders[i]->Activate();
    loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].is_on").c_str());
    
    // does this shader have lights[i]?
    if(loc != -1)
    {
      glUniform1i(loc, is_on);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].pos").c_str());
      glUniform3f(loc, position.x, position.y, position.z);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].dir").c_str());
      glUniform3f(loc, direction.x, direction.y, direction.z);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].color").c_str());
      glUniform4f(loc, color.x, color.y, color.z, color.w);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].cone_outer").c_str());
      glUniform1f(loc, cone_outer);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].cone_inner").c_str());
      glUniform1f(loc, cone_inner);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].type").c_str());
      glUniform1i(loc, type);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].shadow_map_2d").c_str());
      glUniform1i(loc, light_num * 2);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].shadow_map_cube").c_str());
      glUniform1i(loc, light_num * 2 + 1);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].intensity_quadratic").c_str());
      glUniform1f(loc, intensity_quadratic);
      
      loc = glGetUniformLocation(shaders[i]->id, ("lights[" + num + "].intensity_linear").c_str());
      glUniform1f(loc, intensity_linear);
      
      loc = glGetUniformLocation(shaders[i]->id, "far_plane");
      glUniform1f(loc, 100.0f);
      
      glm::mat4 light_matrix = ortho_projection * glm::lookAt(position, position + direction, glm::vec3( 0.0f,1.0f, 0.0f));
      loc = glGetUniformLocation(shaders[i]->id, ("light_space_matrix[" + num + "]").c_str());
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(light_matrix));
    }
  }
}

#include <imGuIZMO.quat/imGuIZMOquat.h>

bool Light::ImGuiDraw()
{
  bool changed = false;
  
  ImGui::PushID(this);
  
  changed |= ImGui::Checkbox("Active", &is_on);
  
  if(is_on)
  {
    int temp = int(type);
    if(ImGui::Combo("Type", &temp, LightTypeNames, 3))
    {
      if((LightType(temp) == Directional || type == Directional) && type != LightType(temp))
        shadow_map.ChangeType(LightType(temp) == Directional ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP);
      changed = true;
      type = LightType(temp);
    }
    
    changed |= ImGui::ColorEdit4("Color", &color.x);
    
    if(type != Directional)
    {
      changed |= ImGui::SliderFloat("Quadratic Intensity", &intensity_quadratic, 0.0f, 1.0f);
      changed |= ImGui::SliderFloat("Linear Intensity", &intensity_linear, 0.0f, 1.0f);
    }
    
    changed |= ImGui::DragFloat3("Position", &position.x, 0.1f);
    if(ImGui::IsItemHovered() && type == Directional)
      ImGui::SetTooltip("Only affects the Shadow Map");
    
    if(type != Point)
    {
      vec3 direction_mod(direction.x, direction.y, direction.z);
      if(ImGui::gizmo3D("Direction", direction_mod))
      {
        direction = glm::vec3(direction_mod.x, direction_mod.y, direction_mod.z);
        changed = true;
      }
    }
    
    if(type == Spot)
    {
      changed |= ImGui::SliderFloat("Outer Cone", &cone_outer, 0.0f, 1.0f);
      changed |= ImGui::SliderFloat("Inner Cone", &cone_inner, 0.0f, 1.0f);
    }
  }
  
  ImGui::PopID();
  
  return changed;
}
