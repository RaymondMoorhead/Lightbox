#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

struct Camera
{
  Camera(int width, int height, glm::vec3 position);
  
  void UpdateMatrix(float fov_deg, float near, float far);
  void Matrix(Shader& shader, const char* uniform);
  void Inputs(GLFWwindow* window);
  
  bool first_click;
  
  glm::vec3 position;
  glm::vec3 orientation;
  glm::vec3 up;
  glm::mat4 camera;
  
  int width, height;
  
  float speed;
  float sensitivity ;
};