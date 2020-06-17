#version 300 es

uniform highp sampler3D data;
uniform highp sampler3D colormap;

out highp vec4 fragment;

void main()
{
	fragment = vec4(1.0, 1.0, 1.0, 1.0);
}
