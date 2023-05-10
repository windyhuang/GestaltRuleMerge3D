#include <math.h>
#include "../../Include/GLM/glm/glm.hpp"
#include <string>
#include <vector>
#include <stack>
#include "MeshObject.h"
#include <string>
//#include "VecMath.h"
using namespace std;
using namespace glm;
enum Type { Repeat };

class Way {
public:
	int x=0;
	int y=0;
	int z=0;
	int creatnum;
	vec3 startpos;
};
class BezierWay {
public:
	vec3 start;
	vec3 mid;
	vec3 end;
	//是否有舉重
	int raise = 0;
};
class ParametricNode {

public:
	string name;
	Type type;
	Way generateWay;////生成方式
	int generateNum;
	
	vector<vec3> elementsPos;//元件位置	
	vector<vec3> generateDirection;//生成方向	
	BezierWay bezierWay;//貝茲的生成方式，並加入舉重以因應不同建築的變化
	vector<MeshObject> differentElements;
	vector<MeshObject> elementsLine;
	vector<ParametricNode> ParametricRule;
	vector<float> regularitynum;
	vector<vector<int>> sameObjectMerge;
	GLuint drawSameObj, drawSameObjV;

	
	//vector<MyMesh::EdgeIter> ans;
	vec3 generatePos; //生成位置
	vec3 generateNormal;//生成法向量
	//是否有交集
	bool intersection=false;
	int meshnum;
	bool proximity;
	bool similarity;
	float proximityT;
	float SimilarityT;
	float RegularityT;
	float gestaltT;
	void createElements();
	void createElements(vec3 start);
	void lineSegment();
	void immediateRender(vec3 campos);
	void sameObjectBuild(mat4 postions);
	void renderEachObj(mat4 postions);
	void renderFarSameLine(mat4 pos1, mat4 pos2, mat4 farnum,int nearfar,vec3 campos);
	void rendtest();
	vec3 mat4tovec3(glm::mat4 xf, glm::vec3 v) {
		float* pSource = glm::value_ptr(xf);
		float h = pSource[3] * v.x + pSource[7] * v.y + pSource[11] * v.z + pSource[15];
		h = 1 / h;

		return glm::vec3(float((pSource[0] * v.x + pSource[4] * v.y + pSource[8] * v.z + pSource[12])),
			float((pSource[1] * v.x + pSource[5] * v.y + pSource[9] * v.z + pSource[13])),
			float((pSource[2] * v.x + pSource[6] * v.y + pSource[10] * v.z + pSource[14])));
	}
};