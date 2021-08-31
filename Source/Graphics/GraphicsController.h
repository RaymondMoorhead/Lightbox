#pragma once

#include <vector>
#include <map>
#include <string>
#include "LowLevel/Camera.h"
#include "LowLevel/ShadowMap.h"
#include "Light.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

struct GLFWwindow;
struct Shader;
class Object;
class Model;


enum ShaderType
{
  #define SHADER_UNIFORM1F(Unused1, Unused2)
  #define SHADER_UNIFORM2F(Unused1, Unused2, Unused3)
  #define SHADER_UNIFORM3F(Unused1, Unused2, Unused3, Unused4)
  #define SHADER_UNIFORM4F(Unused1, Unused2, Unused3, Unused4, Unused5)
  
  #define SHADER(NAME, UNUSED1, UNUSED2, UNUSED3) NAME,
  #include "../DefineLists/ShaderTypes.inl"
  #undef SHADER
  NumShaderTypes
    
  #undef SHADER_UNIFORM1F
  #undef SHADER_UNIFORM2F
  #undef SHADER_UNIFORM3F
  #undef SHADER_UNIFORM4F
};

extern const char* ShaderTypeNames[];

class GraphicsController
{
  public:
    GraphicsController();
    GraphicsController(unsigned screen_width, unsigned screen_height, const char* window_name);
  
    void Initialize();
    bool Update(); // returns true if we should continue, false if we should close
    void UpdateImgui();
    void Exit();
    
    // won't create duplicate objects
    // model_name is only used if the model's name differs from the object's
    // can onyl create non-instanced models if the model doesn't exist
    Object* CreateObject(const char* name, ShaderType shader, const char* model_name = nullptr);
    void DeleteObject(const char* name);
    Object* GetObject(const char* name);
    
    // won't create duplicate models
    Model* CreateModel(const char* name, unsigned instances = 1, std::vector<glm::mat4> instance_matrices = std::vector<glm::mat4>());
    Model* GetModel(const char* name);
    
    Shader* GetShader(ShaderType shader_type);
    
    double GetDeltaTime();
  
    GLFWwindow* window = nullptr;
    Camera* camera;
    
    void ReloadLightUniforms(unsigned light_num = unsigned(-1));
    
    static constexpr unsigned num_lights = 10;
    Light lights[num_lights];
    
  private:
  
    void AddHardCodedModels_();
    bool PrintErrors_(unsigned line_num);
  
    unsigned screen_width_ = 1024;
    unsigned screen_height_ = 1024;
    const char* window_name_;
    std::map<std::string, Object*> objects_;
    
    std::map<std::string, Model*> models_;
    Shader* shaders_[NumShaderTypes];
    
    // fps tracking
    double prev_time_ = 0.0;
    double cur_time_ = 0.0;
    double time_diff_ = 0.0;
    unsigned int counter_ = 0;
    
    // version, for extra features
    int gl_version_major_, gl_version_minor_;
    
    // used in imgui object loading
    static constexpr size_t model_name_size_ = 30;
    char model_name_[model_name_size_] = {'\0'};
};