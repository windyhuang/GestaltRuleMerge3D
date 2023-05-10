#pragma once

#include <ShaderObject.h>

class DrawModelLineShader : public ShaderObject
{
public:
	DrawModelLineShader();
	~DrawModelLineShader();

	bool Init();
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void SetNormalMat(const glm::mat3& mat);
	void SetColor(const glm::vec4& Color);
	void Enable();
	void Disable();

private:
	GLuint um4pLocation;
	GLuint um4mvLocation;
	GLuint um3nLocation;
	GLuint tbo, tboTex;
	GLuint colorLocation;

};

