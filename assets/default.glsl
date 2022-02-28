#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

struct VertexOutput {
  vec4 Color;
  vec2 TexCoord;
  float TilingFactor;
};

layout(location = 0) out VertexOutput Output;

void main() {
  Output.Color = a_Color;

  // Set vertex position
  gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

struct VertexOutput {
  vec4 Color;
  vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main() {
  vec4 texColor = Input.Color;
  color = texColor;
}
