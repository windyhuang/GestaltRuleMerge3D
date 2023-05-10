#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

out vec3 vPosition;

void main()
{
	vPosition = iv3vertex;
}