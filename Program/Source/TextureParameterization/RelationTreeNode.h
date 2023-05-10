#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include "../../Include/GLM/glm/glm.hpp"
#include <string>
//#include "VecMath.h"
using namespace std;
using namespace glm;
struct BoundingBox {
public:
	vec3 min=vec3(0,0,0); vec3 max = vec3(0, 0, 0);
	int subObject[6];//minx,miny,minz,maxx,maxy,maxz
	float volume = (max[0] - min[0]) * (max[1] - min[1]) * (max[2] - min[2]);//體積
};
struct CalculationRule {
	vec3 start, midcontrol, end;
	int size;
	int raise;
};

class RelationTreeNode {
public:
	//geometry
	bool hasGeometry = false;
	bool isitContinue = false;
	
	//GeotryTreeNode* getry;
	//存取上下關係
	std::vector < RelationTreeNode> child;
	RelationTreeNode *parent;
	//存取左右關係
	std::vector < RelationTreeNode> left;
	std::vector < RelationTreeNode> right;
	/* To allow for larger scale changes in the split rules, we often want to tile a specified element.
	RelationTreeNode pos to find the line
	Example :s-Bézier line can be include inside
	*/
	//存取ParametricNode
	vector<ParametricNode> parametricNode;
	std::vector<RelationTreeNode> repeat;
	//Rule
	bool CalculationCubicBeizer = false;
	CalculationRule cRule;
	CalculationRule CalculationCubicBeizerPointRule(vec3 start, vec3 midcontrol, vec3 end,int size,	int raise) {
		cRule.start = start;
		cRule.midcontrol = midcontrol;
		cRule.end = end;
		cRule.size = size;
		cRule.raise = raise;
		return cRule;
	};
	//Data
	vec3 centerPoint;
	std::vector< std::vector<vec3>> modelOutPut;	
	std::vector<vec3> samplePointPos;//儲存點
	std::vector<vec3> samplePointNomal;//儲存點
	std::vector<std::vector<vec3>> linePoint;//儲存
	std::vector<vec3> axis;//影響的軸線,若是以中心為根據會有原始長度
	float hierarchy = 0;//hierarchy權重
	float gestalt = 0;//gestalt權重
	float angle[3] = {0,0,0};
	bool geomtrycontrol[3] = { false,false,false };//控制三軸長x高y寬z是否改變，預設為否，代表使用物件的原始長寬，若修改為true代表以計算的長度做變化
	//規則是否可合併
	bool isTheRuleSame = false;
	vec3 color;
	BoundingBox boundingbox;
	//基本線段的設立規則(自動)
	//Geomtry
	bool showsampleline = true;
	int objType=1000;
	vector<int> objTypes[2];
	string name;
	bool showcountour=true;
	vec3 objectrotate=vec3(0,0,0);
	
	void linebulid(vec3 center, int x, int y, int z) {
		axis.push_back(vec3(x,y,z));
		std::vector<vec3> line;
		vec3 a = center +vec3(x / 2,-y/2, z / 2);		
		vec3 b = center+vec3(x / 2, y/2, z / 2);		
		line.push_back(a);
		line.push_back(b);
		linePoint.push_back(line);
		line.clear();
		vec3 c = center + vec3(-x / 2,-y/2, z / 2);
		vec3 d = center + vec3(-x / 2, y / 2, z / 2);
		line.push_back(a);
		line.push_back(c);
		linePoint.push_back(line);
		line.clear();
		line.push_back(c);
		line.push_back(d);
		linePoint.push_back(line);
		line.clear();
		line.push_back(b);
		line.push_back(d);
		linePoint.push_back(line);
		line.clear();

		vec3 ee = center + vec3(x / 2,-y/2, -z / 2);
		vec3 f = center + vec3(x / 2, y/2, -z / 2);
		line.push_back(a);
		line.push_back(ee);
		linePoint.push_back(line);
		line.clear();
		line.push_back(ee);
		line.push_back(f);
		linePoint.push_back(line);
		line.clear();
		line.push_back(b);
		line.push_back(f);
		linePoint.push_back(line);
		line.clear();

		vec3 g = center + vec3(-x / 2,-y/2, -z / 2);
		line.push_back(g);
		line.push_back(ee);
		linePoint.push_back(line);
		line.clear();
		line.push_back(g);
		line.push_back(c);
		linePoint.push_back(line);
		line.clear();

		vec3 h = center + vec3(-x / 2, y/2, -z / 2);
		line.push_back(g);
		line.push_back(h);
		linePoint.push_back(line);
		line.clear();
		line.push_back(h);
		line.push_back(f);
		linePoint.push_back(line);
		line.clear();
		line.push_back(h);
		line.push_back(d);
		linePoint.push_back(line);
		line.clear();


	}

	void linebulid(std::vector<vec3> samplePointPos) {
		for (int i = 0; i < samplePointPos.size()-1; i++) {
			std::vector<vec3> line;
			line.push_back(samplePointPos[i]);
			line.push_back(samplePointPos[i+1]);
			linePoint.push_back(line);
		}
	}


	void boundingboxinformation() {
		/*for (int i = 0; i < sizeof(boundingbox); i++) {
			boundingbox=new Boundingbox
		}*/
		for (int i = 0; i < child.size(); i++) {
			for (int j = 0; j < child[i].samplePointPos.size();j++) {
				if (child[i].samplePointPos[j][0] < boundingbox.min[0]) {
					boundingbox.min[0] = child[i].samplePointPos[j][0]; boundingbox.subObject[0] = i;
				}
				if (child[i].samplePointPos[j][1] < boundingbox.min[1]) { 
					boundingbox.min[1] = child[i].samplePointPos[j][1]; boundingbox.subObject[1] = i;
				}
				if (child[i].samplePointPos[j][2] < boundingbox.min[2]) { 
					boundingbox.min[2] = child[i].samplePointPos[j][2]; boundingbox.subObject[2] = i;
				}
				if (child[i].samplePointPos[j][0] > boundingbox.max[0]) {
					boundingbox.max[0] = child[i].samplePointPos[j][0]; boundingbox.subObject[3] = i;
				}
				if (child[i].samplePointPos[j][1] > boundingbox.max[1]) { 
					boundingbox.max[1] = child[i].samplePointPos[j][1]; boundingbox.subObject[4] = i;
				}
				if (child[i].samplePointPos[j][2] > boundingbox.max[2]) { 
					boundingbox.max[2] = child[i].samplePointPos[j][2]; boundingbox.subObject[5] = i;
				}
			}
		}
	}

	bool int_ptr_less(int* a, int* b)
	{
		return *a < *b;
	}

	vec3 boundingboxmin(vec3 a, vec3 b) {
		vec3 rp=min(a,b);
		return rp;
	}

	vec3 boudinbboxmax(vec3 a, vec3 b) {
		vec3 rp=max(a,b);
		return rp;
	}



	/// <summary>
	/// 以線段為長寬的規則「需儲存：線段、長度或寬度、影響的軸線」
	/// 輸入為各軸的原始長寬,及需變化的位置
	/// </summary>
	/// <returns>軸線，長度或寬度變化</returns>
	std::vector< std::vector<vec3>> ByLineRule(std::vector<vec3> modelsize, std::vector<int> modelChange) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		returnValue.resize(linePoint[0].size());//輸出的數量應與線條的一致
		int change = 1;//第一個模型都是0開始，預設為1代表從第二個開始轉換不同位模型開始
		int model = 0;//設定第一個模型開始
		for (int i = 0; i < linePoint[0].size(); i++) {
			vec3 pos = (linePoint[0][i] + linePoint[1][i]) / vec3(2, 2, 2);
			returnValue[i].push_back(pos);
			vec3 scale;
			if (i == modelChange[change]) {
				model++;
				if (change < modelChange.size())
					change++;
			}
			for (int j = 0; j < 3; j++) {
				if (axis[i][j] == 0 || axis[i][j] == 1)
					scale[j] = 1;
				else if (axis[i][j] > 1) {
					scale[j] = sqr((linePoint[0][i][j] - linePoint[1][i][j]) * (linePoint[0][i][j] - linePoint[1][i][j]));
					scale[j] /= modelsize[model][j];
				}
			}

			returnValue[i].push_back(scale);
		}
		
		modelOutPut = returnValue;
		return returnValue;
	}
	/// <summary>
	/// 以線段為長寬的規則「需儲存：線段、長度或寬度、影響的軸線」
	/// 當輸入只有一個模型時
	/// </summary>
	/// <param name="modelsize"></param>
	/// <param name="modelChange"></param>
	/// <returns></returns>
	std::vector< std::vector<vec3>> ByLineRule(vec3 modelsize) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		returnValue.resize(linePoint[0].size());//輸出的數量應與線條的一致

		for (int i = 0; i < linePoint[0].size(); i++) {
			vec3 pos = (linePoint[0][i] + linePoint[1][i]) / vec3(2, 2, 2);
			returnValue[i].push_back(pos);
			vec3 scale;

			for (int j = 0; j < 3; j++) {
				if (axis[i][j] == 0 || axis[i][j] == 1)
					scale[j] = 1;
				else if (axis[i][j] > 1) {
					scale[j] = sqrt((linePoint[0][i][j] - linePoint[1][i][j]) * (linePoint[0][i][j] - linePoint[1][i][j]));

					scale[j] /= modelsize[j];
				}
			}

			returnValue[i].push_back(scale);
		}

		
		modelOutPut = returnValue;
		return returnValue;
	}
	/// <summary>
	/// 以中心點為依據並已知長寬的規則「需儲存：中心點位置、是否需縮放、長寬、影響的軸線、其他規則」
	/// </summary>
	/// <returns>中心點，軸線，長度或寬度變化</returns>
	std::vector< std::vector<vec3>> CenterPointRule(std::vector<vec3> modelsize, std::vector<int> modelChange) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		returnValue.resize(axis.size());//輸出的數量應與的一致
		int change = 1;//第一個模型都是0開始，預設為1代表從第二個開始轉換不同位模型開始
		int model = 0;//設定第一個模型開始
		for (int i = 0; i < axis.size(); i++) {
			vec3 pos = centerPoint;
			returnValue[i].push_back(pos);
			vec3 scale;
			if (i == modelChange[change]) {
				model++;
				if (change < modelChange.size())
					change++;
			}
			for (int j = 0; j < 3; j++) {
				if (axis[i][j] == 1) {
					//scale[j] = sqr(linePoint[0][i][j] - linePoint[1][i][j]);
					scale[j] = 1;
				}
				else if (axis[i][j] > 1)scale[j] = axis[i][j] / modelsize[model][j];
			}

			returnValue[i].push_back(scale);
		}
		
		return returnValue;
	}
	/// <summary>
	/// 以中心點為依據並已知長寬的規則「需儲存：中心點位置、是否需縮放、長寬、影響的軸線、其他規則」
	/// 當輸入只有一個模型時
	/// </summary>
	/// <param name="modelsize"></param>
	/// <returns></returns>
	std::vector< std::vector<vec3>> CenterPointRule(vec3 modelsize) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		returnValue.resize(axis.size());//輸出的數量應與的一致

		for (int i = 0; i < axis.size(); i++) {
			vec3 pos = centerPoint;
			returnValue[i].push_back(pos);
			vec3 scale;

			for (int j = 0; j < 3; j++) {
				if (axis[i][j] == 1) {
					//scale[j] = sqr(linePoint[0][i][j] - linePoint[1][i][j]);
					scale[j] = 1;
				}
				else if (axis[i][j] > 1)scale[j] = axis[i][j] / modelsize[j];
			}

			returnValue[i].push_back(scale);
		}
		
		modelOutPut = returnValue;
		return returnValue;
	}



	/// <summary>
	/// 根據geometry來找點，並且點有相應的法向量位置「需儲存：所有的點、法向量」
	/// 若線段比geometry長使用線段做縮放依據
	/// </summary>
	/// <returns>中心點，軸線，長度或寬度變化</returns>
	std::vector< std::vector<vec3>> GeometryOrLineRule(std::vector<vec3> modelsize, std::vector<int> modelChange) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		//從開頭找適合的點去match算geometry
		int startnum = 0, linenum = 0;

		//returnValue.resize(axis.size());//輸出的數量應與的一致
		int change = 1;//第一個模型都是0開始，預設為1代表從第二個開始轉換不同位模型開始
		int model = 0;//設定第一個模型開始

		while (linenum <= linePoint[0].size()) {
			std::vector<vec3> stepValue;
			//point pos = centerPoint;
			//returnValue[i].push_back(pos);
			vec3 scale;
			float distance = 0;
			float modeldisantce = 0;
			int changAxis = 0;
			for (int j = 0; j < 3; j++) {
				if (axis[0][j] == 1) {
					//scale[j] = sqr(linePoint[0][i][j] - linePoint[1][i][j]);
					scale[j] = 1;
				}
				else if (axis[0][j] > 1) {
					modeldisantce = modelsize[model][j]; changAxis = j;
					scale[j] = 1;
				}
			}
			while (distance >= modeldisantce) {
				if (linenum < linePoint[0].size())
				{
					distance += dist(linePoint[0][linenum], linePoint[1][linenum]);
				}
				linenum++;
			}
			vec3 pos = linePoint[0][(startnum + linenum) / 2];
			stepValue.push_back(pos);
			if (returnValue.size() == modelChange[change]) {
				model++;
				if (change < modelChange.size())
					change++;
			}


			stepValue.push_back(scale);
			returnValue.push_back(stepValue);
		}
		
		modelOutPut = returnValue;
		return returnValue;
	}
	/// <summary>
	/// 根據geometry來找點，並且點有相應的法向量位置「需儲存：所有的點、法向量」
	/// 若線段比geometry長使用線段做縮放依據
	/// </summary>
	/// <returns>中心點，軸線，長度或寬度變化</returns>
	std::vector< std::vector<vec3>> GeometryOrLineRule(vec3 modelsize) {
		std::vector< std::vector<vec3>> returnValue;//回傳值前者為位置,後者為調整後的長寬
		//從開頭找適合的點去match算geometry
		int startnum = 0, linenum = 0;

		//returnValue.resize(axis.size());//輸出的數量應與的一致
		int change = 1;//第一個模型都是0開始，預設為1代表從第二個開始轉換不同位模型開始
		int model = 0;//設定第一個模型開始

		while (linenum <= linePoint[0].size()) {
			std::vector<vec3> stepValue;
			//point pos = centerPoint;
			//returnValue[i].push_back(pos);
			vec3 scale;
			float distance = 0;
			float modeldisantce = 0;
			for (int j = 0; j < 3; j++) {
				if (axis[0][j] == 1) {
					//scale[j] = sqr(linePoint[0][i][j] - linePoint[1][i][j]);
					scale[j] = 1;
				}
				else if (axis[0][j] > 1) {
					modeldisantce = modelsize[j]; scale[j] = 1;
				}
			}
			while (distance <= modeldisantce) {
				if (linenum < linePoint[0].size())
				{
					distance += dist(linePoint[0][linenum], linePoint[1][linenum]);
				}
				linenum++;
			}
			vec3 pos = linePoint[0][(startnum + linenum) / 2];
			startnum = linenum;
			stepValue.push_back(pos);

			stepValue.push_back(scale);
			returnValue.push_back(stepValue);
		}
		
		modelOutPut = returnValue;
		return returnValue;
	}
};
