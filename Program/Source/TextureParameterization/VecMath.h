#include "../Include/GLM/glm/gtc/type_ptr.hpp"
#include <cmath>
#include <algorithm>

using namespace glm;



// Utility functions for square and cube, to go along with sqrt and cbrt
template <class T>
static inline T sqr(const T& x)
{
	return x * x;
}

float len2(vec3& v)
{
	float l2 = v[0] * v[0];
	for (int i = 1; i < 2; i++)
		l2 += v[i] * v[i];
	return l2;
}

float len(vec3 v)
{
	return sqrt(len2(v));
}

vec3 fabs(vec3& v)
{
	vec3 result(v);
	for (int i = 0; i < 3; i++)
		if (result[i] < v.x)
			result[i] = -result[i];
	return result;
}
float dist2(vec3& v1,vec3& v2)
{
	float d2 = sqr(v2[0] - v1[0]);
	for (int i = 1; i < 3; i++)
		d2 += sqr(v2[i] - v1[i]);
	return d2;
}


float dist(vec3& v1, vec3& v2)
{
	return sqrt(dist2(v1, v2));
}

vec3 get_normal(vec3 x0, vec3 x1, vec3 x2)
{
	vec3 v0 = x0 - x2;
	vec3 v1 = x1 - x2;
	vec3 n = cross(v0, v1); //v0 % v1;

	return normalize(n);
}