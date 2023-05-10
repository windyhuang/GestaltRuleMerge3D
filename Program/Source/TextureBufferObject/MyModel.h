#pragma once

#include <Common.h>

class MyModel
{
public:
	MyModel();
	~MyModel();

	bool Init(std::string fileName);
	void Render();

private:
	int indiceCount;
	GLuint vao;
	GLuint ebo;
	GLuint vboVertices, vboNormal, vboTexCoord;
};

