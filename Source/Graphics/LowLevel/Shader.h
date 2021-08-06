#pragma once

#include <glad/glad.h>
#include <stdlib.h> // malloc, free
#include <string>

// helper function
std::string GetFileContents(const char* filename);

struct Shader
{
  GLuint id;
  const char* const name;
  Shader(const char* name, const char* vertex_file, const char* fragment_file, const char* geometry_file = nullptr);
  
  void Activate();
  void Delete();
};