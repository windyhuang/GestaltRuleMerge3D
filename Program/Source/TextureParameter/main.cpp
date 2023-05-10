#include "../../Include/AntTweakBar/AntTweakBar.h"
#include "../../Include/Common.h"
#include "../../Include/ViewManager.h"

using namespace glm;
using namespace std;

#define TO_RADIAN(angle) angle / 180.0f * -glm::pi<float>()

//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
} uniforms;

const std::string ProjectName = "TextureParameter";
const std::string ShaderPath = "./Shader/" + ProjectName + "/";

GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
mat4			modelMatrix = translate(vec3(0, -3, 0)) * rotate(TO_RADIAN(80), vec3(1, 0, 0));
float			aspect;
		
GLuint			textureID;
GLuint			textureUnit = 0;
GLuint			textureLoc;
ViewManager		m_camera;

// GUI
TwBar *bar;
float fps = 0;
unsigned int currentWrapping;
unsigned int currentFiltering;
unsigned int currentAniso;
unsigned int frames = 0;
unsigned int lastTime = 0;
unsigned int currentTime = 0;
vec3 borderColor(1, 1, 0);


typedef enum
{
	REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, NUM_WRAPPING
}
WrappingParam;

typedef enum
{
	NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_NEAREST, LINEAR_MIPMAP_LINEAR, NUM_FILTERING
} 
FilteringParam;

const int NUM_SAMPLER = 6;
GLuint samplerObjs[NUM_SAMPLER];

GLuint MapWrappingParam(WrappingParam param)
{
	switch (param)
	{
	case REPEAT:			return GL_REPEAT;
	case MIRRORED_REPEAT:	return GL_MIRRORED_REPEAT;
	case CLAMP_TO_EDGE:		return GL_CLAMP_TO_EDGE;
	case CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
	default:				return GL_REPEAT;
	}
}

GLuint MapFilteringParam(FilteringParam param)
{
	switch (param)
	{
	case NEAREST:					return GL_NEAREST;
	case LINEAR:					return GL_LINEAR;
	case NEAREST_MIPMAP_NEAREST:	return GL_NEAREST_MIPMAP_NEAREST;
	case NEAREST_MIPMAP_LINEAR:		return GL_NEAREST_MIPMAP_LINEAR;
	case LINEAR_MIPMAP_NEAREST:		return GL_LINEAR_MIPMAP_NEAREST;
	case LINEAR_MIPMAP_LINEAR:		return GL_LINEAR_MIPMAP_LINEAR;
	default:						return GL_NEAREST;
	}
}

void TW_CALL SetWrappingCB(const void *value, void *clientData)
{
	currentWrapping = *(unsigned int *)value;

	int display;
	if (currentWrapping == 3)
	{
		display = 1;
		TwSetParam(bar, "BorderColor", "visible", TW_PARAM_INT32, 1, (void *)&display);
	}
	else
	{
		display = 0;
		TwSetParam(bar, "BorderColor", "visible", TW_PARAM_INT32, 1, (void *)&display);
	}

	GLuint wrappingParam = MapWrappingParam((WrappingParam)currentWrapping);
	for (int i = 0; i < NUM_SAMPLER; ++i)
	{
		glSamplerParameteri(samplerObjs[i], GL_TEXTURE_WRAP_S, wrappingParam);
		glSamplerParameteri(samplerObjs[i], GL_TEXTURE_WRAP_T, wrappingParam);
		glSamplerParameterfv(samplerObjs[i], GL_TEXTURE_BORDER_COLOR, value_ptr(borderColor));
	}
}

void TW_CALL GetWrappingCB(void *value, void *clientData)
{
	*(unsigned int *)value = currentWrapping;
}

void TW_CALL SetBorderColor(const void *value, void *clientData)
{
	float *src = (float *)value;
	float *dst = value_ptr(borderColor);
	std::copy(src, src + 3, dst);

	for (int i = 0; i < NUM_SAMPLER; ++i)
	{
		glSamplerParameterfv(samplerObjs[i], GL_TEXTURE_BORDER_COLOR, dst);
	}
}

void TW_CALL GetBorderColor(void *value, void *clientData)
{
	float *src = value_ptr(borderColor);
	float *dst = (float *)value;
	std::copy(src, src + 3, dst);
}

void TW_CALL SetAniso(const void *value, void *clientData)
{
	currentAniso = *(unsigned int *)value;
	
	for (int i = 0; i < NUM_SAMPLER; ++i)
	{
		glSamplerParameteri(samplerObjs[i], GL_TEXTURE_MAX_ANISOTROPY_EXT, 1 << currentAniso);
	}
}

void TW_CALL GetAniso(void *value, void *clientData)
{
	*(unsigned int *)value = currentAniso;
}

void setupGUI()
{
#ifdef _MSC_VER
	TwInit(TW_OPENGL, NULL);
#else
	TwInit(TW_OPENGL_CORE, NULL);
#endif
	TwGLUTModifiersFunc(glutGetModifiers);
	bar = TwNewBar("Texture Parameter Setting");
	TwDefine(" 'Texture Parameter Setting' size='220 220' ");
	TwDefine(" 'Texture Parameter Setting' fontsize='3' color='96 216 224'");
	TwAddVarRO(bar, "time", TW_TYPE_FLOAT, &fps, " label='FPS' help='Frame Per Second(FPS)' ");
	
	{
		TwEnumVal wrappingEV[4] = { { REPEAT, "Repeat" },{ MIRRORED_REPEAT, "Mirrored Repeat" },
									{ CLAMP_TO_EDGE, "Clamp to edge" },{ CLAMP_TO_BORDER, "Clamp to border" } };
		TwType wrappingType = TwDefineEnum("WrappingType", wrappingEV, 4);
		TwAddVarCB(bar, "Wrapping", wrappingType, SetWrappingCB, GetWrappingCB, NULL, "help='Change texture wrapping.' ");

		TwAddVarCB(bar, "BorderColor", TW_TYPE_COLOR3F, SetBorderColor, GetBorderColor, NULL, "visible=false label='Border Color' opened=false help='Used in CALMP_TO_BORDER'");

		TwEnumVal filteringEV[6] = {{ NEAREST, "Nearest" },{ LINEAR, "Linear" },
									{ NEAREST_MIPMAP_NEAREST, "Nearest Mipmap Nearest" },{ NEAREST_MIPMAP_LINEAR, "Nearest Mipmap Linear" },
									{ LINEAR_MIPMAP_NEAREST, "Linear Mipmap Nearest" },{ LINEAR_MIPMAP_LINEAR, "Linear Mipmap Linear" } };
		TwType filteringType = TwDefineEnum("FilteringType", filteringEV, 6);
		TwAddVarRW(bar, "Filtering", filteringType, &currentFiltering, "help='Change texture filtering.' ");
	}

	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		GLfloat maxAF;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF);

		std::vector<std::string> labels;
		while (maxAF > 1)
		{
			labels.push_back(std::to_string((int)maxAF) + "x");
			maxAF /= 2;
		}
		labels.push_back("None");
		
		std::vector<TwEnumVal> anisoEV;
		for (int i = 0; i < labels.size(); ++i)
		{
			anisoEV.push_back({ i, labels[labels.size() - 1 - i].c_str() });
		}

		TwType anisoType = TwDefineEnum("AnisoType", &anisoEV[0], anisoEV.size());
		TwAddVarCB(bar, "Anisotropic", anisoType, SetAniso, GetAniso, NULL, "help='Change anisotropic level.' ");
	}

}

void My_LoadTextures()
{
	//Load texture data from file
	TextureData tdata = Common::Load_png(("./Imgs/" + ProjectName + "/brickwork.jpg").c_str());

	//Generate texture
	glGenTextures( 1, &textureID);
	glBindTexture( GL_TEXTURE_2D, textureID);	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void My_CreateSamplerObj()
{
	glGenSamplers(NUM_SAMPLER, samplerObjs);

	for (int i = 0; i < NUM_SAMPLER; ++i)
	{
		glSamplerParameteri(samplerObjs[i], GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(samplerObjs[i], GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glSamplerParameteri(samplerObjs[NEAREST], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(samplerObjs[NEAREST], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glSamplerParameteri(samplerObjs[LINEAR], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerObjs[LINEAR], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glSamplerParameteri(samplerObjs[NEAREST_MIPMAP_NEAREST], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glSamplerParameteri(samplerObjs[NEAREST_MIPMAP_NEAREST], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glSamplerParameteri(samplerObjs[NEAREST_MIPMAP_LINEAR], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glSamplerParameteri(samplerObjs[NEAREST_MIPMAP_LINEAR], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glSamplerParameteri(samplerObjs[LINEAR_MIPMAP_NEAREST], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glSamplerParameteri(samplerObjs[LINEAR_MIPMAP_NEAREST], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glSamplerParameteri(samplerObjs[LINEAR_MIPMAP_LINEAR], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(samplerObjs[LINEAR_MIPMAP_LINEAR], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
	

void My_Init()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Initialize shaders
	///////////////////////////	
    program = glCreateProgram();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = Common::LoadShaderSource((ShaderPath + "vertex.vs.glsl").c_str());
	char** fsSource = Common::LoadShaderSource((ShaderPath + "fragment.fs.glsl").c_str());
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);
	Common::FreeShaderSource(vsSource);
	Common::FreeShaderSource(fsSource);
	glCompileShader(vs);
	glCompileShader(fs);
	Common::ShaderLog(vs);
	Common::ShaderLog(fs);
    
	//Attach Shader to program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

	//Cache uniform variable id
	uniforms.proj_matrix = glGetUniformLocation(program, "um4p");
	uniforms.mv_matrix = glGetUniformLocation(program, "um4mv");
    
	
	textureLoc = glGetUniformLocation(program, "tex");
	glUseProgram(program);
	glUniform1i(textureLoc, textureUnit);
	
	///////////////////////////	

	//Load model to shader program
	My_LoadTextures();
	My_CreateSamplerObj();

	glUseProgram(0);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (currentTime - lastTime > 1000)
	{
		fps = (frames * 1000) / (currentTime - lastTime);
		frames = 0;
		lastTime = currentTime;
	}
	++frames;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////	
	glUseProgram(program);

    glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, value_ptr(m_camera.GetViewMatrix() * m_camera.GetModelMatrix() * modelMatrix));
	glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, value_ptr(m_camera.GetProjectionMatrix(aspect)));

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindSampler(textureUnit, samplerObjs[currentFiltering]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindSampler(0, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	///////////////////////////	

	TwDraw();
    glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	m_camera.SetWindowSize(width, height);
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Mouse event
void My_Mouse(int button, int state, int x, int y)
{
	if (!TwEventMouseButtonGLUT(button, state, x, y))
	{
		m_camera.mouseEvents(button, state, x, y);
		TwRefreshBar(bar);
	}
}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	if (!TwEventKeyboardGLUT(key, x, y))
	{
		m_camera.keyEvents(key);
		printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	}
}


void My_Mouse_Moving(int x, int y) {
	if (!TwEventMouseMotionGLUT(x, y))
	{
		m_camera.mouseMoveEvent(x, y);
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    //Change working directory to source code path
    chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow(ProjectName.c_str()); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	//Print debug information 
	Common::DumpInfo();
	////////////////////

	//Call custom initialize function
	My_Init();
	setupGUI();

	//Register GLUT callback functions
	////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutTimerFunc(16, My_Timer, 0); 
	glutPassiveMotionFunc(My_Mouse_Moving);           
	glutMotionFunc(My_Mouse_Moving);     
	////////////////////

	// Enter main event loop.
	glutMainLoop();

	return 0;
}

