#version 410

/*layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

out VS_OUT        
{                 
    vec2 texcoord;
} vs_out; 

void main()
{
	gl_Position = vec4(position.x,position.y, 0.0, 1.0);
	vs_out.texcoord = texcoord;
}*/

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}