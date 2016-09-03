#version 450

layout(location = 0) out vec4 outColor;

in vec4 color;

void main() {
    outColor = color;
}
