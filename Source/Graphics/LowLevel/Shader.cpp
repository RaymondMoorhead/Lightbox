#include "Shader.h"
#include <fstream>  // ifstream
#include <stdio.h>  // printf

// HELPER FUNCTIONS

void CheckShaderCompilation(GLuint shader, const char* name)
{
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
  	GLint maxLength = 0;
  	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
  
  	// The maxLength includes the NULL character
  	GLchar* errorLog = (GLchar*)malloc(maxLength);
  	glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
    printf("Shader %s(%u) encountered an error:\n%s\n", name, shader, errorLog);
    
  	// Exit with failure.
  	glDeleteShader(shader); // Don't leak the shader.
    free(errorLog);
  }
}

std::string GetFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in.is_open())
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
  else
    printf("GetFileContents failed to open file %s\n", filename);
	return std::string();
}

// MAIN FUNCTIONS

Shader::Shader(const char* name_, const char* vertex_file, const char* fragment_file, const char* geometry_file) : name(name_)
{
  std::string shader_file_path = "../Resources/Shaders/";
  
  GLuint vertex;
  if(vertex_file != nullptr)
  {
    std::string vertex_file_path = shader_file_path + vertex_file;
    std::string vertex_code = GetFileContents(vertex_file_path.c_str());
    const char* vertex_source = vertex_code.c_str();
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, nullptr);
    glCompileShader(vertex);
    CheckShaderCompilation(vertex, vertex_file);
  }
  
  GLuint fragment;
  if(fragment_file != nullptr)
  {
    std::string fragment_file_path = shader_file_path + fragment_file;
    std::string fragment_code = GetFileContents(fragment_file_path.c_str());
    const char* fragment_source = fragment_code.c_str();
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, nullptr);
    glCompileShader(fragment);
    CheckShaderCompilation(fragment, fragment_file);
  }
  
  GLuint geometry;
  if(geometry_file != nullptr)
  {
    std::string geometry_file_path = shader_file_path + geometry_file;
    std::string geometry_code = GetFileContents(geometry_file_path.c_str());
    const char* geometry_source = geometry_code.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &geometry_source, nullptr);
    glCompileShader(geometry);
    CheckShaderCompilation(geometry, geometry_file);
  }
  
  id = glCreateProgram();
  if(vertex_file != nullptr)
    glAttachShader(id, vertex);
  if(fragment_file != nullptr)
    glAttachShader(id, fragment);
  if(geometry_file != nullptr)
    glAttachShader(id, geometry);
  glLinkProgram(id);
  
  if(vertex_file != nullptr)
    glDeleteShader(vertex);
  if(fragment_file != nullptr)
    glDeleteShader(fragment);
  if(geometry_file != nullptr)
    glDeleteShader(geometry);
}
  
void Shader::Activate()
{
  glUseProgram(id);
}

void Shader::Delete()
{
  glDeleteProgram(id);
}