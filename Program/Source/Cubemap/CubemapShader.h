#pragma once

#include <ShaderObject.h>
#include <vector>

typedef struct CubemapTexture
{
	GLenum type;
	std::string fileName;
};

class CubemapShader: public ShaderObject
{
public:
	CubemapShader();
	~CubemapShader();

	bool Init(const std::vector<CubemapTexture>& textures);
	bool LoadTextures(const std::vector<CubemapTexture>& textures);
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void Render();
	void UseTexture(bool use);

private:
	GLuint textureID;
	GLuint um4mvLocation;
	GLuint um4pLocation;
	GLuint vao, vbo, ebo;

	void CreateBox();
};

