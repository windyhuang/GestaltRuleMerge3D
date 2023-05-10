#include <Common.h>
#include <ViewManager.h>
#include <ResourcePath.h>
#include "DrawModelShader.h"
#include "MyModel.h"

using namespace glm;
using namespace std;

const std::string ProjectName = "TextureBufferObject";

mat4			projMat;		//projection matrix
mat4			viewMat;		//projection matrix
mat4			modelMat;		//projection matrix
float			aspect;
vec3			cameraPos(0, 50, 100);

GLuint textureID;
float yawAngle = 0.0f;
float pitchAngle = 0.0f;
float lastMouseX;
float lastMouseY;
bool isLeftMousePressed = false;

DrawModelShader drawModelShader;
MyModel model;


void My_Init()
{
	ResourcePath::shaderPath = "./Shader/" + ProjectName + "/";
	ResourcePath::imagePath = "./Imgs/" + ProjectName + "/";


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///////////////////////////

	TextureData tdata = Common::Load_png("Model/bottle_mana.png");

	//Generate empty texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Do texture setting
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	///////////////////////////

	drawModelShader.Init();
	model.Init("Model/Potion_bottle.obj");

	viewMat = lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glm::mat4 rotMat;

	rotMat = glm::rotate(rotMat, radians(pitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(rotMat, radians(yawAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 newCameraPos = cameraPos * glm::mat3(rotMat);
	viewMat = lookAt(newCameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 mvMat = viewMat * modelMat;
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));

	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawModelShader.Enable();
	drawModelShader.SetMVMat(mvMat);
	drawModelShader.SetPMat(projMat);
	glBindTexture(GL_TEXTURE_2D, textureID);
	model.Render();
	glBindTexture(GL_TEXTURE_2D, 0);
	drawModelShader.Disable();

	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	aspect = width * 1.0f / height;
	glViewport(0, 0, width, height);
	projMat = perspective(radians(60.0f), aspect, 0.1f, 1000.0f);
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
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			isLeftMousePressed = true;
			printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		}
		else if (state == GLUT_UP)
		{
			isLeftMousePressed = false;
			printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		printf("Mouse %d is pressed\n", button);
	}
	printf("%d %d %d %d\n", button, state, x, y);
}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}


void My_Mouse_Moving(int x, int y) {

	if (isLeftMousePressed)
	{
		yawAngle += 0.1f * (x - lastMouseX);
		pitchAngle += 0.1f * (y - lastMouseY);
	}

	lastMouseX = x;
	lastMouseY = y;

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
	glutInitWindowSize(800, 800);
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

