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

	um4vLocation = GetUniformLocation("um4v");
	if (um4vLocation == -1)
	{
		puts("Get uniform loaction error: um4v");
		return false;
	}

	um4pLocation = GetUniformLocation("um4p");
	if (um4pLocation == -1)
	{
		puts("Get uniform loaction error: um4p");
		return false;
	}

	return CreateTBO();
}

void DrawModelShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4vLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void DrawModelShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

bool DrawModelShader::CreateTBO()
{
	// create texture buffer object
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);

	glm::mat4 *transform = new glm::mat4[10000];
	glm::mat4 *ptrBegin = transform;
	for (int row = 0; row < 100; ++row)
	{
		for (int col = 0; col < 100; ++col)
		{
			glm::mat4 transformMat;
			transformMat = glm::rotate(transformMat, glm::radians((float)(row * col)), glm::vec3(1.0f, 1.0f, 1.0f));
			transformMat = glm::translate(transformMat, glm::vec3(col * 2.0f - 100.0f, 0, row * 2.0f - 100.0f));
			*ptrBegin++ = transformMat;
		}
	}

	glBufferData(GL_TEXTURE_BUFFER, 10000 * sizeof(glm::mat4), transform, GL_DYNAMIC_COPY);
	delete[] transform;

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// bind texture buffer object to texture
	glGenTextures(1, &tboTex);
	glBindTexture(GL_TEXTURE_BUFFER, tboTex);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tbo);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return Common::CheckGLError();
}

bool DrawModelShader::UpdateTBO()
{
	glBindBuffer(GL_TEXTURE_BUFFER, tbo);

	void *ptr = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	glm::mat4 *tranformMat = (glm::mat4*)ptr;

	for (int i = 0; i < 10000; ++i)
	{
		tranformMat[i] = glm::rotate(tranformMat[i], glm::radians(5.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

	glUnmapBuffer(GL_TEXTURE_BUFFER);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	return Common::CheckGLError();
}

void DrawModelShader::Enable()
{

	UpdateTBO();
	ShaderObject::Enable();
	glBindTexture(GL_TEXTURE_BUFFER, tboTex);
}

void DrawModelShader::Disable()
{
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	ShaderObject::Disable();
}
