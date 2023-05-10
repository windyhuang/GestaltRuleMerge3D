#version 410 core

layout(triangles,invocations) in;
layout(line_strip, max_vertices = 2) out;

in VertexData
{
	vec3 vNormal;
	vec3 vViewPos;
	vec2 vTexCoord;

} vertexIn[];

out VertexData
{
	vec3 vNormal;
	vec3 vViewPos;
	vec2 vTexCoord;
	vec3 barycentric;

} vertexOut;
out vec4 gsOutColor;
uniform mat3 um3n;
uniform mat4 um4mv;
uniform mat4 um4p;
uniform mat4 um4u;
const vec4 invocation_colors = vec4(1.0, 0.0, 0.0, 1.0);
void main()
{
	//vertexOut.vNormal = vertexIn[0].vNormal;
	vertexOut.vViewPos = vertexIn[0].vViewPos;
	vertexOut.vTexCoord = vertexIn[0].vTexCoord;
	vertexOut.barycentric = vec3(1, 0, 0);
	gl_Position = gl_in[0].gl_Position;
	gsOutColor=invocation_colors;
	EmitVertex();

	//vertexOut.vNormal = vertexIn[1].vNormal;
	vertexOut.vViewPos = vertexIn[1].vViewPos;
	vertexOut.vTexCoord = vertexIn[1].vTexCoord;
	vertexOut.barycentric = vec3(0, 1, 0);
	gl_Position = gl_in[1].gl_Position;
	gsOutColor=invocation_colors;
	EmitVertex();

	

	EndPrimitive();
}