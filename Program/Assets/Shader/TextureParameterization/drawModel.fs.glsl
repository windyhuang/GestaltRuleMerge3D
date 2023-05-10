#version 410 core

out vec4 fragColor;

in VertexData
{
	vec3 vNormal;
	vec3 vViewPos;
	vec2 vTexCoord;
	vec3 barycentric;

} vertexIn;
const float Ka=0.5;   // Ambient reflection coefficient
const float Kd=0.1;   // Diffuse reflection coefficient
const float Ks=1.0;   // Specular reflection coefficient
const float shininessVal=1.0; // Shininess
uniform sampler2D texImage;
uniform bool useLighting=true;
uniform bool drawWireframe;
uniform vec4 faceColor;
uniform vec4 wireColor;
const vec3 ambientColor=vec3(0,0,0);
const vec3 diffuseColor=vec3(255,255,255);
float edgeFactor()
{
	vec3 d = fwidth(vertexIn.barycentric);
	vec3 a3 = smoothstep(vec3(0.0), d *0.8, vertexIn.barycentric);
	return min(min(a3.x, a3.y), a3.z);
}
float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main(void)
{
	//vec4 faceColor = vec4(vec3(1.0) * lightIntense, 1.0);
	vec4 newFaceColor = faceColor;
	

	if (useLighting)
	{
		vec3 viewVector = -vertexIn.vViewPos;
		vec3 lightDir =  normalize(vec3(0, 0, -1));

		vec3 L = -lightDir;
		vec3 V = normalize(viewVector);
		vec3 N = normalize(vertexIn.vNormal);

		float ambient = 0.01;
		float diffuse = max(dot(N, L), 0);
		float specular = 0;
		if (diffuse > 0)
		{
			vec3 H = normalize(L + V);
			specular = pow(dot(N, H), 256);
		}

		newFaceColor = vec4(vec3(1.0) * ( ambient ) + newFaceColor.xyz * diffuse, newFaceColor.a);
	}



	//vec4 color = newFaceColor;
	vec4 color = faceColor;
	if (drawWireframe)
	{
		float ef = edgeFactor();
		color = mix(wireColor, newFaceColor, ef);
	}
	// outputs final color
	float depth = logisticDepth(gl_FragCoord.z);
	 fragColor =color;//vec4(0.92f, 1.28f, 1.44f, 1.0f);//newFaceColor; //
	//fragColor = vec4(vec3(depth), 1.0f);
	//fragColor=color;
}