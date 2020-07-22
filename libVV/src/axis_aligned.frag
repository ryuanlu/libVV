#version 300 es

uniform highp sampler3D data;
uniform highp sampler3D colormap;

in highp vec3 frag_texcoord;
out highp vec4 fragment;

void main()
{
	highp float v = float(texture(data, frag_texcoord).g * 256.0 + texture(data, frag_texcoord).r) / 256.0;
	fragment = texture(colormap, vec3(v, 0.0, 0.0));
}