#pragma once

#include <ShaderObject.h>

class BasicShader : public ShaderObject
{
public:
	BasicShader();
	~BasicShader();

	bool Init();
	

private:
	GLuint mvLocation;
	GLuint pLocation;
};

