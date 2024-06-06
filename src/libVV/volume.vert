#version 320 es

uniform int number_of_slices;

uniform mat4 direction;
uniform mat4 scaling;
uniform mat4 world_matrix;
uniform mat4 viewing_matrix;
uniform mat4 projection_matrix;

out vec3 out_texcoord;

vec2 slice_vertices[] = vec2[](

	vec2(-0.5, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5),

	vec2(-0.5, -0.5),
	vec2(0.5, -0.5),
	vec2(0.5, 0.5)

);

void main()
{
	int serial = gl_VertexID / 6;
	float z = -0.5 + float(serial) / float(number_of_slices);
	vec4 vertex = direction * vec4(slice_vertices[gl_VertexID % 6], z, 1.0);
	out_texcoord = vertex.xyz + vec3(0.5, 0.5, 0.5);
	vec4 position = (projection_matrix * viewing_matrix * world_matrix * scaling) * vertex;
	gl_Position = position;
}