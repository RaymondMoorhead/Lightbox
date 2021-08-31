#include "Camera.h"

Camera::Camera(int width_, int height_, glm::vec3 position_) :
  first_click(true), position(position_), orientation(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f),
  camera(1.0f), width(width_), height(height_), base_speed(1.0f), sensitivity(100.0f)
{
    cur_speed = base_speed;
}

void Camera::UpdateMatrix(float fov_deg, float near, float far)
{
  glm::mat4 view(1.0f);
  glm::mat4 projection(1.0f);
  
  view = glm::lookAt(position, position + orientation, up);
  projection = glm::perspective(glm::radians(fov_deg), float(width) / float(height), near, far);
  
  camera = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
  glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(camera));
}

void Camera::Inputs(GLFWwindow* window, float delta_time)
{
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    position += cur_speed * orientation * delta_time;
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    position += cur_speed * -glm::normalize(glm::cross(orientation, up)) * delta_time;
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    position += cur_speed * -orientation * delta_time;
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    position += cur_speed * glm::normalize(glm::cross(orientation, up)) * delta_time;

  glm::vec3 move_up = glm::rotate(orientation, glm::radians(90.0f), glm::normalize(glm::cross(orientation, up))) * delta_time;
  if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    position += cur_speed * move_up;
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    position += cur_speed * -move_up;
  
  if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cur_speed = base_speed * 4.0f;
  else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    cur_speed = base_speed;
  
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    if(first_click)
      first_click = false;
    else
    {
      double mouse_x;
      double mouse_y;
      glfwGetCursorPos(window, &mouse_x, &mouse_y);
      
      float rotation_x = -sensitivity * float(mouse_y - double(height) / 2.0) / float(height);
      float rotation_y = -sensitivity * float(mouse_x - double(height) / 2.0) / float(height);
      
      glm::vec3 new_orientation = glm::rotate(orientation, glm::radians(rotation_x), glm::normalize(glm::cross(orientation, up)));
      
      if(!(glm::angle(new_orientation, up) <= glm::radians(5.0f)) || glm::angle(new_orientation, -up) <= glm::radians(5.0f))
        orientation = new_orientation;
      
      orientation = glm::rotate(orientation, glm::radians(rotation_y), up);
    }
    glfwSetCursorPos(window, width / 2, height / 2);
  }
  else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    first_click = true;
  }
}