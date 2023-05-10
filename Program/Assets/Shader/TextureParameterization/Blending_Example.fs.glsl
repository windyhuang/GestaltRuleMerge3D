#version 410
/*uniform sampler2D tex; 
out vec4 color; 
uniform int shader_int; 
uniform sampler2D shader_now; 
in VS_OUT
{
vec2 texcoord; 
} fs_in;

void main(void)
{
	vec4 texColor = texture(tex, fs_in.texcoord);
	//color = vec4(col,1.0,1.0);
	//color = texture(tex, fs_in.texcoord);
	//color = mix(texture(tex, fs_in.texcoord), texture(shader_int, fs_in.texcoord), 0.5);
	//vec4 texColor =texture(tex, texcoord);//texture(tex, fs_in.texcoord);
	//color = texColor;
	//
	if (texColor.a < 0.01)
		color = vec4(0,0,0,0);
	else
		color = texColor;
} */

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int width;
uniform int height;
int half_size=3;
vec2 img_size= vec2(1920, 1017);
void main()
{
	//vec2 img_size=vec2(width,height);
	
	//if(color2.w>0.8&&color2.w<color1.w)//&&&color1.g<1&&color1.b<1
	//	color1.rgb=color2.rgb;
	

	float color1 = texture(texture1, TexCoord).x;
	float color2 = texture(texture2, TexCoord).x;
	vec4 color6 = texture(texture1, TexCoord);
	vec4 color5 = texture(texture2, TexCoord);
	vec4 color3,color4;

	if (color2 <= color1 && color2 != 1)//||color.w==1) 
		color3.rgb = vec3(1, 0, 0);
	//else if(s12.g< s11.g||s22<s21.g||s32.g<s31.g||s42.g<s41.g)
	//	color3.rgb = vec3(1, 0, 0);
	else //if(color2.r==1)
		color3.rgb = vec3(1, 1, 1);
	//color1.rgb = color1.rgb * (1.0f - color2.a) + color2.rgb * color2.a * 1.0f;

	if(color2<1)
		color4.rgb = vec3(1, 0, 0);

	FragColor = color5;//color1+color2;
	//FragColor
}