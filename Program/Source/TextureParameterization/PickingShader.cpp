#include "PickingShader.h"
#include "ResourcePath.h"


PickingShader::PickingShader()
{
}


PickingShader::~PickingShader()
{
}

bool PickingShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "picking.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "picking.fs.glsl"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	
	
	return true;
}

