#version 450


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
out vec4 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main() {
    color = vec4(in_color, 1.0);
    gl_Position = proj * view * model * vec4(in_position, 1.0);
}
