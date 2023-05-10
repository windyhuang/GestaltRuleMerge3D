#include "../../Include/Common.h"
#include "../../Include/ViewManager.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Include/STB/stb_image_write.h"

using namespace glm;
using namespace std;


//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
} uniforms;

const std::string ProjectName = "Mipmap";
const std::string ShaderPath = "./Shader/" + ProjectName + "/";

GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;
		
GLuint			textureID;
ViewManager		m_camera;

GLuint fbo;
GLuint fboTex;
GLuint rbo;

GLuint samplerObj;

void CHECK_FRAMEBUFFER_STATUS()
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		/* choose different formats */
		break;

	default:
		/* programming error; will fail on all hardware */
		fputs("Framebuffer Error\n", stderr);
		exit(-1);
	}
}

void My_LoadTextures()
{
	//Texture setting
	///////////////////////////	
	//Load texture data from file
	TextureData tdata = Common::Load_png("checkerboard.jpg");

	//Generate empty texture
	glGenTextures( 1, &textureID);
	glBindTexture( GL_TEXTURE_2D, textureID);

	//Do texture setting
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);*/
	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////	

	glGenSamplers(1, &samplerObj);
	glSamplerParameteri(samplerObj, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerObj, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(samplerObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glSamplerParameteri(samplerObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameterf(samplerObj, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);


	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &fboTex);
	glBindTexture(GL_TEXTURE_2D, fboTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, 1024, 1024);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void My_Init()
{
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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
    
	glUseProgram(program);
	///////////////////////////	

	//Load model to shader program
	My_LoadTextures();

	// Set Camera
	//m_camera.SetRotation(0, -1, 0);

	if (glGetError() == GL_NO_ERROR)
	{
		puts("no error");
	}

	GLfloat maxAF;

	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		//do something
	}

	

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF);
	printf("Max AF: %f\n", maxAF);
}



//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	m_camera.SetWindowSize(width, height);
	glViewport(0, 0, width, height);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glm::mat4 modelMatrix = glm::translate(glm::vec3(0, -1, 0)) * glm::rotate(-glm::pi<float>() / 2, glm::vec3(1, 0, 0));

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 1024, 1024);
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, value_ptr(m_camera.GetViewMatrix() * m_camera.GetModelMatrix() * modelMatrix));
	glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, value_ptr(m_camera.GetProjectionMatrix(aspect)));
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1024, 1024);
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, value_ptr(m_camera.GetViewMatrix() * m_camera.GetModelMatrix() * modelMatrix));
	glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, value_ptr(m_camera.GetProjectionMatrix(aspect)));
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindSampler(0, samplerObj);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	glutSwapBuffers();
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
	m_camera.mouseEvents(button, state, x, y);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		printf("Mouse %d is pressed\n", button);
	}
	printf("%d %d %d %d\n",button,state,x,y);
}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	m_camera.keyEvents(key);
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);

	if (key == 'g' || key == 'G')
	{
		GLubyte *data = new GLubyte[1024 * 1024 * 4];
		glBindTexture(GL_TEXTURE_2D, fboTex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_write_png("ScreenShot.png", 1024, 1024, 4, data, 1024 * 4);
	}
}


void My_Mouse_Moving(int x, int y) {
	m_camera.mouseMoveEvent(x, y);
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
	glutInitWindowSize(1024, 1024);
	glutCreateWindow(ProjectName.c_str()); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	//Print debug information 
	Common::DumpInfo();
	////////////////////

	//Call custom initialize function
	My_Init();

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

