#include "BlendingShader.h"
#include "ResourcePath.h"


BlendingShader::BlendingShader()
{
}


BlendingShader::~BlendingShader()
{
}

void BlendingShader::Enable()
{
	ShaderObject::Enable();
}

void BlendingShader::Enable(GLuint modeltexture, GLuint linetexture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glBindTexture(GL_TEXTURE_2D, linetexture);
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glUniform1i(texture1,0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, linetexture);
	glUniform1i(texture2, 1);*/
}

bool BlendingShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "Blending_Example.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "Blending_Example.fs.glsl"))
	{
		return false;
	}

	
	if (!Finalize())
	{
		return false;
	}

	
	return true;
}
