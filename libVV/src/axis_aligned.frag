#version 300 es

uniform int width;
uniform int height;
uniform int depth;
uniform int threshold;

uniform highp sampler3D data;
uniform highp sampler3D colormap;

in highp vec3 frag_texcoord;
out highp vec4 fragment;

float	calculate_gradient(highp sampler3D volume, highp vec3 uvw)
{
	highp vec3	result;
	highp vec3	v[2];

	v[0].x = texture(volume, uvw + vec3(1.0 / float(width), 0, 0)).r;
	v[1].x = texture(volume, uvw - vec3(1.0 / float(width), 0, 0)).r;
	v[0].y = texture(volume, uvw + vec3(0, 1.0 / float(height), 0)).r;
	v[1].y = texture(volume, uvw - vec3(0, 1.0 / float(height), 0)).r;
	v[0].z = texture(volume, uvw + vec3(0, 0, 1.0 / float(depth))).r;
	v[1].z = texture(volume, uvw - vec3(0, 0, 1.0 / float(depth))).r;

	result = v[0] - v[1];
	return length(result);
}


void main()
{
	highp float v = float(texture(data, frag_texcoord).g * 65280.0 + texture(data, frag_texcoord).r * 255.0) / float(threshold);
	fragment = texture(colormap, vec3(v, 0.0, 0.0)) * vec4(1.0, 1.0, 1.0, calculate_gradient(data, frag_texcoord));
}