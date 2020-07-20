#version 300 es

uniform highp sampler3D data;
uniform highp sampler3D colormap;

in highp vec3 frag_texcoord;
out highp vec4 fragment;

void main()
{
	fragment = texture(data, frag_texcoord);
}
