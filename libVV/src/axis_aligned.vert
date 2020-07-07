#version 300 es

uniform int nr_slices;

uniform highp mat4 local;
uniform highp mat4 world;
uniform highp mat4 view;
uniform highp mat4 projection;

vec4 vertices[6] = vec4[]
(
	vec4(-0.5, -0.5, 0, 1.0),
	vec4(0.5, 0.5, 0, 1.0),
	vec4(-0.5, 0.5, 0, 1.0),
	vec4(0.5, 0.5, 0, 1.0),
	vec4(-0.5, -0.5, 0, 1.0),
	vec4(0.5, -0.5, 0, 1.0)
);

out vec3 frag_texcoord;

void main()
{
	int i = gl_VertexID / 6;
	vec4 v = vertices[gl_VertexID % 6];
	v.z = float(i) / float(nr_slices) - 0.5;
	frag_texcoord = v.xyz + vec3(0.5, 0.5, 0.5);
	gl_Position = v * local * world * view * projection;
}
