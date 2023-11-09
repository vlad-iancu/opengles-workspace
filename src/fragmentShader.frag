#version 300 es
precision mediump float;

in vec3 vColor;
in vec2 vTextureCoord;
out vec4 Color;
uniform sampler2D uSampler;

void main() {
  Color = texture(uSampler, vTextureCoord);
}
