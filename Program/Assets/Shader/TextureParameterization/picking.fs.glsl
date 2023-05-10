#version 410 core

uniform sampler2D tex; 
out vec4 color; 
uniform int shader_now; 
in VS_OUT
{
vec2 texcoord; 
} fs_in;

void main(void)
{
	vec4 texColor = texture(tex, fs_in.texcoord);
	if (texColor.a < 0.01)
		color = vec4(0,0,0,0);
	else
		color = texColor;
}