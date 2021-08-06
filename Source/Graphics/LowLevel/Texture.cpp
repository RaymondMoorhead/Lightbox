#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdio.h>

Texture::Texture(const char* image_file, const char* tex_type, GLuint slot) : type(tex_type)
{
  int img_width, img_height, img_num_color_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* img_bytes = stbi_load(image_file, &img_width, &img_height, &img_num_color_channels, 0);
  
  glGenTextures(1, &id);
  glActiveTexture(GL_TEXTURE0 + slot);
  unit = slot;
  glBindTexture(GL_TEXTURE_2D, id);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  //float img_border_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
  //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, img_border_color);
  
  GLenum format;
  if(img_num_color_channels == 4)
    format = GL_RGBA;
  else if(img_num_color_channels == 3)
    format = GL_RGB;
  else if(img_num_color_channels == 1)
    format = GL_RED;
  else
    printf("Texture::Texture supplied an incompatible number of channels: %d\n", img_num_color_channels);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, img_bytes);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  stbi_image_free(img_bytes);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetUniform(Shader& shader, const char* uniform_name, GLuint unit)
{
  GLuint texture_uniform = glGetUniformLocation(shader.id, uniform_name);
  
  shader.Activate();
  
  glUniform1i(texture_uniform, unit);
}
  
void Texture::Bind()
{
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
  glDeleteTextures(1, &id);
}