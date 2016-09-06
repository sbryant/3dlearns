#version 450 core

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

uniform sampler2D tex;

void main() {
	vec4 c = texture(tex, texCoords);
    outColor = vec4(1.0, 0.0, 0.0, 1.0) * vec4(1.0, 1.0, 1.0, c.r);
}
