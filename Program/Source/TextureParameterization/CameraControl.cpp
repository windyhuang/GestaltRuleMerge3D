#include "CameraControl.h"

using namespace glm;

/**
* �إ߬۾��A�öi���l�ơC
*/
CameraControl::CameraControl()
{
	ortho = false;
	zoom = 1.0f;
	moveSpeed = 5.0f;
	lmbDown = false;
	midDown = false;
	eyePosition = vec3(0.0f, 0.0f, 10.0f);
	eyeLookPosition = vec3(0.0f, 0.0f, 0.0f);
	vec3 up = vec3(0, 1, 0);
	viewMatrix = lookAt(eyePosition, eyeLookPosition, up);
	viewVector = eyePosition - eyeLookPosition;
	viewVector = normalize(viewVector);
}

/**
* ���oModel Matrix�C
*/
mat4 CameraControl::GetModelMatrix() {
	return translationMatrix * rotationMatrix;
}

/**
* ���oView Matrix�C
*/
mat4 CameraControl::GetViewMatrix()
{
	return viewMatrix;
}

/**
* �]�w�è��oProjection Matrix�C
* @param aspect �e�������e��C
*/
mat4 CameraControl::GetProjectionMatrix(float aspect)
{
	float nearVal;
	float farVal;
	nearVal = 0.1f;
	farVal = 100.0f;
	if (ortho) {
		float size = 1.5f * zoom;
		projMatrix = glm::ortho(-aspect * size, aspect * size, -size, size, nearVal, farVal);
	}
	else {
		projMatrix = perspective(radians(30.0f * zoom), aspect, nearVal, farVal);
	}
	return projMatrix;
}
void CameraControl::SetupGL() {
	float nearVal;
	float farVal;
	nearVal = 0.1f;
	farVal = 1000.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float size = 1.5f * zoom;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(-aspect * size, aspect * size, -size, size, nearVal, farVal);
	eyePosition = mat4tovec3(translationMatrix * rotationMatrix,vec3(0.0f, 0.0f, 10.0f));
	eyeLookPosition = vec3(0.0f, 0.0f, 0.0f);
	vec3 up = vec3(0, 1, 0);
	gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z, 
		eyeLookPosition.x, eyeLookPosition.y, eyeLookPosition.z, 
		up.x, up.y,up.z);
	GLfloat light0_position[] = { lightdir[0], lightdir[1], lightdir[2], 0 };
	GLfloat light1_position[] = { -lightdir[0], -lightdir[1], -lightdir[2], 0 };
	GLfloat light2_position[] = { lightdir[2], 0, -lightdir[0], 0 };
	GLfloat light3_position[] = { -lightdir[2], 0, lightdir[0], 0 };
	GLfloat light4_position[] = { 0, lightdir[2], -lightdir[1], 0 };
	GLfloat light5_position[] = { 0, -lightdir[2], lightdir[1], 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
}
/**
* ���oProjection Matrix�C
*/
mat4 CameraControl::GetProjectionMatrix()
{
	return GetProjectionMatrix(aspect);
}


/**
* ���o V * P ���x�}�C
* @param aspect �e�������e��C
*/
mat4 CameraControl::GetViewProjectionMatrix(float aspect)
{
	return GetProjectionMatrix(aspect) * viewMatrix;
}

/**
* ���o M * V * P ���x�}�C
* @param aspect �e�������e��C
*/
mat4 CameraControl::GetModelViewProjectionMatrix(float aspect)
{
	return GetViewProjectionMatrix(aspect) * GetModelMatrix();
}

/**
* ���o�ثe�۾��b�@�ɮy�Ъ���m�C
*/
vec3 CameraControl::GetWorldEyePosition() {
	vec4 wordEyePosition = vec4(eyePosition, 0) * GetModelMatrix();
	return wordEyePosition.xyz();
}

/**
* ���o�ثe�۾��b�@�ɮy�Ъ��Ҭݪ��I��m�C
*/
vec3 CameraControl::GetWorldViewVector() {
	vec4 wordLookVector = vec4(-viewVector, 0) * GetModelMatrix();
	return wordLookVector.xyz();
}

/**
* �B�z�������J�ɡA�۾����ʧ@�C
* @param key ��J������C
*/
void CameraControl::keyEvents(unsigned char key) {
	switch (key)
	{
		//�V�W���ʡC
	case 'w':
	case 'W':
		Translate(vec2(0, moveSpeed));
		break;

		//�V�����ʡC
	case 'a':
	case 'A':
		Translate(vec2(moveSpeed, 0));
		break;

		//�V�U���ʡC
	case 's':
	case 'S':
		Translate(vec2(0, -moveSpeed));
		break;

		//�V�k���ʡC
	case 'd':
	case 'D':
		Translate(vec2(-moveSpeed, 0));
		break;

		//��j�C
	case '+':
		wheelEvent(-moveSpeed);
		break;

		//�Y�p�C
	case '-':
		wheelEvent(moveSpeed);
		break;
	default:
		break;
	}
}

/**
* �B�z���ƹ��ƥ�ɡA�۾����ʧ@�C
* @param button ��J������C
* @param state ���䪺���A,�I�U,�u�_�C
* @param x ��J����, �ƹ��b�e����x�y�ЭȡC
* @param y ��J����, �ƹ��b�e����y�y�ЭȡC
*/
void CameraControl::mouseEvents(int button, int state, int x, int y) {

	if (state == GLUT_UP)
	{
		mouseReleaseEvent(button, x, y);
	}
	else if (state == GLUT_DOWN)
	{
		mousePressEvent(button, x, y);
	}

	//�B�z�ƹ�����V�W�u�ʮ�
	if (button == 4)
	{
		wheelEvent(1);
	}
	//�B�z�ƹ�����V�U�u�ʮ�
	else if (button == 3)
	{
		wheelEvent(-1);
	}
}

/**
* �B�z��ƹ������I�U�ɪ��ʧ@�C
* @param button �ƹ�������C
* @param x ��J����, �ƹ��b�e����x�y�ЭȡC
* @param y ��J����, �ƹ��b�e����y�y�ЭȡC
*/
void CameraControl::mousePressEvent(int button, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		//�����{�b����Q����
		lmbDown = true;
		lmbDownCoord = vec2(x, y);
		mat4 invrtRot = inverse(rotationMatrix);
		rotateYAxis = (invrtRot * vec4(0, 1, 0, 0)).xyz();
		rotateXAxis = (invrtRot * vec4(1, 0, 0, 0)).xyz();
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		//�����{�b����Q����
		midDown = true;
		midDownCoord = vec2(x, y);
	}
}

/**
* �B�z��ƹ�����u�_�ɪ��ʧ@�C
* @param button �ƹ�������C
* @param x ��J����, �ƹ��b�e����x�y�ЭȡC
* @param y ��J����, �ƹ��b�e����y�y�ЭȡC
*/
void CameraControl::mouseReleaseEvent(int button, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		lmbDown = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON || button == 3 || button == 4) {
		midDown = false;
	}
}

/**
* �B�z��ƹ����ʮɪ��ʧ@�C
* @param x �ƹ��b�e����x�y�ЭȡC
* @param y �ƹ��b�e����y�y�ЭȡC
*/
void CameraControl::mouseMoveEvent(int x, int y)
{
	if (lmbDown)
	{
		/*
		* ��ƹ��������,�i��즲�ɪ��ɭ�
		* �p�Ⲿ�ʪ��V�q,�i��۾�������
		*/
		vec2 coord = vec2(x, y);
		vec2 diff = coord - lmbDownCoord;
		float factor = 0.002f;
		rotationMatrix = rotate(rotationMatrix, diff.x * factor, rotateYAxis);
		rotationMatrix = rotate(rotationMatrix, diff.y * factor, rotateXAxis);
		lmbDownCoord = coord;
	}
	else if (midDown)
	{
		vec2 coord = vec2(x, y);
		vec2 diff = coord - midDownCoord;

		vec4 up = vec4(0, 1, 0, 0);
		vec4 right = vec4(1, 0, 0, 0);

		vec3 diffUp = up.xyz() * diff.y / (float)w_height;
		vec3 diffRight = right.xyz() * diff.x / (float)w_width;

		translationMatrix = translate(translationMatrix, (-diffUp + diffRight) * zoom * 3.0f);
		midDownCoord = coord;
	}
}

/**
* �ھڤ��䪺�u�ʤ�V�B�z�ƥ�C
* @param direction �e�u,��u�C
*/
void CameraControl::wheelEvent(int direction)
{
	wheel_val = direction * 15.0f;
	Zoom(wheel_val / 120.0f);
}

/**
* �ھڿ�J����,�վ�ثe���Y��ȡC
* @param distance �W�[���ȡC
*/
void CameraControl::Zoom(float distance)
{
	zoom *= (1.0f + 0.05f * distance);
	zoom = clamp(0.1f, zoom, 3.0f);
}

/**
* �i�D�۾��{�b���ù��j�p�C
* @param width �ù����e�C
* @param height �ù������C
*/
void CameraControl::SetWindowSize(int width, int height) {
	w_width = width;
	w_height = height;
	projMatrix = GetProjectionMatrix();
	aspect = height / width;
}


/**
* �H�שԨ��]�w�۾�������C
* @param theta �שԨ���theta����C
* @param phi �שԨ���phi����C
*/
void CameraControl::SetRotation(float theta, float phi)
{
	rotationMatrix = mat4(1.0);
	rotationMatrix = rotate(rotationMatrix, theta, vec3(0, 1, 0));
	rotationMatrix = rotate(rotationMatrix, phi, vec3(1, 0, 0));
}

/**
* �H�שԨ��]�w�۾�������C
* @param x �שԨ���x�ȡC
* @param y �שԨ���y�ȡC
* @param z �שԨ���z�ȡC
*/
void CameraControl::SetRotation(float x, float y, float z)
{
	vec3 v(x, y, z);
	v = normalize(v);
	vec3 o(0, 0, 1);
	double angle = acos(dot(v, o));
	rotationMatrix = mat4(1.0);
	rotationMatrix = rotate(rotationMatrix, (float)angle, cross(o, v));
}

/**
* ���]�۾����]�w�C
*/
void CameraControl::Reset()
{
	wheel_val = 0.0f;
	zoom = 1.0f;
	translationMatrix = mat4(1.0);
	rotationMatrix = mat4(1.0);
}

/**
* �Ϭ۾����ʡC
* @param vec �Ϭ۾�����vec���C
*/
void CameraControl::Translate(vec2 vec) {
	vec2 diff = vec;

	vec4 up = vec4(0, 1, 0, 0);
	vec4 right = vec4(1, 0, 0, 0);

	vec3 diffUp = up.xyz() * diff.y / (float)w_height;
	vec3 diffRight = right.xyz() * diff.x / (float)w_width;

	translationMatrix = translate(translationMatrix, (-diffUp + diffRight) * zoom * 3.0f);
}

