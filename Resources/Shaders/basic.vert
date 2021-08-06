#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 face_normal;
out vec3 color;
out vec2 texCoord;
out vec3 curPos;
out vec3 light_space_pos[10];

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 light_space_matrix[10];

void main()
{
  face_normal = vec3(model * rotation * vec4(aNormal, 0.0f));
  color = aColor;
  texCoord = mat2(0.0f, -1.0f, 1.0f, 0.0f) * aTex;
  curPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
  gl_Position = camMatrix * vec4(curPos, 1.0f);
  
  for(int i = 0; i < 10; ++i)
    light_space_pos[i] = (light_space_matrix[i] * vec4(curPos, 1.0f)).xyz;
}
