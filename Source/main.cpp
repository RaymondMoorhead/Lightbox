#include "Graphics/GraphicsController.h"
#include "Graphics/Object.h"

#include <stdio.h>

int main(int argc, char** argv)
{
  GraphicsController g_control;
  
  // DEBUG LIGHTS
  printf("main loading lights...\n");
  g_control.lights[1] = Light(glm::vec3(0.0f, 0.0f, 2.0f),
                                   glm::vec3(0.0f, 0.0f, -1.0f),
                                   glm::vec4(0.5f, 1.0f, 0.5f, 1.0f),
                                   Spot, .98f, .99f);
  g_control.lights[2] = Light(glm::vec3(0.0f, 2.0f, 0.0f),
                                   glm::vec3(-1.0f, -1.0f, 0.0f),
                                   glm::vec4(1.0f, 0.5f, 0.5f, 1.0f),
                                   Point);
  g_control.lights[3] = Light(glm::vec3(0.0f, 5.0f, 0.0f),
                                   glm::vec3(-1.0f, -1.0f, 0.0f),
                                   glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
                                   Directional);
  
  g_control.Initialize();
  
  // DEBUG MODELS
  printf("main loading models...\n");
  g_control.CreateObject("ground", Basic);
  Object* statue = g_control.CreateObject("statue", Basic);

  bool flashlight_toggle_in_progress[g_control.num_lights] = {false};
  
  printf("main beginning loop\n");
  float rotate = 0.0f;
  while(g_control.Update())
  {
    rotate += 360.0f * 0.25f * (1.0f / 60.0f);
    statue->rotation = glm::angleAxis(glm::radians(rotate), glm::vec3(0.f, 1.f, 0.f));
    if(statue->rotation.z >= 360.0f)
      statue->rotation.z -= 360.0f;
    
    for(int i = 0; i < g_control.num_lights; ++i)
    {
      if(glfwGetKey(g_control.window, GLFW_KEY_0 + i) == GLFW_PRESS && !flashlight_toggle_in_progress[i])
      {
        g_control.lights[i].is_on = !g_control.lights[i].is_on;
        g_control.ReloadLightUniforms(i);
        flashlight_toggle_in_progress[i] = true;
      }
      else if(glfwGetKey(g_control.window, GLFW_KEY_0 + i) == GLFW_RELEASE)
        flashlight_toggle_in_progress[i] = false;
    }
    
    g_control.lights[1].position = g_control.camera->position + g_control.camera->orientation;
    g_control.lights[1].direction =  g_control.camera->orientation;
    g_control.ReloadLightUniforms(1);
  }
  
  g_control.Exit();
  
  return 0;
}