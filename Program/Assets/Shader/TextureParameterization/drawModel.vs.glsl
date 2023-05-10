#version 410 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat3 um3n;
uniform mat4 um4mv;
//uniform mat4 um4m;
//uniform mat4 um4v;
uniform mat4 um4p;
uniform mat4 um4u;
uniform vec3 norm;
uniform bool drawTexCoord;
out vec4 OutColor;									// ¿é¥XÃC¦â
out VertexData
{
	vec3 vNormal;
	vec3 vViewPos;
	vec2 vTexCoord;
} vertexOut;

void main()
{
	vec4 pos;
	if (drawTexCoord)
	{
		vec2 offset = vec2(-0.5, -0.5);
		pos = um4u * vec4(texCoord + offset, 0.0, 1.0);
	}
	else
	{
		pos = vec4(vertex, 1.0);
	}

	vec4 newTexCoord = um4u * vec4(texCoord, 0.0, 1.0);
	vec4 viewModelPos = um4mv * vec4(vertex, 1.0);
	vertexOut.vNormal = normalize(um3n * norm);
	vertexOut.vViewPos = vec3(viewModelPos);
	vertexOut.vTexCoord = newTexCoord.xy;

	gl_Position = um4p * um4mv * pos;
}
