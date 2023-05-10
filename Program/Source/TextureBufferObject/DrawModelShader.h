#pragma once

#include <ShaderObject.h>

class DrawModelShader: public ShaderObject
{
public:
	DrawModelShader();
	~DrawModelShader();

	bool Init();
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void Enable();
	void Disable();

private:
	GLuint um4pLocation;
	GLuint um4vLocation;
	GLuint tbo, tboTex;

	bool CreateTBO();
	bool UpdateTBO();
};

