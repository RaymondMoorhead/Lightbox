#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 face_normal;
out vec3 color;
out vec2 texCoord;
out vec3 curPos;
out vec3 curPosRaw;
out vec3 light_space_pos[10];
flat out int has_normal_map;
out vec3 cam_pos;

uniform mat4 camMatrix;
uniform mat4 light_space_matrix[10];
uniform int use_normal_map;
uniform vec3 camPos;

in DATA
{
  vec3 face_normal;
  vec3 color;
  vec2 texCoord;
  vec3 curPos;
  mat4 model;
} data_in[];

// default
void main()
{
  mat3 TBN = mat3(1.0f);

  if(use_normal_map != 0)
  {
    // calculate TBN for normal map
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;
    
    float invDot = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);
    
    vec3 tangent = vec3(invDot * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDot * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));
    
    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));
    
    TBN = mat3(T, B, N);
    TBN = transpose(TBN);
  }

  gl_Position = camMatrix * gl_in[0].gl_Position;
  face_normal = data_in[0].face_normal;
  color = data_in[0].color;
  texCoord = data_in[0].texCoord;
  has_normal_map = use_normal_map;
  curPos = TBN * data_in[0].curPos;
  curPosRaw = data_in[0].curPos;
  cam_pos = TBN * camPos;
  for(int i = 0; i < 10; ++i)
    light_space_pos[i] = (light_space_matrix[i] * vec4(data_in[0].curPos, 1.0f)).xyz;
  EmitVertex(); // we're done with this vertex
  
  gl_Position = camMatrix * gl_in[1].gl_Position;
  face_normal = data_in[1].face_normal;
  color = data_in[1].color;
  texCoord = data_in[1].texCoord;
  has_normal_map = use_normal_map;
  curPos = TBN * data_in[1].curPos;
  curPosRaw = data_in[1].curPos;
  cam_pos = TBN * camPos;
  for(int i = 0; i < 10; ++i)
    light_space_pos[i] = (light_space_matrix[i] * vec4(data_in[1].curPos, 1.0f)).xyz;
  EmitVertex(); // we're done with this vertex
  
  gl_Position = camMatrix * gl_in[2].gl_Position;
  face_normal = data_in[2].face_normal;
  color = data_in[2].color;
  texCoord = data_in[2].texCoord;
  has_normal_map = use_normal_map;
  curPos = TBN * data_in[2].curPos;
  curPosRaw = data_in[2].curPos;
  cam_pos = TBN * camPos;
  for(int i = 0; i < 10; ++i)
    light_space_pos[i] = (light_space_matrix[i] * vec4(data_in[2].curPos, 1.0f)).xyz;
  EmitVertex(); // we're done with this vertex
  
  EndPrimitive(); // we're done with this primitive
}