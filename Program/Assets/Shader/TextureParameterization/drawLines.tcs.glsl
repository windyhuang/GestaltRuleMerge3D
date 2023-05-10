#version 410 core
#define ID gl_InvocationID

layout(vertices = 2) out;


in vec3 vViewPos[];
out vec3 tcPosition[];

uniform mat3 um3n;
uniform mat4 um4mv;
uniform mat4 um4p;
uniform mat4 um4u;
uniform bool drawTexCoord;



void main()
{
	mat4 mvp = um4p * um4mv;
	tcPosition[ID] = vViewPos[ID];
	
	if (ID == 0) {
		vec4 p0 = mvp * gl_in[0].gl_Position;
		vec4 p1 = mvp * gl_in[1].gl_Position;
		float newTessLevelInner = TessLevelInner;
		float newTessLevelOuter = TessLevelOuter;

		
		gl_TessLevelOuter[0] = 64;
		gl_TessLevelOuter[1] = 64;
		
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
}
