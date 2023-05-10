#include "DrawCurveShader.h"
#include <ResourcePath.h>


DrawCurveShader::DrawCurveShader()
{
	pointCount = 0;
	lineDiv = 100;
	drawColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}


DrawCurveShader::~DrawCurveShader()
{
}

bool DrawCurveShader::Init(const std::vector<glm::vec3>& controlPoints)
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "drawCurve.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "drawCurve.fs.glsl"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	

	um4mvLocation = GetUniformLocation("um4mv");
	if (um4mvLocation == -1)
	{
		puts("Get uniform loaction error: um4mv");
		return false;
	}

	um4pLocation = GetUniformLocation("um4p");
	if (um4pLocation == -1)
	{
		puts("Get uniform loaction error: um4p");
		return false;
	}

	lineDivLoc = GetUniformLocation("lineDiv");
	if (lineDivLoc == -1)
	{
		puts("Get uniform loaction error: lineDiv");
		return false;
	}

	paramMatrixLoc = GetUniformLocation("paramMatrix");
	if (paramMatrixLoc == -1)
	{
		puts("Get uniform loaction error: paramMatrix");
		return false;
	}

	drawLineLoc = GetUniformLocation("drawLine");
	if (drawLineLoc == -1)
	{
		puts("Get uniform loaction error: drawLine");
		return false;
	}

	drawColorLoc = GetUniformLocation("drawColor");
	if (drawColorLoc == -1)
	{
		puts("Get uniform loaction error: drawColor");
		return false;
	}

	return CreateTBO(controlPoints);
}

bool DrawCurveShader::CreateTBO(const std::vector<glm::vec3>& controlPoints)
{
	if (controlPoints.size() < 4)
	{
		puts("Error: controlPoints size < 4");
		return false;
	}
	pointCount = controlPoints.size();

	// create texture buffer object
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);

	float *points = new float[pointCount * 4];
	float *ptrBegin = points;
	for (int i = 0; i < pointCount; ++i)
	{
		*ptrBegin++ = controlPoints[i].x;
		*ptrBegin++ = controlPoints[i].y;
		*ptrBegin++ = controlPoints[i].z;
		*ptrBegin++ = 1.0;
	}

	glBufferData(GL_TEXTURE_BUFFER, pointCount * 4 * sizeof(float), points, GL_STATIC_DRAW);
	delete[] points;

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// bind texture buffer object to texture
	glGenTextures(1, &tboTex);
	glBindTexture(GL_TEXTURE_BUFFER, tboTex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tbo);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return Common::CheckGLError();
}

void DrawCurveShader::SetMVMat(const glm::mat4 & mat)
{
	glUniformMatrix4fv(um4mvLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawCurveShader::SetPMat(const glm::mat4 & mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawCurveShader::SetParamMat(const glm::mat4 & mat)
{
	glUniformMatrix4fv(paramMatrixLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawCurveShader::SetLineDiv(int lineDiv)
{
	glUniform1i(lineDivLoc, lineDiv);
	this->lineDiv = lineDiv;
}

void DrawCurveShader::SetDrawColor(const glm::vec4 & color)
{
	drawColor = color;
	glUniform4fv(drawColorLoc, 1, glm::value_ptr(drawColor));
}

void DrawCurveShader::Render()
{
	glBindTexture(GL_TEXTURE_BUFFER, tbo);

	glLineWidth(5.0f);
	glUniform1i(drawLineLoc, 1);
	drawColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	glUniform4fv(drawColorLoc, 1, glm::value_ptr(drawColor));
	glDrawArrays(GL_LINE_STRIP, 0, pointCount * lineDiv);
	glLineWidth(1.0f);

	glPointSize(30.0f);
	glUniform1i(drawLineLoc, 0);
	drawColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glUniform4fv(drawColorLoc, 1, glm::value_ptr(drawColor));
	glDrawArrays(GL_POINTS, 0, pointCount);
	glPointSize(1.0f);

	glBindTexture(GL_TEXTURE_BUFFER, 0);
	

}
