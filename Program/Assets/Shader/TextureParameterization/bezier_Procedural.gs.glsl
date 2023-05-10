#version 410

uniform mat4 um4p;
uniform mat4 um4mv;
uniform mat3 um3n;
uniform float TessLevelOuter;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];//
in vec4 tePatchDistance[3];

out vec3 gFacetNormal;
out vec4 gPatchDistance;
out vec3 gTriDistance;



void main()
{
	/*gPatchDistance=tePatchDistance[0];
	gPatchDistance=tePatchDistance[1];
	gPatchDistance=tePatchDistance[2];
	*/
	/*mat4 mvp_matrix =  um4p * um4mv ;
	vec4 up = vec4(0, 1, 0, 0);
	vec4 right = vec4(1, 0, 0, 0);
	vec4 nowRight = mvp_matrix*(tePosition) * right;
	vec4 nextRight = mvp_matrix*(tePosition + 1) *  right;
	int count =TessLevelOuter*2;
	float angle offset= (360.0 / count) * (PI / 180.0);
	float (int i=0;i<count;i++){
		float nowAngle = i * angleOffset;
		float x = cos(nowAngle);
		float y = sin(nowAngle);
		gl_Position = mvp_matrix * (gl_in[0].gl_Position + nowRight * x + up * y);
		gsOutColor = vec4(i / count, 0, 0, 1.0);
		EmitVertex();
		gl_Position = mvp_matrix * (gl_in[1].gl_Position + nextRight * x + up * y);
		gsOutColor = vec4(i / count, 0, 0, 1.0);
		EmitVertex();
	}
	EndPrimitive();
	*/
	vec3 A = tePosition[2] - tePosition[0];
	vec3 B = tePosition[1] - tePosition[0];
	gFacetNormal = normalize(cross(A, B));

	gPatchDistance = tePatchDistance[0];
	gTriDistance = vec3(1, 0, 0);
	gl_Position = gl_in[0].gl_Position; EmitVertex();

	gPatchDistance = tePatchDistance[1];
	gTriDistance = vec3(0, 1, 0);
	gl_Position = gl_in[1].gl_Position; EmitVertex();

	gPatchDistance = tePatchDistance[2];
	gTriDistance = vec3(0, 0, 1);
	gl_Position = gl_in[2].gl_Position; EmitVertex();
	EmitVertex();
	EndPrimitive();
}