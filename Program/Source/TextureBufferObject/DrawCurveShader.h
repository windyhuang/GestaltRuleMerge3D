#pragma once

#include <ShaderObject.h>
#include <vector>

class DrawCurveShader: public ShaderObject
{
public:
	DrawCurveShader();
	~DrawCurveShader();

	bool Init(const std::vector<glm::vec3>& controlPoints);
	bool CreateTBO(const std::vector<glm::vec3>& controlPoints);
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void SetParamMat(const glm::mat4& mat);
	void SetLineDiv(int lineDiv);
	void SetDrawColor(const glm::vec4& color);
	void Render();

private:
	GLuint tbo;
	GLuint tboTex;
	GLuint paramMatrixLoc;
	GLuint um4mvLocation;
	GLuint um4pLocation;
	GLuint lineDivLoc;
	GLuint drawLineLoc;
	GLuint drawColorLoc;

	int pointCount;
	int lineDiv;
	glm::vec4 drawColor;
};

