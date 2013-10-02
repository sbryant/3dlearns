#version 150

in vec2 position;
in vec3 color;

out vec3 Color;

uniform mat4 trans;

void main() {
    Color = color;
    gl_Position = trans * vec4(position, 0.0, 1.0);
}
