#include "../../Include/Common.h"
#include "../../Include/ViewManager.h"

using namespace glm;
using namespace std;


//uniform id
struct
{
	GLint  mv_matrix;
	GLint  proj_matrix;
} uniforms;

const std::string ProjectName = "BasicTexture";
const std::string ShaderPath = "./Shader/" + ProjectName + "/";

GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;
		
GLuint			textureID;
ViewManager		m_camera;


void My_LoadTextures()
{
	//Texture setting
	///////////////////////////	
	//Load texture data from file
	TextureData tdata = Common::Load_png(("./Imgs/" +ProjectName + "/wood.png").c_str());

	//Generate empty texture
	glGenTextures( 1, &textureID);
	glBindTexture( GL_TEXTURE_2D, textureID);
	
	//Do texture setting
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);
	///////////////////////////	
}

void My_Init()
{
    
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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
	m_camera.Zoom(-16);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Update shaders' input variable
	///////////////////////////	
	glUseProgram(program);

    glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, value_ptr(m_camera.GetViewMatrix() * m_camera.GetModelMatrix()));
	glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, value_ptr(m_camera.GetProjectionMatrix(aspect)));

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);
	///////////////////////////	

    glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	m_camera.SetWindowSize(width, height);
	glViewport(0, 0, width, height);
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

