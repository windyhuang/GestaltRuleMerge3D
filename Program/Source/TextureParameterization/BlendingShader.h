#pragma once

#include <ShaderObject.h>

class BlendingShader: public ShaderObject
{
public:
	BlendingShader();
	~BlendingShader();
	void Enable();
	void Enable(GLuint modeltexture, GLuint linetexture);
	bool Init();
	//void SetWidth(const int width);
	//void SetHeight(const int pixel);

private:
	GLuint width, height;
};

