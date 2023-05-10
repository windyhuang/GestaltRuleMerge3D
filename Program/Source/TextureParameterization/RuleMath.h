#pragma once
#include <vector>
//#include "../Include/GLM/glm/gtc/type_ptr.hpp"
#include "VecMath.h"
#include"../../Include/GLM/glm/gtc/type_ptr.hpp"
using namespace glm;

vec3 findControlPointToCubicBezierPoint(float t, vec3 p0, vec3 controlpoint, vec3 p2)
{
	//求p1
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;


	vec3 p = uu * p0;
	p += tt * p2;
	p = controlpoint - p;
	vec3 p1;
	p1 = p / (2 * u * t);


	return p1;
}
vec3 CalculateCubicBezierPoint(float t, vec3 p0, vec3 p1, vec3 p2)
{
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;

	vec3 p = uu * p0;
	p += 2 * u * t * p1;
	p += tt * p2;

	return p;
}
std::vector<vec3> GetCubicBeizerList(vec3 startPoint, vec3 controlPoint, vec3 endPoint, int segmentNum)
{
	std::vector<vec3> pathend;
	//vec3[] path = new Vector3[segmentNum];

	for (int i = 1; i <= segmentNum; i++)
	{
		float t = i / (float)segmentNum;
		vec3 pixel = CalculateCubicBezierPoint(t, startPoint,
			controlPoint, endPoint);
		//path[i - 1] = pixel;
		pathend.push_back(pixel);

	}
	return pathend;
}
std::vector<vec3> CalculationCubicBeizerPoint(vec3 startPoint, vec3 controlPoint, vec3 endPoint, int segmentNum, int Raise)
{
	float distance = dist(startPoint, endPoint);
	std::cout << "distance" << distance << std::endl;
	std::vector<vec3> pathend;
	std::vector<vec3> pathall;
	if (Raise == 5) {
		float x = (startPoint.x - endPoint.x) / 2;//x方向長度，分割為2，e0位於1
		float y = (endPoint.y - startPoint.y) / 13;//y方向長度，分割為1.3，e0位於0.5
		vec3 e0 = vec3(x, y * 5, (startPoint.z + endPoint.z) / 2);
		//每舉高度比例
		float proportion = 5.0 / 7.0;//start to end
		vec3 returnPoint = findControlPointToCubicBezierPoint(proportion, startPoint, controlPoint, endPoint); //當t=0.5=controlpoint, 求p1
		//std::vector<point> pathend;
		//原來根據數量生成

		vec3 linelength = vec3(0, 0, 0);
		for (int i = 1; i <= 3000; i++)
		{
			float t = i / (float)3000;
			vec3 pixel = CalculateCubicBezierPoint(t, startPoint, returnPoint, endPoint);
			pathall.push_back(pixel);
			//計算與上一點的長度
			if (i > 1) {
				linelength += fabs(fabs(pixel) - fabs(pathall[i - 2]));
			}
		}
		//float lengths = len(linelength) / segmentNum + 10;
		vec3 linestep =vec3( linelength[0]/ (segmentNum *2), linelength[1] / (segmentNum *2), linelength[2] / (segmentNum *2) );
		int start = 0;
		for (int i = 0; i < segmentNum; i++) {
			vec3 startpoint = pathall[start];
			vec3 lenpoint;
			while (len(lenpoint) < len(linestep)) {//)&& lenpoint[1] >= linestep[1]&& lenpoint[2] >= linestep[2]
				start++;
				lenpoint += fabs(fabs(pathall[start]) - fabs(pathall[start - 1]));

			}
			lenpoint = vec3(0, 0, 0);
			//std::cout << "segmentNum" <<i<<"all"<< segmentNum << std::endl;
			//std::cout << "outputpoint" << start << std::endl;
			pathend.push_back(pathall[start]);
		}

	}

	return pathend;
}
