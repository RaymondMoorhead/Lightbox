#pragma once

#include "LowLevel/Shader.h"
#include "LowLevel/Model.h"
#include "LowLevel/Camera.h"

class Object
{
  public:
    Object(Shader* shader, Model* model);
    void Draw(Camera camera, Shader* shader = nullptr);
    
    void ImGuiDraw();
    
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    Shader* shader;
  
  private:
    Model* model_;
    bool use_normal_map_ = true;
    bool use_displacement_map_ = true;
    float displacement_mod_ = 0.05f;
};