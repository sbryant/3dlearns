#version 150

in vec3 position;
in vec3 color;

out vec4 Color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform vec4 vecColor;

void main() {
    Color = vecColor;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
