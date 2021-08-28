#include "Object.h"
#include <imgui/imgui.h>
#include <imGuIZMO.quat/imGuIZMOquat.h>

Object::Object(Shader* shader_, Model* model) : shader(shader_), model_(model)
{
}

void Object::Draw(Camera camera, Shader* shader_)
{
  shader->Activate();
  glUniform1f(glGetUniformLocation(shader->id, "displacement_mod"), displacement_mod_);
  
  model_->Draw((shader_ != nullptr ? *shader_ : *shader), camera, translation, rotation, scale, use_normal_map_, use_displacement_map_);
}

void Object::ImGuiDraw()
{
  ImGui::PushID(this);
  
  if(model_->HasNormalMap())
    ImGui::Checkbox("Normal Map", &use_normal_map_);
  
  if(model_->HasDisplacementMap())
  {
    ImGui::Checkbox("Displacement Map", &use_displacement_map_);
    ImGui::SliderFloat("Displacement Modifier", &displacement_mod_, 0.0f, 0.1f);
  }
  
  ImGui::DragFloat3("Translation", &translation.x, 0.1f);
  ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.01f, 100.0f);
  
  quat view_rotation(rotation.w, rotation.x, rotation.y, rotation.z);
  ImGui::Text("(%f, %f, %f, %f)", view_rotation.w, view_rotation.x, view_rotation.y, view_rotation.z);
  if(ImGui::gizmo3D("Rotation", view_rotation, imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin))
    rotation = glm::quat(view_rotation.w, view_rotation.x, view_rotation.y, view_rotation.z);
  
  ImGui::PopID();
}