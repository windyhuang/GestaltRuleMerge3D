#version 410

layout(quads, equal_spacing, ccw) in;

in vec3 tcPosition[];

out vec3 tePosition;
out vec4 tePatchDistance;

uniform mat4 um4p;
uniform mat4 um4mv;

uniform mat4 BT;
//uniform vec3 startpoint;
//uniform vec3 midpoint;
//uniform vec3 endpoint;

void main()
{

	float u = gl_TessCoord.x, v = gl_TessCoord.y;

    mat4 Px = mat4(
        tcPosition[0].x, tcPosition[1].x, tcPosition[2].x, tcPosition[3].x, 
        tcPosition[4].x, tcPosition[5].x, tcPosition[6].x, tcPosition[7].x, 
        tcPosition[8].x, tcPosition[9].x, tcPosition[10].x, tcPosition[11].x, 
        tcPosition[12].x, tcPosition[13].x, tcPosition[14].x, tcPosition[15].x );

    mat4 Py = mat4(
        tcPosition[0].y, tcPosition[1].y, tcPosition[2].y, tcPosition[3].y, 
        tcPosition[4].y, tcPosition[5].y, tcPosition[6].y, tcPosition[7].y, 
        tcPosition[8].y, tcPosition[9].y, tcPosition[10].y, tcPosition[11].y, 
        tcPosition[12].y, tcPosition[13].y, tcPosition[14].y, tcPosition[15].y );

    mat4 Pz = mat4(
        tcPosition[0].z, tcPosition[1].z, tcPosition[2].z, tcPosition[3].z, 
        tcPosition[4].z, tcPosition[5].z, tcPosition[6].z, tcPosition[7].z, 
        tcPosition[8].z, tcPosition[9].z, tcPosition[10].z, tcPosition[11].z, 
        tcPosition[12].z, tcPosition[13].z, tcPosition[14].z, tcPosition[15].z );

    mat4 cx =  Px * BT;
    mat4 cy =  Py * BT;
    mat4 cz =  Pz * BT;

    vec4 U = vec4(u*u*u, u*u, u, 1);
    vec4 V = vec4(v*v*v, v*v, v, 1);

    float x = dot(cx * V, U);
    float y = dot(cy * V, U);
    float z = dot(cz * V, U);
    tePosition =  vec3(x, y, z);

    tePatchDistance = vec4(u, v, 1-u, 1-v);
    gl_Position = um4p * um4mv * vec4(x, y, z, 1);
}
