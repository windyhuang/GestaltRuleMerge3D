#pragma once

#include <ShaderObject.h>

class Bezier_ProceduralShader : public ShaderObject
{
public:
	Bezier_ProceduralShader();
	~Bezier_ProceduralShader();

	bool Init();
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void SetNormalMat(const glm::mat3& mat);
	void SetBT(const glm::mat4& BT);
	void SetmodelDepth(GLuint modelDepth);
	void SetTessLevelOuter(float tessLevelOuter);
	void Setstartpoint(glm::vec3 point);
	void Setmidpoint(glm::vec3 point);
	void Setendpoint(glm::vec3 point);
private:
	GLuint BT;
	GLuint um4pLocation;
	GLuint um4mvLocation;
	GLuint um3nLocation;
	GLuint modelDepth;
	GLuint startpoint,midpoint, endpoint;
	float tessLevelOuter;
	//glm::vec3 startpoint;
	//glm::vec3 midpoint;
	//glm::vec3 endpoint;
	 
};

