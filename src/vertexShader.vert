#version 300 es
in vec3 aPosition;
in vec3 aColor;
in vec2 aTextureCoord;

out vec3 vColor;
out vec2 vTextureCoord;

uniform mat4 uModel;
void main() {
    vColor = aColor;
    vTextureCoord = aTextureCoord;
    gl_Position = uModel * vec4(aPosition, 1.0);
}


