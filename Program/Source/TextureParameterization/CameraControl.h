#include <Common.h>
//#include"GlmMath.h"
class CameraControl {
public:
	CameraControl();

	void mouseEvents(int button, int state, int x, int y);
	void mousePressEvent(int button, int x, int y);
	void mouseReleaseEvent(int button, int x, int y);
	void mouseMoveEvent(int x, int y);
	void keyEvents(unsigned char key);
	void wheelEvent(int direction);
	void Translate(glm::vec2 vec);

	glm::mat4 GetModelMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetProjectionMatrix(float aspect);
	void SetupGL();
	glm::mat4 GetViewProjectionMatrix(float aspect);
	glm::mat4 GetModelViewProjectionMatrix(float aspect);
	glm::vec3 GetEyePosition() { return eyePosition; }
	glm::vec3 GetViewVector() { return viewVector; }

	glm::vec3 GetWorldEyePosition();
	glm::vec3 GetWorldViewVector();

	bool IsOrthoProjection() { return ortho; }

	void SetRotation(float theta, float phi);
	void SetRotation(float x, float y, float z);
	void SetWindowSize(int width, int height);

	bool ToggleOrtho() { return ortho = !ortho; }
	void Zoom(float distance);
	void Reset();
	glm::vec3 mat4tovec3(glm::mat4 xf, glm::vec3 v) {
		float* pSource = glm::value_ptr(xf);
		float h = pSource[3] * v.x + pSource[7] * v.y + pSource[11] * v.z + pSource[15];
		h = 1 / h;

		return glm::vec3(float(h * (pSource[0] * v.x + pSource[4] * v.y + pSource[8] * v.z + pSource[12])),
			float(h * (pSource[1] * v.x + pSource[5] * v.y + pSource[9] * v.z + pSource[13])),
			float(h * (pSource[2] * v.x + pSource[6] * v.y + pSource[10] * v.z + pSource[14])));
	}
private:
	float aspect;					///< �x�s�ثe���������e��C
	bool ortho;						///< �O�_�ϥΥ�������C
	float zoom;
	float moveSpeed;				///< �۾������ʳt�סC
	glm::vec3 lightdir;
	glm::mat4 translationMatrix;	///< ����Translate�ʧ@��Matrix�C
	glm::mat4 rotationMatrix;		///< ����Rotation�ʧ@��Matrix�C
	glm::mat4 viewMatrix;			///< ����ViewMatrix�C
	glm::mat4 projMatrix;			///< ����projMatrix�C
	glm::vec3 viewVector;			///< �����۾��ݩ��J�I�ݪ��V�q�C
	glm::vec3 rotateXAxis;			///< �����۾���X�b����C
	glm::vec3 rotateYAxis;			///< �����۾���Y�b����C
	glm::vec3 eyePosition;			///< �����۾�����m�C
	glm::vec3 eyeLookPosition;		///< �����۾����Ҭݪ���m�C

	bool lmbDown;					///< �����ƹ�����O�_�Q����C
	bool midDown;					///< �����ƹ�����O�_�Q����C
	glm::vec2 lmbDownCoord;			///< �����ƹ������I���ɪ��y�СC
	glm::vec2 midDownCoord;			///< �����ƹ������I���ɪ��y�СC

	int w_width;					///< �����ù����e�C
	int w_height;					///< �����ù������C
	float wheel_val;				///< �����u�����ȡC
};