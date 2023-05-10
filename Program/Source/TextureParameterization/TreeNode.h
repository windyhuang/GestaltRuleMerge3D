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
	TreeNode *parent;
	vector<TreeNode> childNodes; //&
	ParametricNode parmetricNode;
	vec3 normal;
	BoundingBox boundbox;
	float volume;
	bool inChildNode = false;
	bool proximity = false;
	bool similarity = false;
	int proximityG, similarityG, proximityJ, similarityJ;
	//bool inChildNode=false;
	//TreeNode();
};