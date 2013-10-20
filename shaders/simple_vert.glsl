#version 150

in vec3 position;
in vec3 color;

out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    Color = vec4(color, 1.0);
    gl_Position = proj * view * model * vec4(position, 1.0);
}
