#version 320 es

uniform highp sampler3D gradient;

uniform highp mat4 scaling;
uniform highp mat4 world_matrix;
uniform highp mat4 viewing_matrix;
uniform highp mat4 projection_matrix;

uniform highp float ambient;
uniform highp vec4 color;

in highp vec3 out_texcoord;
out highp vec4 fragment;

void main ()
{
	highp vec3 normal = normalize(texture(gradient, out_texcoord).xyz);

	normal = normalize(((projection_matrix * viewing_matrix * world_matrix * scaling) * vec4(normal, 0.0)).xyz);

	highp float n_dot_l = dot(normal, vec3(0.0, 0.0, -1.0));
	highp float diffuse = sign(n_dot_l) * n_dot_l * (1.0 - ambient);

	fragment = vec4(color.rgb * (diffuse + ambient), 1.0);
}
