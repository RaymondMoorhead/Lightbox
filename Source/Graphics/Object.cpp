#include "Object.h"

Object::Object(Shader* shader_, Model* model) : shader(shader_), model_(model)
{
}

void Object::Draw(Camera camera, Shader* shader_)
{
  model_->Draw((shader_ != nullptr ? *shader_ : *shader), camera, translation, rotation, scale);
}