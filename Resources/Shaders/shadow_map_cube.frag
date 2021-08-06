#version 330 core

in vec4 curPos;

uniform int lightType;
uniform vec3 lightPos;
uniform float far_plane;

void main()
{
  if(lightType != 0)
  {
    float length = length(curPos.xyz - lightPos);
    length /= far_plane;
    gl_FragDepth = length;
  }
}