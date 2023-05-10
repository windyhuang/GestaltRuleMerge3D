
#include "../Include/GLM/glm/gtc/type_ptr.hpp"

using namespace glm;
glm::vec3 mat4tovec3(glm::mat4 xf, glm::vec3 v) {
	float* pSource = glm::value_ptr(xf);
	float h = pSource[3] * v.x + pSource[7] * v.y + pSource[11] * v.z + pSource[15];
	h = 1 / h;

	return glm::vec3(float(h * (pSource[0] * v.x + pSource[4] * v.y + pSource[8] * v.z + pSource[12])),
		float(h * (pSource[1] * v.x + pSource[5] * v.y + pSource[9] * v.z + pSource[13])),
		float(h * (pSource[2] * v.x + pSource[6] * v.y + pSource[10] * v.z + pSource[14])));
}