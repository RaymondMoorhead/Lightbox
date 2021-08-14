#include "Object.h"
#include <imgui/imgui.h>
#include <imGuIZMO.quat/imGuIZMOquat.h>

Object::Object(Shader* shader_, Model* model) : shader(shader_), model_(model)
{
}

void Object::Draw(Camera camera, Shader* shader_)
{
  model_->Draw((shader_ != nullptr ? *shader_ : *shader), camera, translation, rotation, scale);
}

void Object::ImGuiDraw()
{
  ImGui::PushID(this);
  
  ImGui::DragFloat3("Translation", &translation.x, 0.1f);
  ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.01f, 100.0f);
  
  quat view_rotation(rotation.w, rotation.x, rotation.y, rotation.z);
  if(ImGui::gizmo3D("Rotation", view_rotation, imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin))
    rotation = glm::quat(view_rotation.w, view_rotation.x, view_rotation.y, view_rotation.z);
  
  ImGui::PopID();
}