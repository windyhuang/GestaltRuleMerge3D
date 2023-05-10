#version 410 core

out vec4 fragColor;

in VertexData
{
	vec3 vNormal;
	vec3 vViewPos;
	vec2 vTexCoord;
	vec3 barycentric;

} vertexIn;


uniform sampler2D tex;
uniform vec4 color;
in vec4 gsOutColor;


void main(void)
{
	
	 fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}