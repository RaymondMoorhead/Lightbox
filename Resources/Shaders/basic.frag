#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 face_normal;
in vec3 curPos;
in vec3 light_space_pos[10];

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec3 camPos;
uniform vec4 ambient;
uniform float far_plane;

struct Light
{
  vec3 pos;
  vec3 dir;
  vec4 color;
  float cone_outer;
  float cone_inner;
  int type;
  bool is_on;
  float intensity_quadratic;
  float intensity_linear;
  samplerCube shadow_map_cube;
  sampler2D shadow_map_2d;
};

uniform Light[10] lights;

float shadowCalc(int light_index)
{
  vec3 light_to_frag;
  float dot_light_normal;
  float bias;
  float depth;
  float shadow = 0.0f;
  
  if(lights[light_index].type == 0)
  {
    light_to_frag = light_space_pos[light_index] * 0.5f + 0.5f;
    if(light_to_frag.z > 1.0f)
      light_to_frag.z = 1.0f;
    dot_light_normal = dot(lights[light_index].dir, face_normal);
    bias = max(0.005f * (1.0f - dot_light_normal), 0.005f);
    
    vec2 texel_size = 1.0f / textureSize(lights[light_index].shadow_map_2d, 0);
    for(int x = -1; x <= 1; ++x)
      for(int y = -1; y <= 1; ++y)
      {
        depth = texture(lights[light_index].shadow_map_2d, light_to_frag.xy + vec2(x, y) * texel_size).r;
        shadow += (depth + bias) < light_to_frag.z ? 0.0f : 0.11111111f;
      }
  }
  else
  {
    light_to_frag = curPos - lights[light_index].pos;
    dot_light_normal = dot(light_to_frag, face_normal);
    bias = max(0.05f * (1.0f - dot_light_normal), 0.05f);
    
    depth = texture(lights[light_index].shadow_map_cube, light_to_frag).r * far_plane;
    shadow = (depth + bias) < length(light_to_frag) ? 0.0f : 1.0f;
  }
  
  return shadow;
}

vec4 pointLight(int lightIndex)
{
  vec3 lightVec = lights[lightIndex].pos - curPos;
  float distance = length(lightVec);
  float intensity = 1.0f / (lights[lightIndex].intensity_quadratic * distance * distance + lights[lightIndex].intensity_linear * distance + 1.0f);

  vec3 normal = normalize(face_normal);
  vec3 lightDir = normalize(lightVec);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camPos - curPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lights[lightIndex].color;
}

vec4 dirLight(int lightIndex)
{
  vec3 normal = normalize(face_normal);
  vec3 lightDir = normalize(-lights[lightIndex].dir);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  if(texture(diffuse0, texCoord).a < 0.1)
    discard;
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camPos - curPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lights[lightIndex].color;
}

vec4 spotLight(int lightIndex)
{
  vec3 normal = normalize(face_normal);
  vec3 lightDir = normalize(lights[lightIndex].pos - curPos);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camPos - curPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  // dropoff from distance
  vec3 lightVec = lights[lightIndex].pos - curPos;
  float distance = length(lightVec);
  float intensity = 1.0f / (lights[lightIndex].intensity_quadratic * distance * distance + lights[lightIndex].intensity_linear * distance + 1.0f);
  
  // dropoff from cone
  float angle = dot(lights[lightIndex].dir, -lightDir);
  intensity *= clamp((angle - lights[lightIndex].cone_outer) / (lights[lightIndex].cone_inner - lights[lightIndex].cone_outer), 0.0f, 1.0f);
  
  return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lights[lightIndex].color;
}

void main()
{
  // add up the lights for this fragment, start with the ambient
  vec4 color = ambient;
  float shadow;
  for(int i = 0; i < 10; ++i)
  {
    if(lights[i].is_on == true)
    {
      shadow = shadowCalc(i);
      if(lights[i].type == 0)
        color += shadow * dirLight(i);
      else if(lights[i].type == 1)
        color += shadow * pointLight(i);
      else if(lights[i].type == 2)
        color += shadow * spotLight(i);
      else
      {
        // use red as a debug color to signal an issue
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
      }
    }
  }
  
  // bring down the color to a max of 1.0f
  color.x = min(color.x, 1.0f);
  color.y = min(color.y, 1.0f);
  color.z = min(color.z, 1.0f);
  color.w = min(color.w, 1.0f);
  
  FragColor = color;
}
