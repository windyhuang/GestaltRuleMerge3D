#version 410

#define ID gl_InvocationID

layout(vertices = 16) out;
in vec3 vPosition[];
out vec3 tcPosition[];

uniform mat4 um4p;
uniform mat4 um4mv;
//uniform float uf1zoom;
uniform float TessLevelInner;
uniform float TessLevelOuter;

uniform vec3 startpoint;
uniform vec3 midpoint;
uniform vec3 endpoint;
TessLevelInner=1;

void main()
{
	/*tcPosition[0] =startpoint;
	tcPosition[1] =midpoint;
	tcPosition[2] =endpoint;*/
	tcPosition[ID] = vPosition[ID];
	if (ID == 0) {

		float newTessLevelInner = TessLevelInner;
		float newTessLevelOuter = TessLevelOuter;

		gl_TessLevelInner[0] = newTessLevelInner;
		gl_TessLevelInner[1] = newTessLevelInner;
		gl_TessLevelOuter[0] = newTessLevelOuter;
		gl_TessLevelOuter[1] = newTessLevelOuter;
		gl_TessLevelOuter[2] = newTessLevelOuter;
		gl_TessLevelOuter[3] = newTessLevelOuter;
	}
}