#pragma once
#include <vector>
#include <math.h>
#include "../../Include/GLM/glm/glm.hpp"
#include <string>
//#include "VecMath.h"
using namespace std;
using namespace glm;


class TreeNode {
public:
	string name;
	int treeNodeID;
	float lodt;
	TreeNode *parent;
	vector<TreeNode> childNodes; //&
	ParametricNode parmetricNode;
	vec3 normal;
	BoundingBox boundbox;
	float volume;
	bool lod = false;//¨Oß_∞ı¶ÊLOD
	bool inChildNode = false;
	bool proximity = false;
	bool similarity = false;
	int proximityG, similarityG, proximityJ, similarityJ;
	vector<vec3> boundingboxlines,convexboxline;
	//bool inChildNode=false;
	//TreeNode();
};