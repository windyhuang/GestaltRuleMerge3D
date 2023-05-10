#version 410 core
layout(lines) in;
layout(line_strip, max_vertices = 2) out;
out vec3 tePosition[2];

uniform sampler2D heightMap;

uniform mat4  um4mv;
uniform mat4  um4p;


void main(void)
{
	mat4 mvp = um4p * um4mv;
	if (gl_InvocationID == 0)
	{
		vec2 tc1 = tes_in[0].tc;
		vec2 tc2 = tes_in[1].tc;
		vec4 p0 = mvp * gl_in[0].gl_Position;
		vec4 p1 = mvp * gl_in[1].gl_Position;
		float depth= texture(heightMap, tc1).x;
		float depth1= texture(heightMap, tc2).x;
		if(p0.z>depth&&p1.z>depth1)
		{
			//gl_Position = um4p * um4mv * p;
			tePosition[0]=p0;
			tePosition[1]=p1;
		}
	}
}
