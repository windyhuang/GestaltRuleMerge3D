#include <ResourcePath.h>
#include "CubemapShader.h"


CubemapShader::CubemapShader()
{
}


CubemapShader::~CubemapShader()
{
}

bool CubemapShader::Init(const std::vector<CubemapTexture>& textures)
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, ResourcePath::shaderPath + "cubemap.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, ResourcePath::shaderPath + "cubemap.fs.glsl"))
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

	CreateBox();

	return LoadTextures(textures);
}

void CubemapShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4mvLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void CubemapShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(um4pLocation, 1, GL_FALSE, glm::value_ptr(mat));
}

void CubemapShader::Render()
{
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindVertexArray(0);
}

void CubemapShader::UseTexture(bool use)
{
	if (use)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}
	else
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

bool CubemapShader::LoadTextures(const std::vector<CubemapTexture>& textures)
{
	if (textures.size() != 6)
	{
		return false;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < textures.size(); ++i)
	{
		//https://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping
		TextureData texData = Common::Load_png(textures[i].fileName.c_str());
		if (texData.data != nullptr)
		{
			glTexImage2D(textures[i].type, 0, GL_RGBA, texData.width, texData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data);
			delete[] texData.data;
		}
		else
		{
			printf("Load texture file error %s\n", textures[i].fileName.c_str());
			return false;
		}
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return Common::CheckGLError();
}

void CubemapShader::CreateBox()
{
	float vertices[] = {
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,
		-0.5, 0.5, -0.5,
		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5
	};

	int indices[] = {
		// posx
		1, 5, 6,
		6, 2, 1,
		// negx
		4, 0, 3,
		3, 7, 4,
		// posy
		3, 2, 6,
		6, 7, 3,
		// neg y
		4, 5, 1,
		1, 0, 4,
		// posz
		5, 4, 7,
		7, 6, 5,
		// negz
		0, 1, 2,
		2, 3, 0
	};

	//int indices[] = {
	//	// posx
	//	1, 2, 6,
	//	6, 5, 1,
	//	// negx
	//	4, 7, 3,
	//	3, 0, 4,
	//	// posy
	//	3, 7, 6,
	//	6, 2, 3,
	//	// neg y
	//	4, 0, 1,
	//	1, 5, 4,
	//	// posz
	//	0, 3, 2,
	//	2, 1, 0,
	//	// negz
	//	5, 6, 7,
	//	7, 4, 5
	//};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}