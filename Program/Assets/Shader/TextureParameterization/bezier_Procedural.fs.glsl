#version 410

layout(location = 0) out vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec4 gPatchDistance;

uniform vec3 diffuse_albedo = vec3(0.7333f, 1.0f, 1.0f);
uniform vec3 ambient = vec3(0.04f, 0.04f, 0.04f);
uniform vec3 light_pos = vec3(0.25, 0.25, 1);
uniform sampler2D depthTexture;
float amplify(float d, float scale, float offset)
{
	d = scale * d + offset;
	d = clamp(d, 0, 1);
	d = 1 - exp2(-2 * d*d);
	return d;
}

void main()
{
	vec4 landscape = texture(m_texture, gs_in.tc);

	vec3 N = normalize(gs_in.normal);
	vec3 L = light_pos;
	float df = abs(clamp(dot(N, L), 0, 1)) * 1.5;

	color = vec4(landscape.xyz / 2 + ambient + df * diffuse_albedo, 1.0);
	FragColor = vec4(color, 1.0);
	/*vec3 N = normalize(gFacetNormal);
	vec3 L = light_pos;
	float df = abs(dot(N, L));
	vec3 color = ambient + df * diffuse_albedo;

	float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
	float d2 = min(min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z), gPatchDistance.w);
	color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
	
	FragColor = vec4(1.0,0,0, 1.0);*///vec4(color, 1.0);
}