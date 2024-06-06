#version 320 es

uniform highp isampler3D volume;
uniform highp sampler3D gradient;
uniform highp sampler2D colormap;
uniform highp int number_of_slices;
uniform bool enable_lighting;
uniform highp float ambient;
uniform bool enable_auto_opacity;

uniform highp mat4 scaling;
uniform highp mat4 world_matrix;
uniform highp mat4 viewing_matrix;
uniform highp mat4 projection_matrix;


uniform int maxvalue;

in highp vec3 out_texcoord;
out highp vec4 fragment;

void main ()
{
	highp float gradient_length = texture(gradient, out_texcoord).a;

	highp vec4 color = texture(colormap, vec2(float(texture(volume, out_texcoord).r) / float(maxvalue), enable_auto_opacity ? 0.0 : gradient_length));

	highp vec3 normal = normalize(texture(gradient, out_texcoord).xyz);
	normal = normalize(((projection_matrix * viewing_matrix * world_matrix * scaling) * vec4(normal, 0.0)).xyz);
	highp float n_dot_l = dot(normal, vec3(0.0, 0.0, -1.0));
	highp float diffuse = sign(n_dot_l) * n_dot_l * (1.0 - ambient);

	if(enable_lighting)
		color.rgb = color.rgb * (diffuse + ambient);

	if(enable_auto_opacity)
	{
		color.a *= gradient_length;
	}


	fragment = color;
}
