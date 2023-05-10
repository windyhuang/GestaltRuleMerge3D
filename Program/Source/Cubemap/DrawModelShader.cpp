#include "DrawModelShader.h"
#include "ResourcePath.h"


DrawModelShader::DrawModelShader()
{
}


DrawModelShader::~DrawModelShader()
{
}

bool DrawModelShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "drawModel.vs.glsl"))
	{
		return false;
	}
	
	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "drawModel.fs.glsl"))
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

	um4mLocation = GetUniformLocation("um4m");
	if (um4mLocation == -1)
	{
		puts("Get uniform loaction error: um4m");
		return false;
	}

	um4pLocation = GetUniformLocation("um4p");
	if (um4pLocation == -1)
	{
		puts("Get uniform loaction error: um4p");
		return false;
	}

	um3nLocation = GetUniformLocation("um3n");
	if (um3nLocation == -1)
	{
		puts("Get uniform loaction error: um3n");
		return false;
	}

	cameraPosLocation = GetUniformLocation("cameraPos");
	if (cameraPosLocation == -1)
	{
		puts("Get uniform loaction error: cameraPos");
		return false;
	}

	etaLocation = GetUniformLocation("eta");
	if (etaLocation == -1)
	{
		puts("Get uniform loaction error: eta");
		return false;
	}

	drawModeLocation = GetUniformLocation("reflectOrRefract");
	if (drawModeLocation == -1)
	{
		puts("Get uniform loaction error: drawMode");
		return false;
	}

	return true;
}

void DrawModelShader::SetMMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4mLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4mvLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelShader::SetNormalMat(const glm::mat3& mat)
{
	glUniformMatrix3fv(um3nLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelShader::SetCameraPos(const glm::vec3& vec)
{
	glUniform3fv(cameraPosLocation, 1, glm::value_ptr(vec));
}

void DrawModelShader::SetRefractRatio(float eta)
{
	glUniform1f(etaLocation, eta);
}

void DrawModelShader::SetDrawMode(bool reflectOrRefract)
{
	glUniform1i(drawModeLocation, reflectOrRefract);
}