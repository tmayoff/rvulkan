#version 450 core

layout(location = 0) out vec4 color;

struct VertexOutput {
  vec4 Color;
};

layout(location = 0) in VertexOutput Input;

void main() {
  vec4 texColor = Input.Color;
  color = texColor;
}
