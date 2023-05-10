#include "Bezier_ProceduralShader.h"
#include "ResourcePath.h"


Bezier_ProceduralShader::Bezier_ProceduralShader()
{
}


Bezier_ProceduralShader::~Bezier_ProceduralShader()
{
}

bool Bezier_ProceduralShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "bezier_Procedural.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_TESS_CONTROL_SHADER, ResourcePath::shaderPath + "bezier_Procedural.tcs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_TESS_EVALUATION_SHADER, ResourcePath::shaderPath + "bezier_Procedural.tes.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, ResourcePath::shaderPath + "bezier_Procedural.gs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "bezier_Procedural.fs.glsl"))
	{
		return false;
	}


	if (!Finalize())
	{
		return false;
	}

	

	BT = GetUniformLocation("BT");
	if (BT == -1)
	{
		puts("Get uniform loaction error: BT");
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
		puts("Get uniform loaction error: procedural-um3n");
		return false;
	}

	modelDepth= GetUniformLocation("depthTexture");
	if(modelDepth==-1)
	{
		puts("Get uniform loaction error: modelDepth");
		return false;
	}
	tessLevelOuter = GetUniformLocation("TessLevelOuter");
	if (tessLevelOuter == -1)
	{
		puts("Get uniform loaction error: TessLevelOuter");
		return false;
	}
	midpoint = GetUniformLocation("startpoint");
	if (tessLevelOuter == -1)
	{
		puts("Get uniform loaction error: startpoint");
		return false;
	}
	midpoint = GetUniformLocation("midpoint");
	if (tessLevelOuter == -1)
	{
		puts("Get uniform loaction error: midpoint");
		return false;
	}
	endpoint = GetUniformLocation("endpoint");
	if (tessLevelOuter == -1)
	{
		puts("Get uniform loaction error: endpoint");
		return false;
	}
	return true;
}

void Bezier_ProceduralShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4mvLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void Bezier_ProceduralShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void Bezier_ProceduralShader::SetNormalMat(const glm::mat3& mat)
{
	glUniformMatrix3fv(um3nLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void Bezier_ProceduralShader::SetBT(const glm::mat4& mat)
{
	glUniformMatrix4fv(BT, 1, GL_FALSE, glm::value_ptr(mat));
}

void Bezier_ProceduralShader::SetmodelDepth(GLuint modelDepth)
{

}

void Bezier_ProceduralShader::SetTessLevelOuter(float tessLevelOuter)
{
	glUniform1f(tessLevelOuter, tessLevelOuter);
}

void Bezier_ProceduralShader::Setstartpoint(glm::vec3 point)
{
	glUniform3fv(startpoint,1, glm::value_ptr(point));
}

void Bezier_ProceduralShader::Setmidpoint(glm::vec3 point)
{
	glUniform3fv(midpoint, 1, glm::value_ptr(point));
}

void Bezier_ProceduralShader::Setendpoint(glm::vec3 point)
{
	glUniform3fv(endpoint, 1, glm::value_ptr(point));
}
