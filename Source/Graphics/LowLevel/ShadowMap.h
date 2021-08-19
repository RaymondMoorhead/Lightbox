#pragma once

#include "FBO.h"
#include <map>

class GraphicsController;
class Object;
struct Shader;
struct Light;

struct ShadowMap : public FBO
{
  ShadowMap();
  
  void DrawMap(GraphicsController* g_control, std::map<std::string, Object*>& objects, Light* light);
  
  static const unsigned shadow_width;
  static const unsigned shadow_height;
};