#include "DrawModelLineShader.h"
#include "ResourcePath.h"


DrawModelLineShader::DrawModelLineShader()
{
}


DrawModelLineShader::~DrawModelLineShader()
{
}

bool DrawModelLineShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "drawLines.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "drawLines.fs.glsl"))
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

	um3nLocation = GetUniformLocation("um3n");
	if (um3nLocation == -1)
	{
		puts("Get uniform loaction error: um3n");
		return false;
	}
	colorLocation = GetUniformLocation("color");
	if (colorLocation == -1)
	{
		puts("Get uniform loaction error: wireColor");
		return false;
	}
	return  true;

}

void DrawModelLineShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4mvLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelLineShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelLineShader::SetNormalMat(const glm::mat3& mat)
{
	glUniformMatrix3fv(um3nLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelLineShader::Enable()
{


	ShaderObject::Enable();
	glBindTexture(GL_TEXTURE_BUFFER, tboTex);
}

void DrawModelLineShader::Disable()
{
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	ShaderObject::Disable();
}
void DrawModelLineShader::SetColor(const glm::vec4& Color)
{
	glUniform4fv(colorLocation, 1, glm::value_ptr(Color));
}