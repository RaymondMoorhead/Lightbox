#include "GraphicsController.h"
#include "Object.h"
#include <stdio.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define GL_ERROR_CHECK PrintErrors_(__LINE__)

const char* ShaderTypeNames[]
{
  #define SHADER_UNIFORM1F(Unused1, Unused2)
  #define SHADER_UNIFORM2F(Unused1, Unused2, Unused3)
  #define SHADER_UNIFORM3F(Unused1, Unused2, Unused3, Unused4)
  #define SHADER_UNIFORM4F(Unused1, Unused2, Unused3, Unused4, Unused5)
  
  #define SHADER(NAME, UNUSED1, UNUSED2, UNUSED3) #NAME,
  #include "../DefineLists/ShaderTypes.inl"
  #undef SHADER
  "NumShaderTypeNames"
  
  #undef SHADER_UNIFORM1F
  #undef SHADER_UNIFORM2F
  #undef SHADER_UNIFORM3F
  #undef SHADER_UNIFORM4F
};

void GLAPIENTRY
GL_MESSAGE_CALLBACK( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  if(type == GL_DEBUG_TYPE_ERROR)
    printf("GL_MESSAGE_CALLBACK ERROR: type = 0x%x, severity = 0x%x, message = %s\n",
              type, severity, message );
}

GraphicsController::GraphicsController() : window_name_("Lightbox")
{
  for(unsigned i = 0; i < num_lights; ++i)
    lights[i].is_on = false;
}

GraphicsController::GraphicsController(unsigned screen_width, unsigned screen_height, const char* window_name) : screen_width_(screen_width), screen_height_(screen_height), window_name_(window_name)
{
  for(unsigned i = 0; i < num_lights; ++i)
    lights[i].is_on = false;
}

void GraphicsController::Initialize()
{
  printf("GraphicsController::Initialization starting\n");
  printf("\tInitializing glfw...\n");
  glfwInit();
  
  printf("\tSetting version hints...\n");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major = 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor = 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  printf("\tCreating window...\n");
  window = glfwCreateWindow(screen_width_, screen_height_, window_name_, nullptr, nullptr);
  
  if(window == nullptr)
  {
    printf("\t\tContext %d.%d failed, proceeding with a lessr version which will disable certain features...\n", gl_version_major, gl_version_minor);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major = 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor = 3);
    
    window = glfwCreateWindow(screen_width_, screen_height_, window_name_, nullptr, nullptr);
    if(window == nullptr)
    {
      printf("Failed to create GLFW window\n");
      glfwTerminate();
      return;
    }
  }
  
  glfwMakeContextCurrent(window);
  
  printf("\tLoading GL with glad...\n");
  gladLoadGL();
  
  printf("\tPrimary initialization complete, loading additional assets...\n");
  
  glViewport(0, 0, screen_width_, screen_height_);
  camera = new Camera(screen_width_, screen_height_, glm::vec3(0.0f, 0.0f, 2.0f));
  
  // SHADER CODE
  
  GLuint cur_shader_id;
  #define SHADER(Name, VERT, FRAG, GEOM)                             \
    shaders_[Name] = new Shader(#Name,                               \
                                (VERT[0] != '\0' ? VERT : nullptr),  \
                                (FRAG[0] != '\0' ? FRAG : nullptr),  \
                                (GEOM[0] != '\0' ? GEOM : nullptr)); \
    shaders_[Name]->Activate();                                      \
    cur_shader_id = shaders_[Name]->id;
  
  #define SHADER_UNIFORM1F(Name, Value)\
    glUniform1f(glGetUniformLocation(cur_shader_id, #Name), Value);
  #define SHADER_UNIFORM2F(Name, Value1, Value2)\
    glUniform2f(glGetUniformLocation(cur_shader_id, #Name), Value1, Value2);
  #define SHADER_UNIFORM3F(Name, Value1, Value2, Value3)\
    glUniform3f(glGetUniformLocation(cur_shader_id, #Name), Value1, Value2, Value3);
  #define SHADER_UNIFORM4F(Name, Value1, Value2, Value3, Value4)\
    glUniform4f(glGetUniformLocation(cur_shader_id, #Name), Value1, Value2, Value3, Value4);
  
  #include "../DefineLists/ShaderTypes.inl"
  
  #undef SHADER
  #undef SHADER_UNIFORM1F
  #undef SHADER_UNIFORM2F
  #undef SHADER_UNIFORM3F
  #undef SHADER_UNIFORM4F
  
  GL_ERROR_CHECK;
  
  // ADDITIONAL FLAGS
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);
  
  if(gl_version_major >= 4 && gl_version_minor >= 3)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GL_MESSAGE_CALLBACK, 0);
  }
  
  // LIGHTS
  for(unsigned i = 0; i < num_lights; ++i)
    lights[i].Initialize();
    
  Light::UpdateProjections(0.1f, 100.0f, float(screen_width_) / float(screen_height_));
  ReloadLightUniforms();
  
  GL_ERROR_CHECK;
  
  // IMGUI
  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsDark();
  
  GL_ERROR_CHECK;
  
  printf("GraphicsController::Initialization Complete\n");
}

bool GraphicsController::Update()
{
  cur_time_ = glfwGetTime();
  time_diff_ = cur_time_ - prev_time_;
  ++counter_;
  if(time_diff_ >= 1.0 / 30.0)
  {
    std::string fps = std::to_string((1.0 / time_diff_) * counter_);
    std::string ms = std::to_string((time_diff_ / counter_) * 1000.0);
    std::string new_title = std::string(window_name_) + " | " + fps + " FPS | " + ms + " ms";
    glfwSetWindowTitle(window, new_title.c_str());
    prev_time_ = cur_time_;
    counter_ = 0;
  }
  
  glViewport(0, 0, screen_width_, screen_height_);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  camera->Inputs(window);
  camera->UpdateMatrix(45.0f, 0.1f, 100.0f);
  
  for(unsigned i = 0; i < num_lights; ++i)
    lights[i].shadow_map.DrawMap(this, objects_, &lights[i]);
  
  glViewport(0, 0, screen_width_, screen_height_);

  for(auto it = objects_.begin(); it != objects_.end(); ++it)
  {
    it->second->shader->Activate();
    for(unsigned i = 0; i < num_lights; ++i)
      lights[i].BindShadowMap(i);
    it->second->Draw(*camera);
  }
  
  UpdateImgui();
  
  glfwSwapBuffers(window);
  
  GL_ERROR_CHECK;
  
  glfwPollEvents();
  
  return !glfwWindowShouldClose(window);
}

#include <imGuIZMO.quat/imGuIZMOquat.h>
extern unsigned CONTROL_LIGHT;
extern Object* ROTATE_OBJECT;
extern glm::vec3 ROTATE_AXIS;
extern float ROTATE_SPEED;

void GraphicsController::UpdateImgui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  
  if(ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("Edit"))
    {
      if(ImGui::BeginMenu("Objects"))
      {        
        for(auto it = objects_.begin(); it != objects_.end(); ++it)
        {
          if(ImGui::BeginMenu(it->first.c_str()))
          {
            bool rotate_this_object = ROTATE_OBJECT == it->second;
            if(ImGui::Checkbox("Rotate", &rotate_this_object))
              ROTATE_OBJECT = rotate_this_object ? it->second : nullptr;
            if(ROTATE_OBJECT)
            {
              ImGui::SliderFloat("Rotation Speed", &ROTATE_SPEED, 0.01f, 1.0f);
              vec3 axis(ROTATE_AXIS.x, ROTATE_AXIS.y, ROTATE_AXIS.z);
              if(ImGui::gizmo3D("Rotation Axis", axis))
                ROTATE_AXIS = glm::vec3(axis.x, axis.y, axis.z);
            }
            
            it->second->ImGuiDraw();
            ImGui::EndMenu();
          }
        }
        ImGui::EndMenu();
      }
      if(ImGui::BeginMenu("Lights"))
      {
        for(unsigned i = 0; i < num_lights; ++i)
        {
          if(ImGui::BeginMenu(std::to_string(i).c_str()))
          {
            bool direct_control = CONTROL_LIGHT == i;
            if(ImGui::Checkbox("Control Directly", &direct_control))
            {
              if(direct_control)
                CONTROL_LIGHT = i;
              else
                CONTROL_LIGHT = -1;
            }
            if(ImGui::IsItemHovered())
              ImGui::SetTooltip("Attach the light to camera for fine positioning constrol");
            
            if(lights[i].ImGuiDraw())
              ReloadLightUniforms(i);
            ImGui::EndMenu();
          }
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    
    ImGui::EndMainMenuBar();
  }
  
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsController::Exit()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
  for(auto it = objects_.begin(); it != objects_.end(); ++it)
    delete it->second;
  
  for(auto it = models_.begin(); it != models_.end(); ++it)
    delete it->second;
  
  for(int i = 0; i < int(NumShaderTypes); ++i)
  {
    shaders_[i]->Delete();
    delete shaders_[i];
  }
    
  glfwDestroyWindow(window);
  glfwTerminate();
  
  delete camera;
  
  printf("GraphicsController::Exit Complete\n");
}

Object* GraphicsController::CreateObject(const char* name, ShaderType shader, const char* model_name)
{
  Object* result;
  if(result = GetObject(name))
    return result;
  else
  {
    Model* model = CreateModel(model_name != nullptr ? model_name : name);
    if(model == nullptr)
    {
      printf("Unable to create Object\n");
      return nullptr;
    }
    return objects_.emplace(name, new Object(GetShader(shader), model)).first->second;
  }
}

void GraphicsController::DeleteObject(const char* name)
{
  auto found = objects_.find(name);
  if(found != objects_.end())
  {
    delete found->second;
    objects_.erase(found);
  }
}

Object* GraphicsController::GetObject(const char* name)
{
  auto found = objects_.find(name);
  if(found != objects_.end())
    return found->second;
  else
    return nullptr;
}

Model* GraphicsController::CreateModel(const char* name, unsigned instances, std::vector<glm::mat4> instance_matrices)
{
  std::string filepath = name;
  filepath = "../Resources/Models/" + filepath + "/scene.gltf";
  Model* result;
  if(result = GetModel(name))
    return result;
  else
  {
    try
    {
      return models_.emplace(name, new Model(filepath.c_str(), instances, instance_matrices)).first->second;
    }
    catch(const char* error)
    {
      printf("Model creation failed with error \"%s\", path is %s\n", error, filepath.c_str());
      return nullptr;
    }
  }
}

Model* GraphicsController::GetModel(const char* name)
{
  auto found = models_.find(name);
  if(found != models_.end())
    return found->second;
  else
    return nullptr;
}

Shader* GraphicsController::GetShader(ShaderType shader_type)
{
  return shaders_[shader_type];
}

void GraphicsController::ReloadLightUniforms(unsigned light_num)
{
  if(light_num == unsigned(-1))
    for(unsigned i = 0; i < num_lights; ++i)
      lights[i].UpdateUniforms(shaders_, i, NumShaderTypes);
  else
    lights[light_num].UpdateUniforms(shaders_, light_num, NumShaderTypes);
  
  GL_ERROR_CHECK;
}

bool GraphicsController::PrintErrors_(unsigned line_num)
{
  GLenum err;
  bool any_errors = false;
  while((err = glGetError()) != 0)
  {
    any_errors = true;
    printf("%s (%d) Caught glError %d\n", __FILENAME__, line_num, err);
  }
  return any_errors;
}