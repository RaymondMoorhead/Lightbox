#include "Graphics/GraphicsController.h"
#include "Graphics/Object.h"

#include <stdio.h>

unsigned CONTROL_LIGHT = -1;

Object* ROTATE_OBJECT = nullptr;
glm::vec3 ROTATE_AXIS = glm::vec3(0.f, 1.f, 0.f);
float ROTATE_SPEED = 0.25f;

int main(int argc, char** argv)
{
  GraphicsController g_control;
  
  // DEBUG LIGHTS
  printf("main loading lights...\n");
  g_control.lights[0] = Light(glm::vec3(0.0f, 2.0f, 2.0f),
                                   glm::vec3(0.0f, -1.0f, -1.0f),
                                   glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
                                   Spot, .90f, 1.0f);
  g_control.lights[1] = Light(glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(-1.0f, -1.0f, 0.0f),
                                   glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
                                   Point);
  g_control.lights[2] = Light(glm::vec3(0.0f, 5.0f, 0.0f),
                                   glm::vec3(-1.0f, -1.0f, 0.0f),
                                   glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
                                   Directional);
  
  g_control.Initialize();
  
  // DEBUG MODELS
  printf("main loading models...\n");
  g_control.CreateObject("ground", Basic);
  g_control.CreateObject("statue", Basic);
  g_control.CreateObject("plane", Basic)->translation.y = -1.0f;

  bool light_toggle_in_progress[g_control.num_lights] = {false};
  
  printf("main beginning loop\n");
  
  Object* last_rotated_object = nullptr;
  float rotate = 0.0f;
  while(g_control.Update())
  {
    if(ROTATE_OBJECT)
    {
      if(last_rotated_object != ROTATE_OBJECT)
      {
        last_rotated_object = ROTATE_OBJECT;
        rotate = 0.0f;
      }
      constexpr float PI_TIMES_2 = 6.28318530718f;
      rotate += PI_TIMES_2 * ROTATE_SPEED * (1.0f / 60.0f);
      ROTATE_OBJECT->rotation = glm::angleAxis(rotate, ROTATE_AXIS);
      if(rotate >= PI_TIMES_2)
        rotate -= PI_TIMES_2;
    }
    
    for(int i = 0; i < g_control.num_lights; ++i)
    {
      if(glfwGetKey(g_control.window, GLFW_KEY_0 + i) == GLFW_PRESS && !light_toggle_in_progress[i])
      {
        g_control.lights[i].is_on = !g_control.lights[i].is_on;
        g_control.ReloadLightUniforms(i);
        light_toggle_in_progress[i] = true;
      }
      else if(glfwGetKey(g_control.window, GLFW_KEY_0 + i) == GLFW_RELEASE)
        light_toggle_in_progress[i] = false;
    }
    
    if(CONTROL_LIGHT < g_control.num_lights)
    {
      g_control.lights[CONTROL_LIGHT].position = g_control.camera->position;
      g_control.lights[CONTROL_LIGHT].direction =  g_control.camera->orientation;
      g_control.ReloadLightUniforms(CONTROL_LIGHT);
    }
  }
  
  g_control.Exit();
  
  return 0;
}