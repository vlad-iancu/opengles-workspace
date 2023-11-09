#version 300 es
in vec3 aPosition;
in vec3 aColor;
in vec2 aTextureCoord;

out vec3 vColor;
out vec2 vTextureCoord;

void main() {
    vColor = aColor;
    vTextureCoord = aTextureCoord;
    gl_Position = vec4(aPosition, 1.0);
}


