#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 face_normal;
out vec3 color;
out vec2 texCoord;
out vec3 curPos;
out vec3 light_space_[10];

out DATA
{
  vec3 face_normal;
  vec3 color;
  vec2 texCoord;
  vec3 curPos;
  mat4 model;
} data_out;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
  data_out.model = model * translation * rotation * scale;
  data_out.face_normal = vec3(data_out.model * vec4(aNormal, 0.0f));
  data_out.color = aColor;
  data_out.texCoord = mat2(0.0f, -1.0f, 1.0f, 0.0f) * aTex;
  data_out.curPos = vec3(data_out.model * vec4(aPos, 1.0f));
  gl_Position = vec4(data_out.curPos, 1.0f);
}
