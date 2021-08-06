#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

const float offset_x = 1.0f / 800.0f;
const float offset_y = 1.0f / 800.0f;

vec2 offsets[9] = vec2[]
(
  vec2(-offset_x,  offset_y), vec2(0.0f,  offset_y), vec2(offset_x,   offset_y),
  vec2(-offset_x,      0.0f), vec2(0.0f,      0.0f), vec2(offset_x,       0.0f),
  vec2(-offset_x, -offset_y), vec2(0.0f, -offset_y), vec2(offset_x,  -offset_y)
);

float kernel[9] = float[]
(
  1,  1, 1,
  1, -8, 1,
  1,  1, 1
);

void main()
{
  // direct translation
  float depth = texture(screenTexture, texCoords).r;
  FragColor = vec4(vec3(depth), 1.0f);
  
  // inverse colors
  //FragColor = vec4(1.0f) - texture(screenTexture, texCoords);
  
  // monochrome
  //vec4 tex = texture(screenTexture, texCoords);
  //float average = (tex.x + tex.y + tex.z) / 3.0f;
  //FragColor = vec4(average, average, average, 1.0f);
  
  // edge detection
  //vec3 color = vec3(0.0f);
  //for(int i = 0; i < 9; ++i)
  //  color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * kernel[i];
  //FragColor = vec4(color, 1.0f);
}