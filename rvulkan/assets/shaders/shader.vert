#version 450

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Normal;

layout(push_constant) uniform constants{ mat4 render_matrix; } PushConstants;

layout(location = 0) out vec4 v_Color;

void main() {
  v_Color = a_Color;
  gl_Position = PushConstants.render_matrix * a_Position;
}
