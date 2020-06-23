#version 300 es

uniform highp mat4 world;
uniform highp mat4 view;
uniform highp mat4 projection;

void main()
{
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0) * world * view * projection;
}
