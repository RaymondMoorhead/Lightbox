#version 330 core

out vec4 FragColor;

in vec3 face_normal;
in vec3 color;
in vec2 texCoord;
in vec3 curTBNPos;
in vec3 curPosRaw;
in vec3 light_space_pos[10];
in vec3 light_tbn_pos[10];
flat in int has_normal_map;
in vec3 camTBNPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;
uniform bool use_displacement_map;
uniform float displacement_mod;
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

// final used normal, varies if normal map is used
vec3 normal = vec3(0.0f, 1.0f, 0.0f);

// final used UVs, varies if parallax occlusion map is used
vec2 uv = vec2(0.0f, 0.0f);

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
    light_to_frag = curPosRaw - lights[light_index].pos;
    dot_light_normal = dot(light_to_frag, face_normal);
    bias = max(0.05f * (1.0f - dot_light_normal), 0.05f);
    
    depth = texture(lights[light_index].shadow_map_cube, light_to_frag).r * far_plane;
    shadow = (depth + bias) < length(light_to_frag) ? 0.0f : 1.0f;
  }
  
  return shadow;
}

vec4 pointLight(int lightIndex)
{
  vec3 lightVec = light_tbn_pos[lightIndex] - curTBNPos;
  float distance = length(lightVec);
  float intensity = 1.0f / (lights[lightIndex].intensity_quadratic * distance * distance + lights[lightIndex].intensity_linear * distance + 1.0f);

  vec3 lightDir = normalize(lightVec);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camTBNPos - curTBNPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  return (texture(diffuse0, uv) * (diffuse * intensity + ambient) + texture(specular0, uv).r * specular * intensity) * lights[lightIndex].color;
}

vec4 dirLight(int lightIndex)
{
  vec3 lightDir = normalize(-lights[lightIndex].dir);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  if(texture(diffuse0, uv).a < 0.1)
    discard;
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camTBNPos - curTBNPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  return (texture(diffuse0, uv) * (diffuse + ambient) + texture(specular0, uv).r * specular) * lights[lightIndex].color;
}

vec4 spotLight(int lightIndex)
{
  vec3 lightDir = normalize(light_tbn_pos[lightIndex] - curTBNPos);
  
  float ambient = 0.0f;
  float diffuse = max(dot(normal, lightDir), 0.0f);
  
  float specular = 0.0f;
  if(diffuse != 0.0f)
  {
    float specular_base = 0.5f;
    vec3 view_dir = normalize(camTBNPos - curTBNPos);
    vec3 halfway_vec = normalize(view_dir + lightDir);
    float specular_amount = pow(max(dot(normal, halfway_vec), 0.0f), 16);
    specular = specular_base * specular_amount;
  }
  
  // dropoff from distance
  vec3 lightVec = light_tbn_pos[lightIndex] - curTBNPos;
  float distance = length(lightVec);
  float intensity = 1.0f / (lights[lightIndex].intensity_quadratic * distance * distance + lights[lightIndex].intensity_linear * distance + 1.0f);
  
  // dropoff from cone
  float angle = dot(lights[lightIndex].dir, -lightDir);
  intensity *= clamp((angle - lights[lightIndex].cone_outer) / (lights[lightIndex].cone_inner - lights[lightIndex].cone_outer), 0.0f, 1.0f);
  
  return (texture(diffuse0, uv) * (diffuse * intensity + ambient) + texture(specular0, uv).r * specular * intensity) * lights[lightIndex].color;
}

void main()
{
  uv = texCoord;

  // apply parallax occlusion, if displacement map exists
  if(use_displacement_map)
  {
    // parallax occlusion mapping quality
    vec3 viewDir = normalize(camTBNPos - curTBNPos);
    const float minLayers = 8.0f;
    const float maxLayers = 64.0f;
    float numLayers = mix(maxLayers, minLayers, abs(dot(face_normal, viewDir)));
    float layerDepth = 1.0f / numLayers;
    float curLayerDepth = 0.0f;
    
    vec2 S = viewDir.xy / viewDir.z * displacement_mod;
    vec2 deltaUV = S / numLayers;
    
    float curDepthMapValue = 1.0f - texture(displacement0, uv).r;
    
    // loop until the heightmap is 'hit'
    while(curLayerDepth < curDepthMapValue)
    {
      uv -= deltaUV;
      curDepthMapValue = 1.0f - texture(displacement0, uv).r;
      curLayerDepth += layerDepth;
    }
    
    // apply occlusion, interpolate to previous value
    vec2 prevUV = uv + deltaUV;
    float afterDepth = curDepthMapValue - curLayerDepth;
    float beforeDepth = 1.0f - texture(displacement0, prevUV).r - curLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    uv = prevUV * weight + uv * (1.0f - weight);
    
    // discard fragment if the uv is out-of-bounds
    if(uv.x > 1.0f || uv.y > 1.0f || uv.x < 0.0f || uv.y < 0.0f)
      discard;
  }

  // apply normal map, if it exists
  if(has_normal_map != 0)
    normal = texture(normal0, uv).xyz * 2.0f - 1.0f;
  else
    normal = face_normal;
  normal = normalize(normal);

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
