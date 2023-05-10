#pragma once

#include <ShaderObject.h>

class DrawModelShader: public ShaderObject
{
public:
	DrawModelShader();
	~DrawModelShader();

	bool Init();
	void SetMVMat(const glm::mat4& mat);
	void SetMMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void SetNormalMat(const glm::mat3& mat);
	void SetCameraPos(const glm::vec3& vec);
	void SetRefractRatio(float eta);
	void SetDrawMode(bool reflectOrRefract);

private:
	GLuint um4pLocation;
	GLuint um4mvLocation;
	GLuint um4mLocation;
	GLuint um3nLocation;
	GLuint cameraPosLocation;
	GLuint etaLocation;
	GLuint drawModeLocation;
};

