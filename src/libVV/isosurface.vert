#version 320 es

uniform highp sampler3D gradient;

uniform mat4 scaling;
uniform mat4 world_matrix;
uniform mat4 viewing_matrix;
uniform mat4 projection_matrix;

in vec3 vertex;
out vec3 out_texcoord;

void main()
{
	out_texcoord = vertex.xyz + vec3(0.5, 0.5, 0.5);
	vec4 position = (projection_matrix * viewing_matrix * world_matrix * scaling) * vec4(vertex, 1.0);
	gl_Position = position;
}