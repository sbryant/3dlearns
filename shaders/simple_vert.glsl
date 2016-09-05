#version 450 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_tex;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 texCoords;

uniform mat4 projection;

void main() {
    /*
	color = vec4(in_color, 1.0);
    gl_Position = proj * view * model * vec4(in_position, 1.0);
	*/
	texCoords = vec2(in_tex);
	gl_Position = projection * vec4(in_position, 1.0);
}
