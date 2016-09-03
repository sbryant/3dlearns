#version 450

layout(location = 0) out vec4 color;

out gl_PerVertex {
	vec4 gl_Position;
};

vec2 positions[3] = vec2[](
	vec2(0.0, 0.5),
	vec2(0.5, -0.5),
	vec2(-0.5, -0.5)
);

vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void main() {
    /*
	color = vec4(in_color, 1.0);
    gl_Position = proj * view * model * vec4(in_position, 1.0);
	*/
	color = vec4(colors[gl_VertexID], 1.0);
	gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}
