#include <Common.h>
#include <ViewManager.h>
#include <AntTweakBar/AntTweakBar.h>
#include <ResourcePath.h>
#include <cmath>
#include "CameraControl.h"
#include "OpenMesh.h"
#include "MeshObject.h"
#include "DrawModelShader.h"
#include "PickingShader.h"
#include "PickingTexture.h"
#include "DrawPickingFaceShader.h"
#include "DrawTextureShader.h"
#include "DrawModelLineShader.h"
#include "BlendingShader.h"
#include "Bezier_ProceduralShader.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../Include/STB/stb_image_write.h"
#include "../Include/Common.h"
#include "ParametricNode.h"
#include "RuleMath.h"
#include "RelationTree.h"
#include "GlmMath.h"
#include "TreeNode.h"
#include<algorithm>
#include <queue>
using namespace glm;
using namespace std;

int windowWidth = 1200;
int windowHeight = 600;
int modelwidth = 50;
int modelheight = 50;
int modeldepth = 50;
const std::string ProjectName = "TextureParameterization";

GLuint			program;			//shader program
mat4			proj_matrix;		//projection matrix
float			aspect;

GLuint			textureID, linetexture, modeltexture, imgtext;
GLuint  model_vao, model_buffer, depthBuffer, model_depth;
GLuint  line_vao, line_buffer, line_depth;
GLuint ground_vao;
GLuint VBO, VAO, EBO;
//unsigned int modeltexture, linetexture
CameraControl		meshWindowCam;
int windowNum, floorsNum, LodT;
ofstream modellinefile, modelposfile, gestaltrulefile;//存取線群與模型的位置圖給unity

vector<vec4> colors = { vec4(0,1,1,1) , vec4(0,1,0,1),vec4(0,0,1,1),
				vec4(1,0,0,1), vec4(1,1,0,1),vec4(1,0,1,1),vec4(1,1 ,1 ,1) };//vec4(0.299,0.587 ,0.114 ,1)


float prevUVRotateAngle = 0.0;
MeshObject model;
vector<MeshObject> models;
DrawTextureShader drawTextureShader;
DrawModelShader drawModelShader;
DrawModelLineShader drawModelLineShader;
DrawPickingFaceShader drawPickingFaceShader;
Bezier_ProceduralShader bezierProceduralShader;
PickingShader pickingShader;
PickingTexture pickingTexture;
BlendingShader blendingShader;
ParametricNode parametricNodeTest, parametricNode;
ParametricNode roofNodeTest, roofSTest;
RelationTree Ting, text;
int mainWindow, meshWindow, texCoordWindow;
TwBar* bar;
GLuint	depthModelFBO, depthModelRBO, depthModelTexture;
GLuint	 colorLineFBO, colorLineRBO, colorLineTexture, fboHandle, rboId;
GLuint vao_shader, vbo_shader;
int sfactor = 1;
int dfactor = 1;
int hierarchdist = 40;
GLuint			FBO;
GLuint			depthRBO;
GLuint			FBODataTexture;
int density = 5;
vector<vec3> buildridgepos0;
bool writemodelline = true, writemodel = true;
RelationTree Build;
vector<TreeNode> similarityNodes;
vector<TreeNode> proximityNodes;
vector<TreeNode> common_orientationNode;
TreeNode houseTree;//tree, gestaltTree
vector<vector<TreeNode>> gestaltTree;
vector<TreeNode> tnodes;//nodes group
vector<TreeNode> nodes;//all node
vector<int> modelrender;
vector<int> linerender;
vector<bool> linerenderb;
int changeshow = 0;
float allvolume;
void TW_CALL ParameterizationBtn(void* clientData)
{

}

void setupGUI()
{
#ifdef _MSC_VER
	TwInit(TW_OPENGL, NULL);
#else
	TwInit(TW_OPENGL_CORE, NULL);
#endif
	TwGLUTModifiersFunc(glutGetModifiers);
	bar = TwNewBar("Setting");
	TwDefine(" 'Texture Parameter Setting' size='220 90' ");
	TwDefine(" 'Texture Parameter Setting' fontsize='3' color='96 216 224'");
	// Create an internal enum to name the meshes
	typedef enum { horizontal, vertical, notshow } MESHLineTYPE;

	// A variable for the current selection - will be updated by ATB
	MESHLineTYPE m_currentMesh = horizontal;

	// Array of drop down items
	TwEnumVal Meshes[] = { { horizontal, "horizontal"}, {vertical, "vertical"}, {notshow, "notshow"} };

	// ATB identifier for the array
	TwType meshLineTwType = TwDefineEnum("MeshType", Meshes, 3);
	
	// Adding variables to the tweak bar
	TwAddVarRW(bar, "Window Number", TW_TYPE_INT32, &windowNum, " label='Number of Windows' min=0 max=200 step=1 ");
	TwAddVarRW(bar, "Floor Number", TW_TYPE_INT32, &floorsNum, " label='Number of Floors' min=0 max=100 step=1 ");
	TwAddVarRW(bar, "LodT Number", TW_TYPE_INT32, &LodT, " label='Number of LodT' min=0 max=100 step=1 ");

}

void My_LoadModel()
{
	if (model.Init(ResourcePath::modelPath))
	{
		int id = 0;
		while (model.AddSelectedFace(id))
		{
			++id;
		}

		puts("Load Model");
	}
	else
	{
		puts("Load Model Failed");
	}
}

void My_LoadModels() {
	//models
	for (int i = 0; i < ResourcePath::modelPath1.size(); i++) {
		MeshObject m;
		if (m.Init(ResourcePath::modelPath1[i]))
		{
			int id = 0;
			while (m.AddSelectedFace(id))
			{
				++id;
			}

			puts("Load Model");
		}
		else
		{
			puts("Load Model Failed");
		}
		models.push_back(m);
	}
}

void InitOpenGL()
{
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void  ProceduralTestBuild() {
	Way pway;
	pway.x = 2;
	pway.startpos = vec3(5, 0, 0);
	pway.creatnum = 50;
	parametricNode.generateWay = pway;
	parametricNode.createElements();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();



	Way way;
	way.x = 2;
	way.startpos = vec3(-50, 0, 0);
	way.creatnum = 100;
	parametricNodeTest.generateWay = way;
	parametricNodeTest.createElements();
	glm::mat4 Matm = modelMat;
	Matm = glm::scale(Matm, glm::vec3(2, 2, 2));
	glm::rotate(Matm, glm::radians((float)(-90)), glm::vec3(1.0f, 0.0f, 0.0f));


	parametricNodeTest.differentElements.push_back(model);
	parametricNodeTest.differentElements[0].ChangePoint(Matm);
	//parametricNodeTest.lineSegment();
	model.meshEdgeGroup = parametricNodeTest.differentElements[0].meshEdgeGroup;
	parametricNodeTest.sameObjectBuild(Matm);
	parametricNode.ParametricRule.push_back(parametricNodeTest);
	if (parametricNodeTest.differentElements.size() == 1)
		parametricNodeTest.similarity = true;


	Way sway;
	sway.x = 2;
	sway.startpos = vec3(-50, 0, 2);
	sway.creatnum = 100;
	ParametricNode parametricNodeTest1; parametricNodeTest1.generateWay = sway;
	parametricNodeTest1.createElements();
	Matm = modelMat;
	Matm = glm::scale(Matm, glm::vec3(2, 2, 2));
	glm::rotate(Matm, glm::radians((float)(-90)), glm::vec3(1.0f, 0.0f, 0.0f));
	parametricNodeTest1.differentElements.push_back(model);
	parametricNodeTest1.differentElements[0].ChangePoint(Matm);
	//parametricNodeTest.lineSegment();
	model.meshEdgeGroup = parametricNodeTest1.differentElements[0].meshEdgeGroup;
	parametricNodeTest1.sameObjectBuild(Matm);
	parametricNode.ParametricRule.push_back(parametricNodeTest1);
	if (parametricNodeTest1.differentElements.size() == 1)
		parametricNodeTest1.similarity = true;



	vec3 step = vec3(0, 0, 0);

	step = parametricNodeTest1.elementsPos[1] - parametricNodeTest1.elementsPos[0];
	MyMesh::Point ss = MyMesh::Point(step[0], step[1], step[2]);
	float disPos = dist(parametricNodeTest1.elementsPos[1], parametricNodeTest1.elementsPos[0]);
	model.lineSegment(ss, disPos);

	RelationTreeNode sceP;
	sceP.parametricNode.push_back(parametricNode);

	text.root.child.push_back(sceP);

}

void RoofTestBuild() {
	//vec3 baoding = vec3(0, 60, 0);

	//底座，預設高度為1/6
		//Platform-Ting.width+	Platform.bottom_eave_out  depth-Ting.depth +Platform.bottom_eave_out
		///--h-----h1---f--
		///|-g---h0----ee-|
		///|| ----x---   ||
		///-|d----h1----b|-
		///--c---h0-----a--
	float platformwidth = modelwidth + (modelwidth / 10);
	float platformdepth = modeldepth + (modeldepth / 10);
	float platformheight = modelheight / 6;
	RelationTreeNode platform;
	platform.samplePointPos.push_back(vec3(0, (modelheight / 6 + 0) / 2, 0));
	platform.linebulid(vec3(0, (modelheight / 6 + 0) / 2, 0), platformwidth, platformheight, platformdepth);
	platform.hierarchy = 5;
	//platform.color = colorTable[0];
	platform.objType = 2;
	platform.boundingboxinformation();
	platform.name = "platform";
	platform.geomtrycontrol[0] = true; platform.geomtrycontrol[2];
	Ting.root.child.push_back(platform);


	//柱子，預設高度為1/2
	RelationTreeNode column;
	float columnheight = modelheight / 2;
	RelationTreeNode column0;
	column0.samplePointPos.push_back(vec3(modelwidth / 2, platformheight, modelwidth / 2));
	column0.samplePointPos.push_back(vec3(modelwidth / 2, platformheight + columnheight, modelwidth / 2));
	column0.linebulid(column0.samplePointPos);
	//column0.color = colorTable[1];
	column0.hierarchy = 5;
	platform.hierarchy += 0.1;
	column0.objType = 0;
	column0.name = "column0";
	column0.geomtrycontrol[1] = true;
	column.child.push_back(column0);

	RelationTreeNode column1;
	column1.samplePointPos.push_back(vec3(-modelwidth / 2, platformheight, modelwidth / 2));
	column1.samplePointPos.push_back(vec3(-modelwidth / 2, platformheight + columnheight, modelwidth / 2));
	column1.linebulid(column1.samplePointPos);
	//column1.color = colorTable[2]; column1.hierarchy = 5;
	column1.objType = 0;
	column1.name = "column1";
	platform.hierarchy += 0.1;
	column1.geomtrycontrol[1] = true;
	column.child.push_back(column1);

	RelationTreeNode column2;
	column2.samplePointPos.push_back(vec3(modelwidth / 2, platformheight, -modelwidth / 2));
	column2.samplePointPos.push_back(vec3(modelwidth / 2, platformheight + columnheight, -modelwidth / 2));
	column2.linebulid(column2.samplePointPos);
	//column2.color = colorTable[3];
	platform.hierarchy += 0.1;
	column2.hierarchy = 5;
	column2.objType = 0;
	column2.name = "column2";
	column2.geomtrycontrol[1] = true;
	column.child.push_back(column2);

	RelationTreeNode column3;
	column3.samplePointPos.push_back(vec3(-modelwidth / 2, platformheight, -modelwidth / 2));
	column3.samplePointPos.push_back(vec3(-modelwidth / 2, platformheight + columnheight, -modelwidth / 2));
	column3.linebulid(column3.samplePointPos);
	//column3.color = colorTable[4];
	platform.hierarchy += 0.1;
	column3.hierarchy = 5;
	column3.objType = 0;
	column3.name = "column3";
	column3.geomtrycontrol[1] = true;
	column.child.push_back(column3);
	column.hierarchy = 5;
	column.boundingboxinformation();
	column.name = "column";
	Ting.root.child.push_back(column);

	//寶頂，預設建築高度為+α(10)
	RelationTreeNode baoding;
	float baodingheight = modelheight + 10;
	//baoding.color = colorTable[5];
	baoding.samplePointPos.push_back(vec3(0, baodingheight, 0));
	baoding.objType = 1;
	platform.linebulid(vec3(0, baodingheight, 0), modelwidth / 20, modelheight / 20, modeldepth / 20);
	baoding.hierarchy = 5;
	//CreateLineModel.subObj.push_back(baoding);
	baoding.boundingboxinformation();
	baoding.name = "baoding";
	Ting.root.child.push_back(baoding);

	//柱子線資訊
	vec3 column00 = column0.samplePointPos[0];
	vec3 column01 = column0.samplePointPos[1];
	vec3 column10 = column1.samplePointPos[0];
	vec3 column11 = column1.samplePointPos[1];
	vec3 column20 = column2.samplePointPos[0];
	vec3 column21 = column2.samplePointPos[1];
	vec3 column30 = column3.samplePointPos[0];
	vec3 column31 = column3.samplePointPos[1];

	//樑模型。相鄰兩個柱頂的控制點 pcpColumn_Top,i+1及 pcpColumn_Top, i + 2 所形成的線段
	RelationTreeNode beam, girder;
	RelationTreeNode beam0;
	beam0.samplePointPos.push_back(column01);
	beam0.samplePointPos.push_back(column11);
	beam0.linebulid(beam0.samplePointPos);
	//beam0.color = colorTable[6];
	beam0.objType = 3;
	beam0.name = "beam0";
	beam0.geomtrycontrol[0] = true; beam0.geomtrycontrol[2] = true;
	beam.child.push_back(beam0);

	RelationTreeNode beam1;
	beam1.samplePointPos.push_back(column11);
	beam1.samplePointPos.push_back(column31);
	beam1.linebulid(beam1.samplePointPos);
	//beam1.color = colorTable[7];
	beam1.objType = 3;
	beam1.name = "beam1";
	beam1.geomtrycontrol[0] = true; beam1.geomtrycontrol[2] = true;

	RelationTreeNode beam2;
	beam2.samplePointPos.push_back(column21);
	beam2.samplePointPos.push_back(column31);
	beam2.linebulid(beam2.samplePointPos);
	//beam2.color = colorTable[8];
	beam2.objType = 3;
	beam2.name = "beam2";
	beam2.geomtrycontrol[0] = true; beam2.geomtrycontrol[2] = true;
	beam.child.push_back(beam2);

	RelationTreeNode beam3;
	beam3.samplePointPos.push_back(column01);
	beam3.samplePointPos.push_back(column21);
	beam3.linebulid(beam3.samplePointPos);
	//beam3.color = colorTable[9];
	beam3.objType = 3;
	beam3.name = "beam3";
	beam3.geomtrycontrol[0] = true; beam3.geomtrycontrol[2] = true;
	beam.child.push_back(beam3);
	beam.boundingboxinformation();
	beam.axis.push_back(vec3(0, 3, 0));
	beam.name = "beam";
	girder.child.push_back(beam);

	//檁模型。相鄰兩個柱頂的控制點 pcpColumn_Top,i+1及 pcpColumn_Top, i + 2 所形成的線段上,並且往上方平移 Beam.height(5)×0.8。
	RelationTreeNode pulin;
	float beamheight = 3;
	RelationTreeNode pulin0;
	pulin0.samplePointPos.push_back(column01 + vec3(0, beamheight, 0));
	pulin0.samplePointPos.push_back(column11 + vec3(0, beamheight, 0));
	pulin0.linebulid(pulin0.samplePointPos);
	//pulin0.color = colorTable[10];
	pulin0.objType = 3;
	pulin0.name = "pulin0";
	pulin0.geomtrycontrol[0] = true; pulin0.geomtrycontrol[2] = true;
	pulin.child.push_back(pulin0);

	RelationTreeNode pulin1;
	pulin1.samplePointPos.push_back(column11 + vec3(0, beamheight, 0));
	pulin1.samplePointPos.push_back(column31 + vec3(0, beamheight, 0));
	pulin1.linebulid(pulin1.samplePointPos);
	//pulin1.color = colorTable[11];
	//pulin1.axis.push_back(point(2, 1, 2));
	pulin1.objType = 3;
	pulin1.name = "pulin1";
	pulin1.geomtrycontrol[0] = true; pulin1.geomtrycontrol[2] = true;
	pulin.child.push_back(pulin1);

	RelationTreeNode pulin2;
	pulin2.samplePointPos.push_back(column21 + vec3(0, beamheight, 0));
	pulin2.samplePointPos.push_back(column31 + vec3(0, beamheight, 0));
	pulin2.linebulid(pulin2.samplePointPos);
	//pulin2.color = colorTable[12];
	pulin2.objType = 3;
	pulin2.name = "pulin2";
	pulin2.geomtrycontrol[0] = true; pulin2.geomtrycontrol[2] = true;
	pulin.child.push_back(pulin2);

	RelationTreeNode pulin3;
	pulin3.samplePointPos.push_back(column01 + vec3(0, beamheight, 0));
	pulin3.samplePointPos.push_back(column21 + vec3(0, beamheight, 0));
	pulin3.linebulid(pulin3.samplePointPos);
	//pulin3.color = colorTable[13];
	pulin3.objType = 3;
	pulin3.name = "pulin3";
	pulin3.geomtrycontrol[0] = true; pulin3.geomtrycontrol[2] = true;
	pulin.child.push_back(pulin3);
	pulin.boundingboxinformation();
	pulin.name = "pulin";
	girder.child.push_back(pulin);
	girder.gestalt = 2;
	girder.boundingboxinformation();
	girder.axis.push_back(vec3(0, beamheight, 0));
	girder.name = "girder";
	Ting.root.child.push_back(girder);


	float bodyheigh = columnheight + beamheight;
	float Roof_top_eave_out = 10;
	float roofwidth = modelwidth + Roof_top_eave_out;
	float roofdepth = modeldepth + Roof_top_eave_out;

	//pcpRidge 控制屋頂的寬和深，它的位置是以下列式子計算：pcpRidge = (Roof.width,Platform.height + Body.height, Roof.depth)。

	vec3 pcpRidge0 = vec3(roofwidth / 2, platformheight + bodyheigh, roofwidth / 2);

	vec3 pcpRidge1 = vec3(roofwidth / 2, platformheight + bodyheigh, -roofwidth / 2);

	vec3 pcpRidge2 = vec3(-roofwidth / 2, platformheight + bodyheigh, roofwidth / 2);//0

	vec3 pcpRidge3 = vec3(-roofwidth / 2, platformheight + bodyheigh, -roofwidth / 2);//1

	//pcpRidge_Tail 表示了脊尾的位置，並以下列式子計算：bcp Ridge pcp Ridge 的延伸線上，長度則設為1/2 pcp* Ridge pcp Ridge _ Tail

	vec3 pcpRidge_Tail0 = pcpRidge0 + vec3(Roof_top_eave_out / 2, 5, Roof_top_eave_out / 2);

	vec3 pcpRidge_Tail1 = pcpRidge1 + vec3(Roof_top_eave_out / 2, 5, -Roof_top_eave_out / 2);

	vec3 pcpRidge_Tail2 = pcpRidge2 + vec3(-Roof_top_eave_out / 2, 5, Roof_top_eave_out / 2);//0

	vec3 pcpRidge_Tail3 = pcpRidge3 + vec3(-Roof_top_eave_out / 2, 5, -Roof_top_eave_out / 2);//1
	//pcpEave 是用來表示屋簷的直線部份和曲線部份的交點的位置控置點。此控制點的預設位置為屋簷中點pEave_Center 而 高 度 pEave_Center_hight則 設 為 pcpEave.y =pcpRideg.y – 0.3×Roof.top_eave_out。此控制點同時也控制了屋簷向內 / 向外以及屋簷的高度。
	float pEave_Center_hight = modelheight - 0.3 * Roof_top_eave_out;
	vec3 pEave_Center0 = vec3((pcpRidge0[0] + pcpRidge1[0]) / 2, pEave_Center_hight, (pcpRidge0[2] + pcpRidge1[2]) / 2);
	vec3 pEave_Center1 = vec3((pcpRidge2[0] + pcpRidge0[0]) / 2, pEave_Center_hight, (pcpRidge2[2] + pcpRidge0[2]) / 2);
	vec3 pEave_Center2 = vec3((pcpRidge2[0] + pcpRidge3[0]) / 2, pEave_Center_hight, (pcpRidge2[2] + pcpRidge3[2]) / 2);
	vec3 pEave_Center3 = vec3((pcpRidge1[0] + pcpRidge3[0]) / 2, pEave_Center_hight, (pcpRidge1[2] + pcpRidge3[2]) / 2);

	//使用二次貝茲曲線建屋脊
	//用寶頂的位置來建屋脊
	int thepointnum = (modelwidth / 2 + Roof_top_eave_out) / modelwidth;
	std::vector<vec3> buildridgepos[4];

	buildridgepos[0] = CalculationCubicBeizerPoint(baoding.samplePointPos[0], pcpRidge0, pcpRidge_Tail0, thepointnum, 5);
	buildridgepos[1] = CalculationCubicBeizerPoint(baoding.samplePointPos[0], pcpRidge1, pcpRidge_Tail1, thepointnum, 5);
	buildridgepos[2] = CalculationCubicBeizerPoint(baoding.samplePointPos[0], pcpRidge2, pcpRidge_Tail2, thepointnum, 5);
	buildridgepos[3] = CalculationCubicBeizerPoint(baoding.samplePointPos[0], pcpRidge3, pcpRidge_Tail3, thepointnum, 5);

	//建立屋脊
	RelationTreeNode ridge;
	RelationTreeNode ridges[4];
	int colorcontrol = 14;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < thepointnum / 3 * 2; j++) {
			//RelationTreeNode ridgeobj;
			if (j == 0)
			{
				ridges[i].samplePointPos.push_back(baoding.samplePointPos[0]);
				vec3 cont;
				if (i == 0)  cont = (pcpRidge0 - pcpRidge1) / fabs(pcpRidge0 - pcpRidge1); else if (i == 1)  cont = (pcpRidge1 - pcpRidge2) / fabs(pcpRidge1 - pcpRidge2); else if (i == 2)  cont = (pcpRidge2 - pcpRidge3) / fabs(pcpRidge2 - pcpRidge3); else if (i == 3)  cont = (pcpRidge3 - pcpRidge0) / fabs(pcpRidge3 - pcpRidge0);
				ridges[i].samplePointNomal.push_back(get_normal(baoding.samplePointPos[0], buildridgepos[i][j], buildridgepos[i][thepointnum / 3 * 2]));
				ridges[i].gestalt += 0.01f;
			}

			else if (j > 0 && j < thepointnum / 3 * 2) {
				vec3 cont;
				if (i == 0)  cont = (pcpRidge0 - pcpRidge1) / fabs(pcpRidge0 - pcpRidge1); else if (i == 1)  cont = (pcpRidge1 - pcpRidge2) / fabs(pcpRidge1 - pcpRidge2); else if (i == 2)  cont = (pcpRidge2 - pcpRidge3) / fabs(pcpRidge2 - pcpRidge3); else if (i == 3)  cont = (pcpRidge3 - pcpRidge0) / fabs(pcpRidge3 - pcpRidge0);
				vec3 objnormal = get_normal(buildridgepos[i][thepointnum / 3 * 2], buildridgepos[i][j], buildridgepos[i][j - 1]);
				ridges[i].samplePointNomal.push_back(objnormal);
			}

			ridges[i].samplePointPos.push_back(buildridgepos[i][j]);
			ridges[i].gestalt += 0.01f;
			float v = ((float)i / 8);
			v = (cos(v * 3.1415926 * 90) + 1) / 2;
			vec3 starttoend = vec3(buildridgepos[i][j][0] - buildridgepos[i][thepointnum / 3 * 2][0], buildridgepos[i][j][1] - buildridgepos[i][thepointnum / 3 * 2][1], buildridgepos[i][thepointnum / 3 * 2][2] - buildridgepos[i][j][2]);
			vec3 po;
			if (i == 0)
				po = (vec3(235, 50, 50));
			else if (i == 1)
				po = (vec3(55, -50, -50));
			else if (i == 2)
				po = (vec3(235, 50, -50));
			else
				po = (vec3(55, -50, 50));
			vec3 ridgeLeft = po;
			ridges[i].angle[0] = ridgeLeft[0]; ridges[i].angle[1] = ridgeLeft[1]; ridges[i].angle[2] = ridgeLeft[2];

		}

		//ridges[i].linebulid(ridges[i].samplePointPos);
		//ridges[i].color = colorTable[colorcontrol];
		colorcontrol++;

		ridges[i].objTypes[0].push_back(0);
		ridges[i].objTypes[1].push_back(4);

		ridges[i].objTypes[0].push_back(thepointnum / 3 * 2 - 3);
		ridges[i].objTypes[1].push_back(5);

		ridges[i].objTypes[0].push_back(thepointnum / 3 * 2 - 2);
		ridges[i].objTypes[1].push_back(6);



		ridges[i].name = "ridges" + to_string(i);
		ridges[i].geomtrycontrol[0] = true; ridges[i].geomtrycontrol[1] = true;
		ridge.child.push_back(ridges[i]);

		if (colorcontrol >= 15)colorcontrol = 0;
	}

	//使用二次貝茲建屋簷

	buildridgepos0 = buildridgepos[0];
}

void setupBuffer() {
	// Generate & Bind the framebuffer
	glGenFramebuffers(1, &depthModelFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthModelFBO);
	// Generate the depth buffer texture


	auto error = glGetError();
	GLenum drawBuffers[1] = { GL_DEPTH_COMPONENT };
	glDrawBuffers(1, drawBuffers);
	glGenTextures(1, &depthModelTexture);
	glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	error = glGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	error = glGetError();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthModelTexture, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	error = glGetError();

	glGenFramebuffers(1, &colorLineFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, colorLineFBO);

	error = glGetError();
	glGenTextures(1, &colorLineTexture);
	glBindTexture(GL_TEXTURE_2D, colorLineTexture);
	error = glGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	error = glGetError();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorLineTexture, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	error = glGetError();
	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			-1.0f,1.0f,0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f,   // 右上
			 1.0f,1.0f,0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			 1.0f,-1.0f,0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 左下
			-1.0f,1.0f,0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,    // 左上
			 1.0f,-1.0f,0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			-1.0f,-1.0f,0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f   // 右下
	};

	unsigned int indices[] = {
		0, 1, 2, // first triangle
		3, 4, 5  // second triangle
	};

	//unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/*error = glGetError();

	glGenVertexArrays(1, &vao_shader);
	glBindVertexArray(vao_shader);
	error = glGetError();
	glGenBuffers(1, &vbo_shader);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_shader);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * buildridgepos0.size(), &buildridgepos0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	error = glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	error = glGetError();*/
}
// Compute the weight of an edge based on proximity
float compute_proximity_weight(const std::vector<vec3>& vertices, int start, int end, float max_distance) {
	float dist = distance(vertices[start], vertices[end]);
	if (dist <= max_distance) {
		return 1.0f - dist / max_distance;
	}
	return 0.0f;
}
float compute_proximity_weight(const vec3 vertice1, vec3 vertice2, float max_distance) {
	float dist = distance(vertice1, vertice2);
	if (dist <= max_distance) {
		return 1.0f - dist / max_distance;
	}
	return 0.0f;
}
float compute_proximity_weight(const vector<vec3> vertice1, vector<vec3> vertice2, float max_distance) {
	float dist = distance(vertice1[0], vertice2[0]);
	for (int i = 0; i < vertice1.size(); i++) {
		for (int j = 0; j < vertice2.size(); j++) {
			float step = distance(vertice1[i], vertice2[j]);
			if (step < dist)
				dist = step;
		}
	}
	if (dist == 0)
		dist = 0.001;

	if (dist <= max_distance) {
		return 1.0f - dist / max_distance;
	}
	else
		return 0.0f;
}
float compute_similarity_weight(float boundingbox1, float boundingbox2) {
	float similarityw = boundingbox1 / boundingbox2;
	if (isfinite(static_cast<double>(similarityw)) == false)
		similarityw = 0;
	//cout << "similarity" << similarityw << endl;
	return similarityw;
}
float angle_between_vectors(const vec3& v1, const vec3& v2) {
	float dotw = dot(v1, v2);
	float cos_angle = dotw / (length(v1) * length(v2));
	return std::acos(std::min(std::max(cos_angle, -1.f), 1.f));
}
//計算面之間的向量
float compute_common_orientation_weight(vec3 obj1, vec3 obj2) {
	/*if (obj1 == obj2)
		return 1;
	else
		return 0;*/
	float angle = angle_between_vectors(obj1, obj2);
	//cout << "angle" << angle << endl;
	if (angle < M_PI / 2)
		return cos(angle);
	else
		return 0;
};
void RoofSurfaceBuild() {
	ParametricNode roofa, roofb, roofeave;
	roofa.bezierWay.start = vec3(0, 60, 0);
	roofa.bezierWay.mid = vec3(50, 20, 50);
	roofa.bezierWay.end = vec3(60, 10, 60);
	roofb.bezierWay.start = vec3(0, 60, 0);
	roofb.bezierWay.mid = vec3(-50, 20, 50);
	roofb.bezierWay.end = vec3(-60, 10, 60);
	roofeave.bezierWay.start = vec3(60, 10, 60);
	roofeave.bezierWay.mid = vec3(0, 20, 0);
	roofeave.bezierWay.end = vec3(-60, 10, 60);
	roofSTest.ParametricRule.push_back(roofa);
	roofSTest.ParametricRule.push_back(roofb);
	roofSTest.ParametricRule.push_back(roofeave);

}

void RoofSurfaceBuildP() {
	ParametricNode roofa, roofb, roofeave;
	Way pway;
	pway.x = 2.5;
	pway.startpos = vec3(5, 0, 0);
	pway.creatnum = 10;
	roofa.generateWay = pway;
	roofSTest.ParametricRule.push_back(roofa);
	Way way;
	way.z = 2.5;
	way.startpos = vec3(5, 0, 0);
	way.creatnum = 10;
	roofa.generateWay = way;
	roofb.generateWay = way;

	roofSTest.ParametricRule.push_back(roofb);
	MyMesh::Point ss = MyMesh::Point(0 - way.startpos[0], 0 - way.startpos[1], 2 - way.startpos[2]);
	float disPos = dist(vec3(5, 0, 2.5), vec3(5, 0, 0));
	model.lineSegment(ss, disPos);

}

void RenderGroudVAO() {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	float uvRotateAngle = 0.0;
	float prevUVRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 190.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));


	drawModelShader.Enable();
	drawModelShader.SetWireColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	drawModelShader.UseLighting(false);
	drawModelShader.DrawTexCoord(false);
	drawModelShader.DrawWireframe(false);
	drawModelShader.SetPMat(pMat);
	drawModelShader.SetUVRotMat(uvRotMat);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glm::mat4 modelMatm = modelMat;
	modelMatm = modelMat;
	drawModelShader.SetFaceColor(colors[0]);
	modelMatm = glm::scale(modelMatm, vec3(1.0f,1.0f,1.0f));
	modelMatm = glm::translate(modelMatm, glm::vec3(0.0f,0.0f,0.0f));
	
	mvMat = vMat * modelMatm;
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(mvMat)));
	drawModelShader.SetNormalMat(normalMat);
	drawModelShader.SetMVMat(mvMat);

	

	// Define the ground plane vertices
	std::vector<glm::vec3> groundVertices = {
		{500.0f, -0.35f, 500.0f},
		{-500.0f, -0.35f, 500.0f},
		{-500.0f, -0.35f, -500.0f},
		{500.0f, -0.35f, -500.0f}
	};

	// Define the ground plane indices
	std::vector<unsigned int> groundIndices = {
		0, 1, 2,
		0, 2, 3
	};
	// Create a new vertex buffer object and copy the ground plane vertices to it

	glBindVertexArray(ground_vao);
	GLuint vboGround;
	glGenBuffers(1, &vboGround);
	glBindBuffer(GL_ARRAY_BUFFER, vboGround);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * groundVertices.size(), &groundVertices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers for the ground plane vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Create a new index buffer object and copy the ground plane indices to it
	GLuint iboGround;
	glGenBuffers(1, &iboGround);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboGround);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * groundIndices.size(), &groundIndices[0], GL_STATIC_DRAW);
	glColor4ub(0, 255, 0, 255);
	// Draw the ground plane
	glDrawElements(GL_TRIANGLES, groundIndices.size(), GL_UNSIGNED_INT, 0);

	// Bind the vertex array object again to render the ground
	glBindVertexArray(ground_vao);

	// Render the ground
	glDrawArrays(GL_TRIANGLE_FAN, 0, groundVertices.size());

	// Unbind the vertex array object
	// Cleanup
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glBindTexture(GL_TEXTURE_2D, 0);
	drawModelShader.Disable();
}

void BuildHouseWindow() {
	vector<vector<float>> stepfloat;
	int buildid = 0;

	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	int num_parts = 10;
	int num_divisions = 30;
	vec3 startpos = vec3(0, 0, 0);
	std::vector<vec3> wall_normals = {
		{0, 1, 0},
		{0, 3, 0},
		{0, 2, 0},
		{0, 0, 0}
	};
	glm::mat4 modelMatm = modelMat;
	modelMatm = modelMat;
	modelMatm = glm::scale(modelMatm, models[0].scareObj);
	modelMatm = glm::translate(modelMatm, glm::vec3(startpos));
	mvMat = vMat * modelMatm;
	BoundingBox bx;
	bx.min = models[0].mat4tovec3(mvMat, models[0].objBounding.minbounding);
	bx.max = models[0].mat4tovec3(mvMat, models[0].objBounding.maxbounding);
	float minz = models[0].objBounding.minz; float maxz = models[0].objBounding.manz;
	float minx = models[0].objBounding.minx; float maxx = models[0].objBounding.manx;
	float miny = models[0].objBounding.miny; float maxy = models[0].objBounding.many;
	houseTree.childNodes.resize(num_parts);

	similarityNodes.resize(models.size());
	common_orientationNode.resize(5);
	proximityNodes.resize(1);
	for (int i = 0; i < num_parts; i++) {
		ParametricNode floor;
		floor.meshnum = 0;
		floor.elementsPos.push_back(vec3(0, (maxy - miny) * i, 0));
		floor.RegularityT = 0.1;

		houseTree.childNodes[i].parmetricNode.name = ("floor");
		houseTree.childNodes[i].parmetricNode = floor;
		houseTree.childNodes[i].childNodes.resize(4);
		houseTree.childNodes[i].normal = vec3(0, 0, 0);
		houseTree.childNodes[i].treeNodeID = buildid;
		nodes.push_back(houseTree.childNodes[i]);
		similarityNodes[0].childNodes.push_back(houseTree.childNodes[i]);
		similarityNodes[0].volume += models[0].objBounding.volume;
		common_orientationNode[0].childNodes.push_back(houseTree.childNodes[i]);
		proximityNodes[0].childNodes.push_back(houseTree.childNodes[i]);
		allvolume += models[0].objBounding.volume;
		proximityNodes[0].volume += models[0].objBounding.volume;
		common_orientationNode[0].volume += models[0].objBounding.volume;
		buildid++;

		ParametricNode p;
		string name = "window";
		int division = 0;

		vec3 buildmin = vec3(99, 99, 99); vec3 buildmax = vec3(-99, -99, -99);
		for (int j = 0; j < num_divisions - 1; j++) {
			p.meshnum = 1;
			p.elementsPos.push_back(vec3(minx + (maxx - minx) / num_divisions * (j + 1), (maxy - miny) * i, minz));
			p.generateNormal = wall_normals[0];
			p.RegularityT += 0.2;
			modelMatm = modelMat;
			modelMatm = modelMat;
			modelMatm = glm::scale(modelMatm, models[p.meshnum].scareObj);
			modelMatm = glm::translate(modelMatm, glm::vec3(minx + (maxx - minx) / num_divisions * (j), (maxy - miny) * i, minz));
			if (p.generateNormal != vec3(0, 0, 0))
				modelMatm = glm::rotate(modelMatm, glm::radians((float)(90 * p.generateNormal.y)), glm::vec3(p.generateNormal));
			mvMat = vMat * modelMatm;
			buildmin = min(buildmin, models[p.meshnum].mat4tovec3(mvMat, models[p.meshnum].objBounding.minbounding));
			buildmax = max(buildmax, models[p.meshnum].mat4tovec3(mvMat, models[p.meshnum].objBounding.maxbounding));
			allvolume += models[p.meshnum].objBounding.volume;
			similarityNodes[1].volume += models[p.meshnum].objBounding.volume;
			common_orientationNode[1].volume += models[p.meshnum].objBounding.volume;
		}
		houseTree.childNodes[i].childNodes[division].name = name;
		houseTree.childNodes[i].childNodes[division].parmetricNode = p;
		houseTree.childNodes[i].childNodes[division].parent = &houseTree.childNodes[i];
		houseTree.childNodes[i].childNodes[division].treeNodeID = buildid;
		houseTree.childNodes[i].childNodes[division].boundbox.min = buildmin;
		houseTree.childNodes[i].childNodes[division].boundbox.max = buildmax;
		houseTree.childNodes[i].childNodes[division].inChildNode = true;
		houseTree.childNodes[i].childNodes[division].normal = wall_normals[0];
		nodes.push_back(houseTree.childNodes[i].childNodes[division]);
		similarityNodes[1].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		common_orientationNode[1].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		buildid++;
		division++;

		ParametricNode p1;
		buildmin = vec3(99, 99, 99); buildmax = vec3(-99, -99, -99);
		for (int j = 0; j < num_divisions - 1; j++) {

			p1.meshnum = 1;
			p1.elementsPos.push_back(vec3(minx + (maxx - minx) / num_divisions * (j + 1), (maxy - miny) * i, maxz));
			p1.generateNormal = wall_normals[1];
			p1.RegularityT += 0.2;

			modelMatm = modelMat;
			modelMatm = modelMat;
			modelMatm = glm::scale(modelMatm, models[p1.meshnum].scareObj);
			modelMatm = glm::translate(modelMatm, glm::vec3(minx + (maxx - minx) / num_divisions * (j), (maxy - miny) * i, maxz));
			if (p1.generateNormal != vec3(0, 0, 0))
				modelMatm = glm::rotate(modelMatm, glm::radians((float)(90 * p1.generateNormal.y)), glm::vec3(p1.generateNormal));
			mvMat = vMat * modelMatm;
			buildmin = min(buildmin, models[p1.meshnum].mat4tovec3(mvMat, models[p1.meshnum].objBounding.minbounding));
			buildmax = max(buildmax, models[p1.meshnum].mat4tovec3(mvMat, models[p1.meshnum].objBounding.maxbounding));
			allvolume += models[p.meshnum].objBounding.volume;
			similarityNodes[1].volume += models[p.meshnum].objBounding.volume;
			common_orientationNode[2].volume += models[p.meshnum].objBounding.volume;
		}
		houseTree.childNodes[i].childNodes[division].name = name;
		houseTree.childNodes[i].childNodes[division].parmetricNode = p1;
		houseTree.childNodes[i].childNodes[division].parent = &houseTree.childNodes[i];
		houseTree.childNodes[i].childNodes[division].treeNodeID = buildid;
		houseTree.childNodes[i].childNodes[division].boundbox.min = buildmin;
		houseTree.childNodes[i].childNodes[division].boundbox.max = buildmax;
		houseTree.childNodes[i].childNodes[division].inChildNode = true;
		houseTree.childNodes[i].childNodes[division].normal = wall_normals[1];
		nodes.push_back(houseTree.childNodes[i].childNodes[division]);
		similarityNodes[1].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		common_orientationNode[2].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		buildid++; division++;

		ParametricNode p2;
		buildmin = vec3(99, 99, 99); buildmax = vec3(-99, -99, -99);
		for (int j = 0; j < num_divisions - 1; j++) {

			p2.meshnum = 1;
			p2.elementsPos.push_back(vec3(minx, (maxy - miny) * i, minz + (maxz - minz) / num_divisions * (j + 1)));
			p2.generateNormal = wall_normals[2];
			p2.RegularityT += 0.2;

			modelMatm = modelMat;
			modelMatm = modelMat;
			modelMatm = glm::scale(modelMatm, models[p2.meshnum].scareObj);
			modelMatm = glm::translate(modelMatm, glm::vec3(minx, (maxy - miny) * i, minz + (maxz - minz) / num_divisions * (j)));
			if (p2.generateNormal != vec3(0, 0, 0))
				modelMatm = glm::rotate(modelMatm, glm::radians((float)(90 * p2.generateNormal.y)), glm::vec3(p2.generateNormal));
			mvMat = vMat * modelMatm;
			buildmin = min(buildmin, models[p2.meshnum].mat4tovec3(mvMat, models[p2.meshnum].objBounding.minbounding));
			buildmax = max(buildmax, models[p2.meshnum].mat4tovec3(mvMat, models[p2.meshnum].objBounding.maxbounding));
			allvolume += models[p.meshnum].objBounding.volume;
			similarityNodes[1].volume += models[p.meshnum].objBounding.volume;
			common_orientationNode[3].volume += models[p.meshnum].objBounding.volume;
		}
		houseTree.childNodes[i].childNodes[division].name = name;
		houseTree.childNodes[i].childNodes[division].treeNodeID = buildid;
		houseTree.childNodes[i].childNodes[division].parent = &houseTree.childNodes[i];
		houseTree.childNodes[i].childNodes[division].parmetricNode = p2;
		houseTree.childNodes[i].childNodes[division].boundbox.min = buildmin;
		houseTree.childNodes[i].childNodes[division].boundbox.max = buildmax;
		houseTree.childNodes[i].childNodes[division].inChildNode = true;
		houseTree.childNodes[i].childNodes[division].normal = wall_normals[2];
		nodes.push_back(houseTree.childNodes[i].childNodes[division]);
		similarityNodes[1].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		common_orientationNode[3].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		buildid++; division++;

		ParametricNode p3;
		buildmin = vec3(99, 99, 99); buildmax = vec3(-99, -99, -99);
		for (int j = 0; j < num_divisions - 1; j++) {
			p3.meshnum = 1;
			p3.elementsPos.push_back(vec3(maxx, (maxy - miny) * i, minz + (maxz - minz) / num_divisions * (j + 1)));
			p3.generateNormal = wall_normals[3];
			p3.RegularityT += 0.2;

			modelMatm = modelMat;
			modelMatm = modelMat;
			modelMatm = glm::scale(modelMatm, models[p3.meshnum].scareObj);
			modelMatm = glm::translate(modelMatm, glm::vec3(maxx, (maxy - miny) * i, minz + (maxz - minz) / num_divisions * (j - 1)));
			if (p3.generateNormal != vec3(0, 0, 0))
				modelMatm = glm::rotate(modelMatm, glm::radians((float)(90 * p3.generateNormal.y)), glm::vec3(p3.generateNormal));
			mvMat = vMat * modelMatm;
			buildmin = min(buildmin, models[p3.meshnum].mat4tovec3(mvMat, models[p3.meshnum].objBounding.minbounding));
			buildmax = max(buildmax, models[p3.meshnum].mat4tovec3(mvMat, models[p3.meshnum].objBounding.maxbounding));
			allvolume += models[p3.meshnum].objBounding.volume;
			similarityNodes[1].volume += models[p3.meshnum].objBounding.volume;
			common_orientationNode[4].volume += models[p3.meshnum].objBounding.volume;
		}
		houseTree.childNodes[i].childNodes[division].name = name;
		houseTree.childNodes[i].childNodes[division].treeNodeID = buildid;
		houseTree.childNodes[i].childNodes[division].parent = &houseTree.childNodes[i];
		houseTree.childNodes[i].childNodes[division].parmetricNode = p3;
		houseTree.childNodes[i].childNodes[division].boundbox.min = buildmin;
		houseTree.childNodes[i].childNodes[division].boundbox.max = buildmax;
		houseTree.childNodes[i].childNodes[division].inChildNode = true;
		houseTree.childNodes[i].childNodes[division].normal = wall_normals[3];
		nodes.push_back(houseTree.childNodes[i].childNodes[division]);
		similarityNodes[1].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		common_orientationNode[4].childNodes.push_back(houseTree.childNodes[i].childNodes[division]);
		buildid++; division++;
	}
	stepfloat.resize(nodes.size());
	for (int i = 0; i < nodes.size(); i++)
		stepfloat[i].resize(nodes.size());
	vector<BoundingBox> bbox; bbox.resize(nodes.size());
	//全部建築的bounding box，找到最外圍的min與max的vec3算體積，之後算兩個物件的平滑度成本
	vec3 buildmin = vec3(99, 99, 99); vec3 buildmax = vec3(-99, -99, -99);

	for (int i = 0; i < similarityNodes.size(); i++) {
		vector<vec3> normalstep;
		vector<TreeNode> nodeStep;

		for (int j = 0; j < similarityNodes[i].childNodes.size(); j++) {
			if (normalstep.size() == 0)
			{
				TreeNode snode;
				normalstep.push_back(similarityNodes[i].childNodes[j].normal);
				snode.childNodes.push_back(similarityNodes[i].childNodes[j]);
				snode.volume = models[similarityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
				nodeStep.push_back(snode);

			}
			else {
				bool isinstep = false;
				for (int k = 0; k < normalstep.size(); k++)
				{
					if (normalstep[k] == similarityNodes[i].childNodes[j].normal)
					{
						isinstep = true;
						nodeStep[k].childNodes.push_back(similarityNodes[i].childNodes[j]);
						nodeStep[k].volume += models[similarityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
					}
				}
				if (isinstep == false)
				{
					TreeNode snode;
					normalstep.push_back(similarityNodes[i].childNodes[j].normal);
					snode.childNodes.push_back(similarityNodes[i].childNodes[j]);
					snode.volume = models[similarityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
					nodeStep.push_back(snode);
				}
			}
		}
		if (nodeStep.size() > 1) {
			similarityNodes.erase(similarityNodes.begin() + i);
			for (int k = 0; k < nodeStep.size(); k++) {
				similarityNodes.insert(similarityNodes.begin() + i, nodeStep[k]);
			}
			i += nodeStep.size();
		}
	}
	for (int i = 0; i < similarityNodes.size(); i++) {
		for (int j = 0; j < similarityNodes[i].childNodes.size(); j++) {
			nodes[similarityNodes[i].childNodes[j].treeNodeID].similarity = true;
			nodes[similarityNodes[i].childNodes[j].treeNodeID].similarityG = i;
			nodes[similarityNodes[i].childNodes[j].treeNodeID].similarityJ = j;
		}
	}
	for (int i = 0; i < proximityNodes.size(); i++) {
		vector<vec3> normalstep;
		vector<TreeNode> nodeStep;
		vector<int> hasid;//是否已有id
		for (int j = 0; j < proximityNodes[i].childNodes.size(); j++) {
			if (normalstep.size() == 0)
			{
				TreeNode snode;
				normalstep.push_back(proximityNodes[i].childNodes[j].normal);
				snode.childNodes.push_back(proximityNodes[i].childNodes[j]);
				snode.volume = models[proximityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
				nodeStep.push_back(snode);
				hasid.push_back(proximityNodes[i].childNodes[j].treeNodeID);
			}
			else {
				bool isinit = false;
				for (int k = 0; k < hasid.size(); k++) {
					if (proximityNodes[i].childNodes[j].treeNodeID == hasid[k]) {
						isinit = true; break;
					}
				}
				if (isinit == false) {
					hasid.push_back(proximityNodes[i].childNodes[j].treeNodeID);
					bool isinstep = false;
					for (int k = 0; k < normalstep.size(); k++)
					{
						if (normalstep[k] == proximityNodes[i].childNodes[j].normal)
						{
							isinstep = true;
							nodeStep[k].childNodes.push_back(proximityNodes[i].childNodes[j]);
							nodeStep[k].volume += models[proximityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
						}
					}
					if (isinstep == false)
					{
						TreeNode snode;
						normalstep.push_back(proximityNodes[i].childNodes[j].normal);
						snode.childNodes.push_back(proximityNodes[i].childNodes[j]);
						snode.volume = models[proximityNodes[i].childNodes[j].parmetricNode.meshnum].objBounding.volume;
						nodeStep.push_back(snode);
					}
				}
			}
		}
		if (nodeStep.size() > 1) {
			proximityNodes.erase(proximityNodes.begin() + i);
			for (int k = 0; k < nodeStep.size(); k++) {
				proximityNodes.insert(proximityNodes.begin() + i, nodeStep[k]);
			}
			i += nodeStep.size();
		}
	}
	for (int i = 0; i < proximityNodes.size(); i++) {
		for (int j = 0; j < proximityNodes[i].childNodes.size(); j++) {
			nodes[proximityNodes[i].childNodes[j].treeNodeID].proximity = true;
			nodes[proximityNodes[i].childNodes[j].treeNodeID].proximityG = i;
		}
	}
	linerenderb.resize(nodes.size());
	for (int i = 0; i < linerenderb.size(); i++)
		linerenderb[i] = false;
}

void BuildSceneTree() {
	vector<float> volue, stratifiedvolume;
	for (int i = 0; i < proximityNodes.size(); i++) {
		bool isin = false;
		for (int j = 0; j < volue.size(); j++) {
			if (proximityNodes[i].volume == volue[j])
				isin = true;
			if (isin)
				break;
		}
		if (isin == false)
			volue.push_back(proximityNodes[i].volume);
	}
	for (int i = 0; i < similarityNodes.size(); i++) {
		bool isin = false;
		for (int j = 0; j < volue.size(); j++) {
			if (similarityNodes[i].volume == volue[j])
				isin = true;
			if (isin)
				break;
		}
		if (isin == false)
			volue.push_back(similarityNodes[i].volume);
	}
	for (int i = 0; i < common_orientationNode.size(); i++) {
		bool isin = false;
		for (int j = 0; j < volue.size(); j++) {
			if (common_orientationNode[i].volume == volue[j])
				isin = true;
			if (isin)
				break;
		}
		if (isin == false)
			volue.push_back(common_orientationNode[i].volume);
	}
	sort(volue.begin(),volue.end());
	if (volue.size() == 2)
		stratifiedvolume.push_back((volue[0] + volue[1]) / 2);
	else if (volue.size() == 3)
		stratifiedvolume.push_back(volue[1]);
	else
	{
		int takeid=volue.size() / 4;
		for (int i = 0; i < 4; i++)
			stratifiedvolume.push_back(volue[takeid+i*takeid]);
	}
	gestaltTree.reserve(stratifiedvolume.size());
	for (int i = 0; i < common_orientationNode.size(); i++) {
		for (int j = 0; j < stratifiedvolume.size(); j++) {
			if (common_orientationNode[i].volume <= stratifiedvolume[j])
				gestaltTree[j].push_back(common_orientationNode[i]);
		}
	}
}

void BuildingFootprints(ParametricNode parmetric) {
	vector<vector<float>> stepfloat;
	stepfloat.resize(parmetric.ParametricRule.size());
	for (int i = 0; i < parmetric.ParametricRule.size(); i++)
		stepfloat[i].resize(parmetric.ParametricRule.size());
	/*for (int i = 0; i < parmetric.ParametricRule[0].generateWay.creatnum; i++) {
		for (int j = 0; j < parmetric.ParametricRule[1].generateWay.creatnum; j++) {
			vec3 step = parmetric.ParametricRule[1].generateWay.startpos + vec3(parmetric.ParametricRule[0].generateWay.x * i, 0, parmetric.ParametricRule[1].generateWay.z * j);

		}
	}*/
	for (int i = 0; i < parmetric.ParametricRule.size(); i++) {
		for (int j = 0; j < parmetric.ParametricRule.size(); j++) {
			if (i != j) {
				float proximity_weight = compute_proximity_weight(parmetric.ParametricRule[i].generatePos, parmetric.ParametricRule[j].generatePos, 1.0);
				float similarity_weight = compute_similarity_weight(models[parmetric.ParametricRule[i].meshnum].objBounding.volume, models[parmetric.ParametricRule[j].meshnum].objBounding.volume);
				float co_orientation_weight = compute_common_orientation_weight(parmetric.ParametricRule[i].generateNormal, parmetric.ParametricRule[j].generateNormal);
				float total_weight = similarity_weight * proximity_weight * parmetric.ParametricRule[i].RegularityT * co_orientation_weight;

				stepfloat[i][j] = total_weight;
				stepfloat[j][i] = total_weight;
			}
		}
	}
	//build gestalt rule tree


}

void InitData()
{
	ResourcePath::shaderPath = "./Shader/" + ProjectName + "/";
	ResourcePath::imagePath = "./Imgs/" + ProjectName + "/";
	ResourcePath::modelPath = "./Model/floor1.obj";
	ResourcePath::modelPath1.push_back("./Model/floor1.obj");
	ResourcePath::modelPath1.push_back("./Model/window1.obj");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//Initialize shaders
	///////////////////////////	

	drawModelShader.Init();
	drawModelLineShader.Init();
	blendingShader.Init();
	pickingShader.Init();
	bezierProceduralShader.Init();


	TextureData tdata = Common::Load_png("Model/wood.png");

	//Generate empty texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Do texture setting
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	My_LoadModel();
	model.findContours();


	model.contourGroup();
	model.MeshContourStructure();
	model.contourShader();
	model.ObjectBounding();

	My_LoadModels();
	models[0].scareObj = vec3(0.1, 0.1, 0.1);
	models[1].scareObj = vec3(0.1, 0.1, 0.1);
	for (int i = 0; i < models.size(); i++) {
		models[i].findContours();
		models[i].contourGroup();
		models[i].MeshContourStructure();
		models[i].contourShader();
		models[i].ObjectBounding();
	}

	BuildHouseWindow();
	modellinefile.open("modelline2.txt"); modelposfile.open("modelpos2.txt"); gestaltrulefile.open("gestaltrule.txt");
	if (!modellinefile.is_open()) {
		cout << "Failed to modellinefile open file.\n";
	}
	if (!modelposfile.is_open()) {
		cout << "Failed to modelpos open file.\n";
	}
	if (!gestaltrulefile.is_open()) {
		cout << "Failed to gestaltrulefile open file.\n";
	}

}

void Reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	TwWindowSize(width, height);

	int meshWindowWidth = width;
	int meshWindowHeight = height;
	glutSetWindow(meshWindow);
	glutPositionWindow(0, 0);
	glutReshapeWindow(meshWindowWidth, meshWindowHeight);
	glViewport(0, 0, meshWindowWidth, meshWindowHeight);

	aspect = 0.7f;//meshWindowWidth * 1.0f / meshWindowHeight;
	meshWindowCam.SetWindowSize(meshWindowWidth, meshWindowHeight);

	glDeleteTextures(1, &depthModelTexture);

	glGenTextures(1, &depthModelTexture);
	glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	auto error = glGetError();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	error = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, depthModelFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthModelTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	error = glGetError();
	glDeleteTextures(1, &colorLineTexture);

	glGenTextures(1, &colorLineTexture);
	glBindTexture(GL_TEXTURE_2D, colorLineTexture);
	error = glGetError();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	error = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, colorLineFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorLineTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	error = glGetError();
}
//渲染model
void RenderModel() {
	glutSetWindow(meshWindow);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set up the view port

	glutSwapBuffers();
}
//渲染line
void RenderLine() {
	glutSetWindow(meshWindow);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set up the view port
	glutSwapBuffers();

}


// GLUT callback. Called to draw the scene.
int changeTestLine = 0;
vec3 rotateAngle = vec3(1, 1, 1);
vector<int> ProceduralLODpos(vector<vec3> pos) {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	int theStartFar = 0; bool startFar = false;
	int start = 1, biggest = density, step = 1, s = 0, interval = biggest / 2;
	if (interval < 3) interval = 3;
	int midpoint = parametricNode.ParametricRule[0].elementsPos.size() / 2;
	vector<int> takeobj;
	int d = 20;
	float front = dist(campos, parametricNode.ParametricRule[0].elementsPos[0]);
	float mid = dist(campos, parametricNode.ParametricRule[0].elementsPos[parametricNode.ParametricRule[0].elementsPos.size() / 2]);
	float end = dist(campos, parametricNode.ParametricRule[0].elementsPos[parametricNode.ParametricRule[0].elementsPos.size() - 1]);
	if (end > d * 2 && front > d * 2 && biggest > 3)
		interval *= 3;

	for (int i = 0; i < parametricNode.ParametricRule[0].elementsPos.size(); i += step) {
		float startFar = dist(campos, parametricNode.ParametricRule[0].elementsPos[theStartFar]);
		float thisD = dist(campos, parametricNode.ParametricRule[0].elementsPos[i]);
		float backD;
		if (i + biggest < parametricNode.ParametricRule[0].elementsPos.size()) {
			backD = dist(campos, parametricNode.ParametricRule[0].elementsPos[i + biggest]);
		}
		if (i == 0 && thisD > d && backD > d) {
			step = biggest; s = 0;
		}
		else if (thisD > d * 2 && backD > d * 2 && step > 1 && step < biggest) {
			step = biggest; s = 0;
		}
		else if (thisD > d && backD > thisD && s > interval && step <= biggest) {
			s = 0;
			step++;
		}
		else if (thisD > d && backD < thisD && s > interval && step > 1) {
			s = 0;
			step--;
		}
		else if (thisD > d) {
			s++;
		}
		else if (thisD < d) {
			s = 0;
			step = 1;
		}
		takeobj.push_back(i);
		if (dist(campos, parametricNode.ParametricRule[0].elementsPos[i + step]) < d)
			step = 1;
		if (end > thisD && end > d * 3 && thisD > d * 2 && i > midpoint && step > biggest - 1) {
			takeobj.push_back(parametricNode.ParametricRule[0].elementsPos.size() - 1);
			break;
		}
	}
	return takeobj;
}
vector<int> roofProceduralLODpos(vector<vec3> pos) {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	int theStartFar = 0; bool startFar = false;
	int start = 1, biggest = density, step = 1, s = 0, interval = biggest / 2;
	if (interval < 3) interval = 3;
	int midpoint = pos.size() / 2;
	vector<int> takeobj;
	int d = 25;
	float front = dist(campos, pos[0]);
	float mid = dist(campos, pos[pos.size() / 2]);
	float end = dist(campos, pos[pos.size() - 1]);
	if (end > d * 2 && front > d * 2 && biggest > 3)
		interval *= 3;

	for (int i = 0; i < pos.size(); i += step) {
		float startFar = dist(campos, pos[theStartFar]);
		float thisD = dist(campos, pos[i]);
		float backD = dist(campos, pos[pos.size() - 1]);
		if (i + biggest < pos.size()) {
			backD = dist(campos, pos[i + biggest]);
		}

		if (i == 0 && thisD > d && backD > d) {
			step = biggest; s = 0;
		}
		else if (thisD > d * 2 && backD > d * 2) {
			step = biggest; s = 0;
		}
		else if (thisD > d && backD > thisD && s > interval && step <= biggest) {
			s = 0;
			theStartFar = i;
			step++;
		}
		else if (thisD > d && backD < thisD && s > interval && step > 1) {
			s = 0;
			theStartFar = i;
			step--;
		}
		else if (thisD > d) {
			s++;
		}
		else if (thisD < d) {
			s = 0;
			step = 1;
		}
		takeobj.push_back(i);
		if (dist(campos, pos[i + step]) < d)
			step = 1;
		if (end > thisD && end > d * 3 && thisD > d * 2 && i > midpoint && step > biggest - 1) {
			takeobj.push_back(pos.size() - 1);
			break;
		}
	}
	return takeobj;
}
void roofSurfaceTest() {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	vector<mat4> rendsame;

	auto error = glGetError();
	//幀緩衝區物件
	glBindFramebuffer(GL_FRAMEBUFFER, depthModelFBO);
	float uvRotateAngle = 0.0;
	float prevUVRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 190.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	int d = 30;
	//生成物件
	vector<mat4> objsMat;
	error = glGetError();
	// Enables the Depth Buffer and choses which depth function to use
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	vector<vec3> objspostion; //攝像機照攝范圍與其物件的scale與旋轉，之後會加上位置參數

	error = glGetError();
	for (int i = 0; i < text.root.child.size(); i++)
	{
		for (int k = 0; k < text.root.child[i].parametricNode.size(); k++) {
			for (int l = 0; l < text.root.child[i].parametricNode[k].ParametricRule.size(); l++) {
				for (int j = 0; j < text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos.size(); j++) {
					mat4 modelMatl = modelMat;
					modelMatl = glm::scale(modelMatl, glm::vec3(2, 2, 2));
					modelMatl = glm::rotate(modelMatl, glm::radians((float)(-90)), glm::vec3(1.0f, 0.0f, 0.0f));
					rendsame.push_back(modelMatl);
					mat4 mvMat = modelMatl;
					//原本Procedural的生成位置  
					vec3 objp = mat4tovec3(mvMat, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]);
					modelMatl = glm::translate(modelMatl, glm::vec3(text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]));
					float d1 = dist(campos, objp);
					mvMat = modelMatl;
					//boundingbox
					vec3 bundingmin = mat4tovec3(mvMat, model.objBounding.minbounding);
					float d2 = dist(campos, bundingmin);
					vec3 bundingmax = mat4tovec3(mvMat, model.objBounding.maxbounding);
					float d3 = dist(campos, bundingmax);
					if (d1 < d2 && d1 < d3)
						objspostion.push_back(objp);
					else if (d2 < d1 && d2 < d3)
						objspostion.push_back(bundingmin);
					else
						objspostion.push_back(bundingmax);
					objsMat.push_back(mvMat);
				}

				drawModelShader.Enable();
				drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				drawModelShader.UseLighting(false);
				drawModelShader.DrawTexCoord(false);
				drawModelShader.DrawWireframe(false);
				drawModelShader.SetPMat(pMat);
				drawModelShader.SetUVRotMat(uvRotMat);
				drawModelShader.SetFaceColor(colors[6]);
				vector<int> trytest;
				for (int j = 0; j < text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos.size(); j++) {
					if (dist(campos, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]) < d) {
						mat4 temp = glm::scale(objsMat[j], glm::vec3(0.975, 0.975, 0.999));
						glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(temp)));

						drawModelShader.SetNormalMat(normalMat);

						drawModelShader.SetMVMat(vMat * temp);
						trytest.push_back(i);
						model.Render();
					}
				}
				std::vector<std::uint8_t> data(windowWidth * windowHeight);
				drawModelShader.Disable();

				objsMat.clear();

			}
		}
	}
	error = glGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	error = glGetError();

	glBindFramebuffer(GL_FRAMEBUFFER, colorLineFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	error = glGetError();
	drawModelLineShader.Enable();
	///////////////////////////
	for (int i = 0; i < text.root.child.size(); i++)
	{
		for (int k = 0; k < text.root.child[i].parametricNode.size(); k++) {
			for (int l = 0; l < text.root.child[i].parametricNode[k].ParametricRule.size(); l++) {
				for (int j = 0; j < text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos.size(); j++) {
					mat4 modelMatl = modelMat;
					modelMatl = glm::scale(modelMatl, glm::vec3(2, 2, 2));
					modelMatl = glm::rotate(modelMatl, glm::radians((float)(-90)), glm::vec3(1.0f, 0.0f, 0.0f));
					//modelMatl = glm::translate(modelMatl, glm::vec3(parametricNode.ParametricRule[0].elementsPos[sameobj[i][1] - z]));
					rendsame.push_back(modelMatl);
					mat4 mvMat = modelMatl;
					//原本Procedural的生成位置  
					vec3 objp = mat4tovec3(mvMat, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]);
					modelMatl = glm::translate(modelMatl, glm::vec3(text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]));
					float d1 = dist(campos, objp);
					mvMat = modelMatl;
					//boundingbox
					vec3 bundingmin = mat4tovec3(mvMat, model.objBounding.minbounding);
					float d2 = dist(campos, bundingmin);
					vec3 bundingmax = mat4tovec3(mvMat, model.objBounding.maxbounding);
					float d3 = dist(campos, bundingmax);
					if (d1 < d2 && d1 < d3)
						objspostion.push_back(objp);
					else if (d2 < d1 && d2 < d3)
						objspostion.push_back(bundingmin);
					else
						objspostion.push_back(bundingmax);
					//modelMatl = glm::translate(modelMatl, glm::vec3(text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]));
					objsMat.push_back(mvMat);
				}
				vector<int> takeobj;
				/*drawModelLineShader.Enable();
				drawModelLineShader.SetPMat(vMat);
				drawModelLineShader.SetColor(colors[2]);
				vector<vec3> lines;
				//輔助線
				for (int j = 0; j < text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos.size() - 1; j++) {
					lines.push_back(mat4tovec3(objsMat[i], text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]));//mat4tovec3(objsMat[takeobj[i]], )
					lines.push_back(mat4tovec3(objsMat[i], text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j + 1]));//mat4tovec3(objsMat[takeobj[i]],)
				}
				model.renderObjConnect(lines);
				drawModelLineShader.Disable();*/
				drawModelLineShader.Enable();
				drawModelLineShader.SetPMat(pMat);
				//drawModelLineShader.RoundPixel(3);
				takeobj = roofProceduralLODpos(text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos);
				int  meshstep = 0; bool rendshowone = true;
				for (int j = 0; j < takeobj.size(); j++) {//takeobj
					glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(objsMat[takeobj[j]])));//takeobj[]
					drawModelLineShader.SetNormalMat(normalMat);
					//drawModelLineShader.SetMVMat(pMat);
					drawModelLineShader.SetMVMat(vMat * objsMat[takeobj[j]]);

					drawModelLineShader.SetColor(colors[3]);
					if (dist(campos, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[takeobj[j]]) < d) {
						model.RenderContours();
					}
					if (dist(campos, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[takeobj[j]]) > d || takeobj[j + 1] - takeobj[j] > 1) {
						//有間隔的物件，把點求出			
						vector<vector<int>>  sameObjectMerge;

						drawModelLineShader.SetNormalMat(normalMat);
						drawModelLineShader.SetMVMat(vMat);
						drawModelLineShader.SetColor(colors[1]);

						//if (rendshowone) {
						//drawModelLineShader.SetTex(depthModelTexture);
						vector<vec3> obj;
						if (dist(campos, text.root.child[i].parametricNode[k].ParametricRule[l].elementsPos[j]) < d)
							obj = model.connectTwoObject(objsMat[takeobj[j] + 1], objsMat[takeobj[j + 1] - 1], takeobj[j + 1] - takeobj[j]);
						else
							obj = model.connectTwoObject(objsMat[takeobj[j]], objsMat[takeobj[j + 1] - 1], takeobj[j + 1] - takeobj[j]);
						//lines.insert(lines.end(), obj.begin(),obj.end());
						model.renderObjConnect(obj);
						//	rendshowone = false;
						//}
						if (j + 1 == takeobj.size() - 1)
							break;
					}
				}
				objsMat.clear();
				takeobj.clear();
			}
		}

	}


	drawModelLineShader.Disable();
	///////////////////////////		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);



	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA);



	blendingShader.Enable();//depthModelTexture, colorLineTexture
	//glActiveTexture(GL_TEXTURE0);

	/*glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindTexture(GL_TEXTURE_2D, 0);


	glBindTexture(GL_TEXTURE_2D, colorLineTexture);*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	glUniform1i(glGetUniformLocation(depthModelTexture, "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorLineTexture);
	glUniform1i(glGetUniformLocation(colorLineTexture, "texture2"), 1);
	//blendingShader.SetWidth(windowWidth);
	//blendingShader.SetHeight(windowHeight);
	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	blendingShader.Disable();
	error = glGetError();
	//混合或陰影貼圖比較


	glUseProgram(0);
}

void HierarchyBuildModel(ParametricNode parmetric) {
	//for (int i = 0; i < parmetric.ParametricRule.size();i++) {

	//}
	///攝像機
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();

	int roofbridgenum = abs(dist(parmetric.ParametricRule[0].bezierWay.start, parmetric.ParametricRule[0].bezierWay.end) / model.objBounding.many);
	int roofeaveenum = abs(dist(parmetric.ParametricRule[2].bezierWay.start, parmetric.ParametricRule[2].bezierWay.end) / model.objBounding.minx);
	vector<vec3>  roofd = GetCubicBeizerList(parmetric.ParametricRule[0].bezierWay.start, parmetric.ParametricRule[0].bezierWay.mid, parmetric.ParametricRule[0].bezierWay.end, roofeaveenum);
	vector<vec3>  roofe = GetCubicBeizerList(parmetric.ParametricRule[2].bezierWay.start, parmetric.ParametricRule[2].bezierWay.mid, parmetric.ParametricRule[2].bezierWay.end, roofeaveenum * 2);
	float uvRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 180.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	int stepnum = 10; int j = 0;
	vector<vector<vec3>> stepspos;
	for (int i = roofd.size() - 1; i > 0; i--) {
		vector<vec3> steppos;
		vec3 midpos = vec3((roofd[i][0] + roofe[j][0]) / 2, (roofd[i][1] + roofe[j][1]) / 2 - 5, (roofd[i][2] + roofe[j][2]) / 2);
		if (stepnum > 1) {
			steppos = GetCubicBeizerList(roofd[i], midpos, roofe[j], stepnum);
			stepspos.push_back(steppos);
			stepnum--;
			if (j > 0 & j < 5) {
				drawModelShader.Enable();
				drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
				drawModelShader.UseLighting(true);
				drawModelShader.DrawTexCoord(false);
				drawModelShader.DrawWireframe(false);
				drawModelShader.SetPMat(pMat);
				drawModelShader.SetUVRotMat(uvRotMat);
				drawModelShader.SetFaceColor(colors[6]);

				for (int z = 1; z < steppos.size(); z++) {
					mat4 modelMatl = modelMat;
					modelMatl = glm::rotate(modelMatl, glm::radians((float)(90)), glm::vec3(0.0f, 1.0f, 0.0f));
					modelMatl = glm::translate(modelMatl, steppos[z]);
					drawModelShader.SetMVMat(vMat * modelMatl);
					vec3 line1 = vec3(steppos[z][0] - steppos[z - 1][0], steppos[z][1] - steppos[z - 1][1], steppos[z][2] - steppos[z - 1][2]);
					vec3 line2 = vec3(steppos[z][0] - stepspos[j - 1][z][0], steppos[z][1] - stepspos[j - 1][z][1], steppos[z][2] - stepspos[j - 1][z][2]);
					vec3 normalpos = line1 * line2;
					modelMatl = glm::rotate(modelMatl, glm::radians((float)(90)), normalpos);
					drawModelShader.SetNorm(normalpos);
					glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(vMat * modelMatl)));

					drawModelShader.SetNormalMat(normalMat);
					model.Render();


				}
				drawModelShader.Disable();
			}
			j++;
		}
	}
}

void HierarchyBuildModelP(ParametricNode parmetric) {
	///攝像機
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();


	float uvRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 180.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	int stepnum = 10; int j = 0;
	vector<vector<vec3>> stepspos;


	for (int i = 0; i < parmetric.ParametricRule[0].generateWay.creatnum; i++) {

		for (int j = 0; j < parmetric.ParametricRule[1].generateWay.creatnum; j++) {
			vec3 step = parmetric.ParametricRule[1].generateWay.startpos + vec3(parmetric.ParametricRule[0].generateWay.x * i, 0, parmetric.ParametricRule[1].generateWay.z * j);

			drawModelShader.Enable();
			drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			drawModelShader.UseLighting(true);
			drawModelShader.DrawTexCoord(false);
			drawModelShader.DrawWireframe(false);
			drawModelShader.SetPMat(pMat);
			drawModelShader.SetUVRotMat(uvRotMat);
			drawModelShader.SetFaceColor(colors[6]);
			mat4 modelMatl = modelMat;
			modelMatl = glm::translate(modelMatl, step);
			drawModelShader.SetMVMat(vMat * modelMatl);
			glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(vMat * modelMatl)));
			drawModelShader.SetNormalMat(normalMat);
			model.Render();
			drawModelShader.Disable();
			if (writemodel) {
				modelposfile << step[0] << "," << step[1] << "," << step[2] << "\n";
			}
		}
	}

}

vector<vec3> CompareDepthLine(vec3 linepoint1, vec3 linepoint2, mat4 um4p, mat4 um4mv, float* depthData) {
	vector<vec3> lines;
	///
	///將線的兩點由三維轉螢幕二維
	/// 
	mat4 mvp = um4p * um4mv;
	mat4 MVPinv = inverse(um4p * um4mv);
	vec3 viewpoint1, viewpoint2;
	//vec3 viewpoint1= um4p * um4mv * linepoint1;
	float s[4];
	s[0] = (linepoint1[0] * mvp[0][0]) + (linepoint1[1] * mvp[1][0]) + (linepoint1[2] * mvp[2][0]) + mvp[3][0];
	s[1] = (linepoint1[0] * mvp[0][1]) + (linepoint1[1] * mvp[1][1]) + (linepoint1[2] * mvp[2][1]) + mvp[3][1];
	s[2] = (linepoint1[0] * mvp[0][2]) + (linepoint1[1] * mvp[1][2]) + (linepoint1[2] * mvp[2][2]) + mvp[3][2];
	s[3] = (linepoint1[0] * mvp[0][3]) + (linepoint1[1] * mvp[1][3]) + (linepoint1[2] * mvp[2][3]) + mvp[3][3];

	viewpoint1[0] = s[0] / s[3] * windowWidth / 2 + windowWidth / 2;
	viewpoint1[1] = s[1] / s[3] * windowHeight / 2 + windowHeight / 2;
	viewpoint1[2] = s[2] / s[3];

	s[0] = (linepoint2[0] * mvp[0][0]) + (linepoint2[1] * mvp[1][0]) + (linepoint2[2] * mvp[2][0]) + mvp[3][0];
	s[1] = (linepoint2[0] * mvp[0][1]) + (linepoint2[1] * mvp[1][1]) + (linepoint2[2] * mvp[2][1]) + mvp[3][1];
	s[2] = (linepoint2[0] * mvp[0][2]) + (linepoint2[1] * mvp[1][2]) + (linepoint2[2] * mvp[2][2]) + mvp[3][2];
	s[3] = (linepoint2[0] * mvp[0][3]) + (linepoint2[1] * mvp[1][3]) + (linepoint2[2] * mvp[2][3]) + mvp[3][3];

	viewpoint2[0] = s[0] / s[3] * windowWidth / 2 + windowWidth / 2;
	viewpoint2[1] = s[1] / s[3] * windowHeight / 2 + windowHeight / 2;
	viewpoint2[2] = s[2] / s[3];
	/// 
	/// 取得對應位置的深度值，使用depthModelTexture
	/// 
	//vec4 depthtexture= glGetTextureImage(depthModelTexture, viewpoint1.xy, 0);
	//GLubyte* data = new GLubyte[windowWidth * windowHeight * 4];
	/// 
	/// 依算出的部分輸出回值值
	/// 1.皆小於depth model value，直接回傳原始線段
	/// 2.有一點大於depth model value，從小於的點開始延著另一點走直到深度大於depth
	/// 3.若兩點都大於depth model value，回傳空值
	/// 
	/// 1.皆小於depth model value，直接回傳原始線段
	int step = viewpoint1[0] * viewpoint1[1];
	if (viewpoint1[2] < depthData[step] && viewpoint2[2] < depthData[step]) {
		lines.push_back(linepoint1);
		lines.push_back(linepoint2);
	}
	/// 2.有一點大於depth model value，從小於的點開始延著另一點走直到深度大於depth
	else if (viewpoint1[2] < depthData[step]) {
		bool isInNear = false;
		vec2 nearPos;
		vec3 midPos = viewpoint2;
		step = viewpoint1[0] * viewpoint1[1];
		while (isInNear == false) {
			nearPos = vec2((viewpoint1[0] + midPos[0]) / 2, (viewpoint1[1] + midPos[1]) / 2);
			float pointdepth = (viewpoint1[2] + midPos[2]) / 2;
			//近就可以算回世界座標
			if (pointdepth < depthData[step]) {
				//將算出的2維座標轉3維座標
				float p[4];
				float s1[3];
				s1[0] = 2 * nearPos[0] / windowWidth - 1;
				s1[1] = 2 * nearPos[1] / windowHeight - 1;
				s1[2] = nearPos[2];
				p[0] = (s1[0] * MVPinv[0][0]) + (s1[1] * MVPinv[1][0]) + (s1[2] * MVPinv[2][0]) + MVPinv[3][0];
				p[1] = (s1[0] * MVPinv[0][1]) + (s1[1] * MVPinv[1][1]) + (s1[2] * MVPinv[2][1]) + MVPinv[3][1];
				p[2] = (s1[0] * MVPinv[0][2]) + (s1[1] * MVPinv[1][2]) + (s1[2] * MVPinv[2][2]) + MVPinv[3][2];
				p[3] = (s1[0] * MVPinv[0][3]) + (s1[1] * MVPinv[1][3]) + (s1[2] * MVPinv[2][3]) + MVPinv[3][3];
				vec3 world;
				float w = 1 / p[3];
				world[0] = p[0] * w;
				world[1] = p[1] * w;
				world[2] = p[2] * w;
				lines.push_back(linepoint1);
				lines.push_back(world);
				isInNear = true;
			}
			else {
				midPos = vec3(nearPos[0], nearPos[1], pointdepth);
			}
		}
	}
	else if (viewpoint2[2] < depthData[step]) {
		bool isInNear = false;
		vec2 nearPos;
		vec3 midPos = viewpoint1;
		step = viewpoint1[0] * viewpoint1[1];
		while (isInNear == false) {
			nearPos = vec2((viewpoint2[0] + midPos[0]) / 2, (viewpoint2[1] + midPos[1]) / 2);
			float pointdepth = (viewpoint2[2] + midPos[2]) / 2;
			//近就可以算回世界座標
			if (pointdepth < depthData[step]) {
				//將算出的2維座標轉3維座標
				float p[4];
				float s1[3];
				s1[0] = 2 * nearPos[0] / windowWidth - 1;
				s1[1] = 2 * nearPos[1] / windowHeight - 1;
				s1[2] = nearPos[2];
				p[0] = (s1[0] * MVPinv[0][0]) + (s1[1] * MVPinv[1][0]) + (s1[2] * MVPinv[2][0]) + MVPinv[3][0];
				p[1] = (s1[0] * MVPinv[0][1]) + (s1[1] * MVPinv[1][1]) + (s1[2] * MVPinv[2][1]) + MVPinv[3][1];
				p[2] = (s1[0] * MVPinv[0][2]) + (s1[1] * MVPinv[1][2]) + (s1[2] * MVPinv[2][2]) + MVPinv[3][2];
				p[3] = (s1[0] * MVPinv[0][3]) + (s1[1] * MVPinv[1][3]) + (s1[2] * MVPinv[2][3]) + MVPinv[3][3];
				vec3 world;
				float w = 1 / p[3];
				world[0] = p[0] * w;
				world[1] = p[1] * w;
				world[2] = p[2] * w;
				lines.push_back(world);
				lines.push_back(linepoint2);
				isInNear = true;
			}
			else {
				midPos = vec3(nearPos[0], nearPos[1], pointdepth);
			}
		}

	}
	/// 3.若兩點都大於depth model value，回傳空值
	return lines;
}
vector<vec3> CompareDepthLine(vec3 linepoint1, vec3 linepoint2, mat4 um4p, mat4 um4mv) {
	vector<vec3> lines;
	///
	///將線的兩點由三維轉螢幕二維
	/// 
	mat4 mvp = um4p * um4mv;
	mat4 MVPinv = inverse(um4p * um4mv);
	vec3 viewpoint1, viewpoint2;
	//vec3 viewpoint1= um4p * um4mv * linepoint1;
	float s[4];
	s[0] = (linepoint1[0] * mvp[0][0]) + (linepoint1[1] * mvp[1][0]) + (linepoint1[2] * mvp[2][0]) + mvp[3][0];
	s[1] = (linepoint1[0] * mvp[0][1]) + (linepoint1[1] * mvp[1][1]) + (linepoint1[2] * mvp[2][1]) + mvp[3][1];
	s[2] = (linepoint1[0] * mvp[0][2]) + (linepoint1[1] * mvp[1][2]) + (linepoint1[2] * mvp[2][2]) + mvp[3][2];
	s[3] = (linepoint1[0] * mvp[0][3]) + (linepoint1[1] * mvp[1][3]) + (linepoint1[2] * mvp[2][3]) + mvp[3][3];

	viewpoint1[0] = s[0] / s[3] * windowWidth / 2 + windowWidth / 2;
	viewpoint1[1] = s[1] / s[3] * windowHeight / 2 + windowHeight / 2;
	viewpoint1[2] = s[2] / s[3];

	s[0] = (linepoint2[0] * mvp[0][0]) + (linepoint2[1] * mvp[1][0]) + (linepoint2[2] * mvp[2][0]) + mvp[3][0];
	s[1] = (linepoint2[0] * mvp[0][1]) + (linepoint2[1] * mvp[1][1]) + (linepoint2[2] * mvp[2][1]) + mvp[3][1];
	s[2] = (linepoint2[0] * mvp[0][2]) + (linepoint2[1] * mvp[1][2]) + (linepoint2[2] * mvp[2][2]) + mvp[3][2];
	s[3] = (linepoint2[0] * mvp[0][3]) + (linepoint2[1] * mvp[1][3]) + (linepoint2[2] * mvp[2][3]) + mvp[3][3];

	viewpoint2[0] = s[0] / s[3] * windowWidth / 2 + windowWidth / 2;
	viewpoint2[1] = s[1] / s[3] * windowHeight / 2 + windowHeight / 2;
	viewpoint2[2] = s[2] / s[3];
	/// 
	/// 取得對應位置的深度值，使用depthModelTexture
	/// 
	//vec4 depthtexture= glGetTextureImage(depthModelTexture, viewpoint1.xy, 0);
	//GLubyte* data = new GLubyte[windowWidth * windowHeight * 4];
	/// 
	/// 依算出的部分輸出回值值
	/// 1.皆小於depth model value，直接回傳原始線段
	/// 2.有一點大於depth model value，從小於的點開始延著另一點走直到深度大於depth
	/// 3.若兩點都大於depth model value，回傳空值
	/// 
	/// 1.皆小於depth model value，直接回傳原始線段
	int step = viewpoint1[0] * viewpoint1[1];
	float depth, depth1;
	glReadPixels(viewpoint1[0], viewpoint1[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	glReadPixels(viewpoint2[0], viewpoint2[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth1);
	if (viewpoint1[2] < depth && viewpoint2[2] < depth1) {
		lines.push_back(linepoint1);
		lines.push_back(linepoint2);
	}
	/// 2.有一點大於depth model value，從小於的點開始延著另一點走直到深度大於depth
	/*else if (viewpoint1[2] < depth) {
		bool isInNear = false;
		vec2 nearPos;
		vec3 midPos = viewpoint2;
		step = viewpoint1[0] * viewpoint1[1];
		while (isInNear == false) {
			nearPos = vec2((viewpoint1[0] + midPos[0]) / 2, (viewpoint1[1] + midPos[1]) / 2);
			float pointdepth = (viewpoint1[2] + midPos[2]) / 2;
			//近就可以算回世界座標
			if (pointdepth < depth) {
				//將算出的2維座標轉3維座標
				float p[4];
				float s1[3];
				s1[0] = 2 * nearPos[0] / windowWidth - 1;
				s1[1] = 2 * nearPos[1] / windowHeight - 1;
				s1[2] = nearPos[2];
				p[0] = (s1[0] * MVPinv[0][0]) + (s1[1] * MVPinv[1][0]) + (s1[2] * MVPinv[2][0]) + MVPinv[3][0];
				p[1] = (s1[0] * MVPinv[0][1]) + (s1[1] * MVPinv[1][1]) + (s1[2] * MVPinv[2][1]) + MVPinv[3][1];
				p[2] = (s1[0] * MVPinv[0][2]) + (s1[1] * MVPinv[1][2]) + (s1[2] * MVPinv[2][2]) + MVPinv[3][2];
				p[3] = (s1[0] * MVPinv[0][3]) + (s1[1] * MVPinv[1][3]) + (s1[2] * MVPinv[2][3]) + MVPinv[3][3];
				vec3 world;
				float w = 1 / p[3];
				world[0] = p[0] * w;
				world[1] = p[1] * w;
				world[2] = p[2] * w;
				lines.push_back(linepoint1);
				lines.push_back(world);
				isInNear = true;
			}
			else {
				midPos = vec3(nearPos[0], nearPos[1], pointdepth);
			}
		}
	}
	else if (viewpoint2[2] < depth1) {
		bool isInNear = false;
		vec2 nearPos;
		vec3 midPos = viewpoint1;
		step = viewpoint1[0] * viewpoint1[1];
		while (isInNear == false) {
			nearPos = vec2((viewpoint2[0] + midPos[0]) / 2, (viewpoint2[1] + midPos[1]) / 2);
			float pointdepth = (viewpoint2[2] + midPos[2]) / 2;
			//近就可以算回世界座標
			if (pointdepth < depth1) {
				//將算出的2維座標轉3維座標
				float p[4];
				float s1[3];
				s1[0] = 2 * nearPos[0] / windowWidth - 1;
				s1[1] = 2 * nearPos[1] / windowHeight - 1;
				s1[2] = nearPos[2];
				p[0] = (s1[0] * MVPinv[0][0]) + (s1[1] * MVPinv[1][0]) + (s1[2] * MVPinv[2][0]) + MVPinv[3][0];
				p[1] = (s1[0] * MVPinv[0][1]) + (s1[1] * MVPinv[1][1]) + (s1[2] * MVPinv[2][1]) + MVPinv[3][1];
				p[2] = (s1[0] * MVPinv[0][2]) + (s1[1] * MVPinv[1][2]) + (s1[2] * MVPinv[2][2]) + MVPinv[3][2];
				p[3] = (s1[0] * MVPinv[0][3]) + (s1[1] * MVPinv[1][3]) + (s1[2] * MVPinv[2][3]) + MVPinv[3][3];
				vec3 world;
				float w = 1 / p[3];
				world[0] = p[0] * w;
				world[1] = p[1] * w;
				world[2] = p[2] * w;
				lines.push_back(world);
				lines.push_back(linepoint2);
				isInNear = true;
			}
			else {
				midPos = vec3(nearPos[0], nearPos[1], pointdepth);
			}
		}

	}*/
	/// 3.若兩點都大於depth model value，回傳空值
	return lines;
}

float proximity = 20;
void HierarchyBuildLineP(ParametricNode parmetric) {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	int step = 1, bstep;
	float d1 = dist(campos, parmetric.ParametricRule[0].generateWay.startpos);
	if (d1 > proximity) {
		step = density;
		bstep = density;
	}

	for (int i = 0; i < parmetric.ParametricRule[0].generateWay.creatnum; i++) {
		vector<vec3> producalpos;
		vector<mat4> objsMat;
		if (writemodelline) {
			float prox = parmetric.ParametricRule[0].generateWay.x / (abs(model.objBounding.manx) * 2);
			modellinefile << "roofsurface" << i << ",Regularity=1,Proximity=" << prox << ",Similarity=1\n";
		}
		for (int j = 0; j < parmetric.ParametricRule[1].generateWay.creatnum; j++) {

			vec3 postation = parmetric.ParametricRule[1].generateWay.startpos + vec3(parmetric.ParametricRule[0].generateWay.x * i, 0, parmetric.ParametricRule[1].generateWay.z * j);
			producalpos.push_back(postation);
			mat4 modelMatl = modelMat;
			modelMatl = glm::translate(modelMatl, postation);
			objsMat.push_back(modelMatl);
			if (writemodelline) {
				float prox = parmetric.ParametricRule[1].generateWay.z / (abs(model.objBounding.manz) * 2);
				modellinefile << "roofobject" << ",Regularity=1,Proximity=" << prox << ",Similarity=1\n";
				for (int z = 0; z < model.meshEdgeGroup.size(); z++) {
					if (model.meshEdgeGroup[z].structlines)
					{
						modellinefile << "horizontalline" << endl;
						model.SaveTreeNode(model.meshEdgeGroup[z].linestree, objsMat[j], modellinefile);
						modellinefile << "end" << "\n";
					}
					else if (model.meshEdgeGroup[z].verticallines) {
						modellinefile << "verticalline" << endl;
						model.SaveTreeNode(model.meshEdgeGroup[z].linestree, objsMat[j], modellinefile);
						modellinefile << "end" << "\n";
					}
				}
				modellinefile << "modelend" << "\n";
			}
		}

		vector<int> takeobj;
		takeobj = roofProceduralLODpos(producalpos);
		mat4 modelMatl = modelMat;
		modelMatl = glm::translate(modelMatl, producalpos[producalpos.size() / 2]);

		drawModelLineShader.Enable();
		drawModelLineShader.SetPMat(pMat);
		//drawModelLineShader.RoundPixel(3);
		glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(vMat)));
		drawModelLineShader.SetNormalMat(normalMat);
		drawModelLineShader.SetMVMat(vMat);

		drawModelLineShader.SetColor(colors[1]);
		//drawModelLineShader.SetTex(depthModelTexture);
		vector<vec3> obj, horizontalline;
		horizontalline = model.connectTwoObject(objsMat[0], objsMat[objsMat.size() / 2], objsMat[objsMat.size() - 1], 0);


		obj = horizontalline;
		model.renderObjConnect(horizontalline);
		if (takeobj[0] != 0)
			takeobj.insert(takeobj.begin(), 0);
		if (takeobj[takeobj.size() - 1] != objsMat.size() - 1)
			takeobj.push_back(objsMat.size() - 1);

		for (int j = 0; j < takeobj.size() - 1; j++) {

			vector<vec3> verticalline;

			if (dist(campos, producalpos[takeobj[j]]) < proximity)
				verticalline = model.connectTwoObject(objsMat[takeobj[j]], objsMat[takeobj[j + 1]], takeobj[j + 1] - takeobj[j]);//takeobj[j + 1] - takeobj[j]
			else
				verticalline = model.connectTwoObject(objsMat[takeobj[j]], objsMat[takeobj[j + 1]], takeobj[j + 1] - takeobj[j]);// takeobj[j + 1] - takeobj[j]

			obj.insert(obj.end(), verticalline.begin(), verticalline.end());
			model.renderObjConnect(verticalline);

			verticalline.clear();
		}


		drawModelLineShader.Disable();

	}
}

void RoofSurface() {
	glBindFramebuffer(GL_FRAMEBUFFER, depthModelFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	HierarchyBuildModelP(roofSTest);
	if (writemodel) {
		modelposfile.close();
		writemodel = false;
	}

	// 繪製線段
	glBindFramebuffer(GL_FRAMEBUFFER, colorLineFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


	//float* sdata;
	HierarchyBuildLineP(roofSTest);
	if (writemodelline) {
		modellinefile.close();
		writemodelline = false;
	}
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*blendingShader.Enable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	glUniform1i(glGetUniformLocation(depthModelTexture, "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorLineTexture);
	glUniform1i(glGetUniformLocation(colorLineTexture, "texture2"), 1);

	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	blendingShader.Disable();*/
	glUseProgram(0);
}

// 取得物體在攝像機視野中的佔比
float GetObjectInViewPercentage(BoundingBox boundbox, mat4 viewProjMatrix)
{
	vec4 minPos = vec4(boundbox.min, 1.0);
	vec4 maxPos = vec4(boundbox.max, 1.0);

	// 將bounding box的8個角落點轉換成投影空間
	minPos = viewProjMatrix * minPos;
	maxPos = viewProjMatrix * maxPos;

	// 將投影空間中的點除以w，得到NDC座標系統中的點
	minPos /= minPos.w;
	maxPos /= maxPos.w;

	// 將NDC座標系統中的點轉換成視口座標系統中的點
	minPos = vec4((minPos.x + 1.0) * 0.5, (minPos.y + 1.0) * 0.5, (minPos.z + 1.0) * 0.5, 1.0);
	maxPos = vec4((maxPos.x + 1.0) * 0.5, (maxPos.y + 1.0) * 0.5, (maxPos.z + 1.0) * 0.5, 1.0);

	// 計算bounding box在視口中的寬度和高度
	float width = abs(maxPos.x - minPos.x);
	float height = abs(maxPos.y - minPos.y);

	// 計算bounding box的面積
	float area = width * height;

	// 計算bounding box佔整個視口的面積比例
	float viewArea = 2.0f * 0.5f * (1.0f - minPos.z) * area;
	float viewportArea = 2.0f * 0.5f * (1.0f - minPos.z);

	return viewArea / viewportArea;
}


void drawModel(TreeNode p, int interval){}
void drawModel(TreeNode p) {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	float uvRotateAngle = 0.0;
	float prevUVRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 190.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	bool gothough = false;
	int interval = 1;
	
	bool gothoughid = false;
	for (int i = 0; i < modelrender.size(); i++)
		if (modelrender[i] == p.treeNodeID)
			gothoughid = true;
	//if(gothoughid==false){
	modelrender.push_back(p.treeNodeID);
	if (p.parmetricNode.elementsPos.size() > 0) {
		float areaView = GetObjectInViewPercentage(p.boundbox, pMat);
		bool changefarornear = false;//false->near,true->true
		int startpoint;
		mat4 startmat, endmat;
		vector<mat4> eachmat;
		cout << "buildmodelid," << p.treeNodeID << endl;
		vector<int> printveclist;
		
		if (printveclist.size() == 0) {
			for (int z = 0; z < p.parmetricNode.elementsPos.size(); z += interval) {
				printveclist.push_back(z);
				if (z + interval > p.parmetricNode.elementsPos.size() && p.parmetricNode.elementsPos.size() > 1)
				{
					printveclist.push_back(p.parmetricNode.elementsPos.size() - 1);
				}
			}
		}
		for (int z = 0; z < printveclist.size(); z++) {
			//bool farornear;//far==true,near=false
			drawModelShader.Enable();
			bool drawTexture = false;
			drawModelShader.SetWireColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			//drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			drawModelShader.UseLighting(false);
			drawModelShader.DrawTexCoord(false);
			drawModelShader.DrawWireframe(false);
			drawModelShader.SetPMat(pMat);
			drawModelShader.SetUVRotMat(uvRotMat);
			glBindTexture(GL_TEXTURE_2D, modeltexture);
			glm::mat4 modelMatm = modelMat;
			modelMatm = modelMat;
			drawModelShader.SetFaceColor(colors[6]);
			modelMatm = glm::scale(modelMatm, models[p.parmetricNode.meshnum].scareObj);
			modelMatm = glm::translate(modelMatm, glm::vec3(p.parmetricNode.elementsPos[printveclist[z]]));
			if (p.parmetricNode.generateNormal != vec3(0, 0, 0))
				modelMatm = glm::rotate(modelMatm, glm::radians((float)(90 * p.parmetricNode.generateNormal.y)), glm::vec3(p.parmetricNode.generateNormal));
			
			mvMat = vMat * modelMatm;
			glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(mvMat)));
			drawModelShader.SetNormalMat(normalMat);
			drawModelShader.SetMVMat(mvMat);

			models[p.parmetricNode.meshnum].Render();
			glBindTexture(GL_TEXTURE_2D, 0);
			drawModelShader.Disable();

		}
		//}
	}
	if (p.childNodes.size() > 0) {
		for (int i = 0; i < p.childNodes.size(); i++)
			drawModel(p.childNodes[i]);//drawModel(p.childNodes[i], interval);
	}
}

// Compute the minimum and maximum x, y, and z values for a set of 3D line segments
void computeBoundingBox(std::vector<vec3>& lineSegments, float& minX, float& minY, float& minZ, float& maxX, float& maxY, float& maxZ) {
	// Initialize the minimum and maximum values to the first point
	minX = maxX = lineSegments[0].x;
	minY = maxY = lineSegments[0].y;
	minZ = maxZ = lineSegments[0].z;

	// Iterate over the remaining points and update the minimum and maximum values
	for (int i = 1; i < lineSegments.size(); i++) {
		vec3& p = lineSegments[i];
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		minZ = std::min(minZ, p.z);
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
		maxZ = std::max(maxZ, p.z);
	}
}

// Merge two bounding boxes in 3D space into a single bounding box
/*void mergeBoundingBoxes(vector<vec3>& lineSegments1, vector<vec3>& lineSegments2, vector<vec3>& mergedBoundingBox) {
	float minX1, minY1, minZ1, maxX1, maxY1, maxZ1;
	computeBoundingBox(lineSegments1, minX1, minY1, minZ1, maxX1, maxY1, maxZ1);

	float minX2, minY2, minZ2, maxX2, maxY2, maxZ2;
	computeBoundingBox(lineSegments2, minX2, minY2, minZ2, maxX2, maxY2, maxZ2);

	double minX = std::min(minX1, minX2);
	double minY = std::min(minY1, minY2);
	double minZ = std::min(minZ1, minZ2);
	double maxX = std::max(maxX1, maxX2);
	double maxY = std::max(maxY1, maxY2);
	double maxZ = std::max(maxZ1, maxZ2);

	// Add the twelve line segments of the merged bounding box
	

	// Add the twelve line segments of the merged bounding box
	vec3 minP = { minX, minY, minZ };
	vec3 maxP = { maxX, maxY, maxZ };

	
	vec3 vertices[] = {
		{ minP.x, minP.y, minP.z },
		{ minP.x, maxP.y, minP.z },
		{ maxP.x, maxP.y, minP.z },
		{ maxP.x, minP.y, minP.z },
		{ minP.x, minP.y, maxP.z },
		{ minP.x, maxP.y, maxP.z },
		{ maxP.x, maxP.y, maxP.z },
		{ maxP.x, minP.y, maxP.z }
	};

	int indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};
	for (int i = 0; i < 24; i++) {
		mergedBoundingBox.push_back(vertices[indices[i]]);
	}
}*/
void mergeBoundingBoxes(vector<vec3>& lineSegments1, vector<vec3>& lineSegments2, vector<vec3>& mergedBoundingBox) {
	// Compute the bounding boxes of the two sets of line segments
	float minX1, minY1, minZ1, maxX1, maxY1, maxZ1;
	computeBoundingBox(lineSegments1, minX1, minY1, minZ1, maxX1, maxY1, maxZ1);
	float minX2, minY2, minZ2, maxX2, maxY2, maxZ2;
	computeBoundingBox(lineSegments2, minX2, minY2, minZ2, maxX2, maxY2, maxZ2);

	// Compute the minimum and maximum coordinates of the merged bounding box
	float minX = std::min(minX1, minX2);
	float minY = std::min(minY1, minY2);
	float minZ = std::min(minZ1, minZ2);
	float maxX = std::max(maxX1, maxX2);
	float maxY = std::max(maxY1, maxY2);
	float maxZ = std::max(maxZ1, maxZ2);

	// Define the eight vertices of the merged bounding box
	vec3 vertices[8] = {
		{minX, minY, minZ},
		{minX, minY, maxZ},
		{minX, maxY, minZ},
		{minX, maxY, maxZ},
		{maxX, minY, minZ},
		{maxX, minY, maxZ},
		{maxX, maxY, minZ},
		{maxX, maxY, maxZ},
	};

	// Define the twelve edges of the merged bounding box
	int edges[12][2] = {
		{0, 1}, {0, 2}, {0, 4},
		{3, 2}, {3, 1}, {3, 7},
		{6, 2}, {6, 4}, {6, 7},
		{5, 1}, {5, 4}, {5, 7},
	};

	// Add the edges to the merged bounding box
	for (int i = 0; i < 12; i++) {
		mergedBoundingBox.push_back(vertices[edges[i][0]]);
		mergedBoundingBox.push_back(vertices[edges[i][1]]);
	}
}

void drawModelLine(TreeNode p) {

	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();

	//計算BoundingBox的寬度、高度、深度
	float width = nodes[p.treeNodeID].boundbox.max.x - nodes[p.treeNodeID].boundbox.min.x;
	float height = nodes[p.treeNodeID].boundbox.max.y - nodes[p.treeNodeID].boundbox.min.y;
	float depth = nodes[p.treeNodeID].boundbox.max.z - nodes[p.treeNodeID].boundbox.min.z;
	//計算BoundingBox的對角線長度
	float diagonal_length = sqrt(width * width + height * height + depth * depth);
	//計算物體中心點的位置
	float center_x = (nodes[p.treeNodeID].boundbox.max.x + nodes[p.treeNodeID].boundbox.min.x) / 2;
	float center_y = (nodes[p.treeNodeID].boundbox.max.y + nodes[p.treeNodeID].boundbox.min.y) / 2;
	float center_z = (nodes[p.treeNodeID].boundbox.max.z + nodes[p.treeNodeID].boundbox.min.z) / 2;
	//計算攝像機到物體中心點的距離
	float distance = sqrt(pow((campos.x - center_x), 2) + pow((campos.y - center_y), 2) + pow((campos.z - center_z), 2));
	//計算物體在攝像機視野中的佔比
	float percentage = diagonal_length / distance;
	cout << "percentage" << percentage << endl;
	int interval = 1;
	vector<int> printveclist;
	bool gothought = false;
	//if (linerenderb[p.treeNodeID] == true)
		//gothought = true;
	
	//if (gothought == false) {
		TreeNode mergep1, mergep2;
		bool mergeb = false;
		
		//if (mergeb==false) {
			if (p.parmetricNode.elementsPos.size() > 0) {//&& percentage>0.1

				vector<mat4> eachmat;

				for (int z = 0; z < p.parmetricNode.elementsPos.size(); z++) {
					BoundingBox bbox;
					bbox.min = models[p.parmetricNode.meshnum].objBounding.minbounding;
					bbox.max = models[p.parmetricNode.meshnum].objBounding.maxbounding;
					float areaView = GetObjectInViewPercentage(bbox, pMat);
					if (areaView > 0.5)
					{
						interval = 3;
					}
					else if (areaView > 0.25)
					{
						interval = 2;
					}
					else if (areaView > 0.1)
					{
						interval = 2;
					}
					else if (areaView > 0.01)
					{
						interval = 1;
					}
					drawModelLineShader.Enable();
					drawModelLineShader.SetPMat(pMat);
					drawModelLineShader.SetColor(colors[5]);
					glBindTexture(GL_TEXTURE_2D, imgtext);
					glm::mat4 modelMatl = modelMat;
					modelMatl = glm::scale(modelMatl, models[p.parmetricNode.meshnum].scareObj);
					modelMatl = glm::translate(modelMatl, glm::vec3(p.parmetricNode.elementsPos[z]));
					if (p.parmetricNode.generateNormal != vec3(0, 0, 0)) //normal.cross(OpenMesh::Vec3f(0, 1, 0)
						modelMatl = glm::rotate(modelMatl, glm::radians((float)(90 * p.parmetricNode.generateNormal.y)), glm::vec3(p.parmetricNode.generateNormal));

					mvMat = vMat * modelMatl;
					eachmat.push_back(mvMat);
					glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(mvMat)));
					float startToCamera = dist(campos, p.parmetricNode.elementsPos[z]);
					modelMatl = modelMat;
					drawModelLineShader.SetNormalMat(normalMat);
					drawModelLineShader.SetMVMat(mvMat);
					glDepthMask(GL_FALSE);
					vector<vec3> linetreeshow;

					if (interval == 1)
						linetreeshow = models[p.parmetricNode.meshnum].boundingline;
					else if (interval == 2)
						linetreeshow = models[p.parmetricNode.meshnum].convexline;
					else
					{
						for (int j = 0; j < models[p.parmetricNode.meshnum].meshEdgeGroup.size(); j++) {
							// Link it to the tweak bar
							models[p.parmetricNode.meshnum].meshEdgeGroup[j].linestree;
							
						}
					}

					models[p.parmetricNode.meshnum].renderObjConnect(linetreeshow);
					glDepthMask(GL_TRUE);
					glBindTexture(GL_TEXTURE_2D, 0);
					drawModelLineShader.Disable();
				}
			}

			if (p.childNodes.size() > 0) {
				for (int i = 0; i < p.childNodes.size(); i++)
					drawModelLine(p.childNodes[i]);
			}
		//}
		
	//}
}
void drawModelLine(TreeNode p, TreeNode p1) {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();
	bool gothought = false;

	linerenderb[p.treeNodeID] = true;
	linerenderb[p1.treeNodeID] = true;
	if (p.parmetricNode.elementsPos.size() > 0) {
		vector<mat4> eachmat;

		for (int z = 0; z < p.parmetricNode.elementsPos.size(); z++) {
			BoundingBox bbox;
			bbox.min = models[p.parmetricNode.meshnum].objBounding.minbounding;
			bbox.max = models[p.parmetricNode.meshnum].objBounding.maxbounding;
			float areaView = GetObjectInViewPercentage(bbox, pMat);
			glm::mat4 modelMatl = modelMat;

			vector<vec3> linetreeshow, linetreeshow2;
			modelMatl = modelMat;
			modelMatl = glm::scale(modelMatl, models[p.parmetricNode.meshnum].scareObj);
			modelMatl = glm::translate(modelMatl, glm::vec3(p.parmetricNode.elementsPos[z]));
			if (p.parmetricNode.generateNormal != vec3(0, 0, 0))
				modelMatl = glm::rotate(modelMatl, glm::radians((float)(90 * p.parmetricNode.generateNormal.y)), glm::vec3(p.parmetricNode.generateNormal));
			linetreeshow = models[p.parmetricNode.meshnum].boundinglinchangeMat4(modelMatl);

			modelMatl = modelMat;
			modelMatl = glm::scale(modelMatl, models[p1.parmetricNode.meshnum].scareObj);
			modelMatl = glm::translate(modelMatl, glm::vec3(p1.parmetricNode.elementsPos[z]));
			if (p1.parmetricNode.generateNormal != vec3(0, 0, 0))
				modelMatl = glm::rotate(modelMatl, glm::radians((float)(90 * p1.parmetricNode.generateNormal.y)), glm::vec3(p1.parmetricNode.generateNormal));
			linetreeshow2 = models[p1.parmetricNode.meshnum].boundinglinchangeMat4(modelMatl);

			vector<vec3> linetreeshow3;
			linetreeshow3.insert(linetreeshow3.end(), linetreeshow.begin(),linetreeshow.end());
			linetreeshow3.insert(linetreeshow3.end(), linetreeshow2.begin(), linetreeshow2.end());
			mergeBoundingBoxes(linetreeshow, linetreeshow2, linetreeshow3);
			drawModelLineShader.Enable();
			drawModelLineShader.SetPMat(pMat);
			drawModelLineShader.SetColor(colors[5]);
			glBindTexture(GL_TEXTURE_2D, imgtext);
			modelMatl = modelMat;
			mvMat = vMat * modelMatl;
			eachmat.push_back(mvMat);
			modelMatl = modelMat;
			drawModelLineShader.SetPMat(pMat);

			glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(vMat)));
			drawModelLineShader.SetNormalMat(normalMat);
			drawModelLineShader.SetMVMat(vMat);
			glDepthMask(GL_FALSE);
			models[p.parmetricNode.meshnum].renderObjConnect(linetreeshow);
			models[p.parmetricNode.meshnum].renderObjConnect(linetreeshow2);
			models[p.parmetricNode.meshnum].renderObjConnect(linetreeshow3);
			//linetreeshow.clear();
			//linetreeshow2.clear();
			glDepthMask(GL_TRUE);
			glBindTexture(GL_TEXTURE_2D, 0);
			drawModelLineShader.Disable();
		}
		for (int i = 0; i < nodes[p.treeNodeID].childNodes.size(); i++)
		{
			drawModelLine(nodes[p.treeNodeID].childNodes[i]);
		}
	}
}
void mergeAndChooseSmallerVolume(TreeNode& node, std::vector<TreeNode>& similarityNodes, std::vector<TreeNode>& proximityNodes) {
	// Calculate the total volume of each group
	double similarityVolume = 0.0;
	for (const auto& n : similarityNodes) {
		similarityVolume += n.volume;
	}

	double proximityVolume = 0.0;
	for (const auto& n : proximityNodes) {
		proximityVolume += n.volume;
	}

	// Choose the group with the smaller volume
	if (similarityVolume < proximityVolume) {
		// Merge node into similarityNodes group
		similarityNodes.push_back(node);
	}
	else {
		// Merge node into proximityNodes group
		proximityNodes.push_back(node);
	}
}
void renderBasedOnVolume(TreeNode& node, double allvolume) {
	// Calculate the relative volume of the node
	double relativeVolume = node.volume / allvolume;

	// Render the node based on its relative volume
	// The exact way to do this will depend on your application

	// For example, we could print out the node's volume and relative volume:
	std::cout << "Node volume: " << node.volume << std::endl;
	std::cout << "Relative volume: " << relativeVolume << std::endl;
}
void traverseTree(TreeNode& node) {
	// Process current node
	// Note: This would be a good place to handle the similarity and proximity 
	// node checks, as well as the Level of Detail (lod) merging and rendering.

	// 檢查節點是否同時在similarityNodes及proximityNodes中存在
	bool inSimilarityNodes = nodes[node.treeNodeID].similarity;
	bool inProximityNodes = nodes[node.treeNodeID].proximity;

	// 如果節點存在於兩者之中，則選擇體積比較小的分群進行合併
	if (inSimilarityNodes && inProximityNodes) {
		// 進行合併並選擇體積比較小的分群
		mergeAndChooseSmallerVolume(node, similarityNodes, proximityNodes);
	}

	// 根據每個分群的體積與整體體積的比例進行Level of Detail (lod)的合併顯示
	renderBasedOnVolume(node, allvolume);

	// Traverse child nodes
	for (auto& child : node.childNodes) {
		traverseTree(child);
	}
}

void renderHouseTree() {
	// Update group volumes based on conditions
	for (auto& node : houseTree.childNodes) {
		float similarityVolume = 0.0f, proximityVolume = 0.0f;
		if (node.similarity) {
			similarityVolume = similarityNodes[node.similarityG].volume;
		}
		if (node.proximity) {
			proximityVolume = proximityNodes[node.proximityG].volume;
		}
		node.volume = (similarityVolume < proximityVolume) ? similarityVolume : proximityVolume;
	}

	// Update Level of Detail (lod) based on group volume ratios
	for (auto& node : houseTree.childNodes) {
		node.lodt= node.volume / allvolume;
		std::cout << "Node node.lodt: " << node.lodt << std::endl;
		// Use node.lod to adjust the detail level in rendering...
	}

	// Traverse the tree hierarchy for rendering
	traverseTree(houseTree);
}


void HouseTest() {
	glGenTextures(1, &modeltexture);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glBindFramebuffer(GL_FRAMEBUFFER, model_buffer);
	vector<mat4> rendsame;
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rule建立
	//先建立平面

	//生成物件
	int camfar = 4, camfarnum = 0;
	vector<vector<int>> sameobj;
	vector<float> eachnodedist;
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	RenderGroudVAO();
	drawModel(houseTree);
	
	for (int i = 0; i < proximityNodes[0].childNodes.size(); i += 2)
		drawModelLine(proximityNodes[0].childNodes[i], proximityNodes[0].childNodes[i+1]);

	for (int i = 0; i < similarityNodes.size();i++)
		for(int j=0;j< similarityNodes[i].childNodes.size();j+=2)
			if (linerenderb[similarityNodes[i].childNodes[j].treeNodeID] == false)
				drawModelLine(similarityNodes[i].childNodes[j], similarityNodes[i].childNodes[j + 1]);
	for (int i = 0; i < linerenderb.size(); i++)
		linerenderb[i] = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(0);
}
void ProceduralTest() {
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();


	vector<mat4> rendsame;
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Rule建立
	//先建立平面
	float uvRotateAngle = 0.0;
	float prevUVRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 190.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	int d = 30;
	//生成物件
	int camfar = 4, camfarnum = 0;
	vector<vector<int>> sameobj;
	vector<float> eachnodedist;
	vector<mat4> objsMat;
	vector<int> objstep;
	vector<vector<vec3>> objsline;
	// Enables the Depth Buffer and choses which depth function to use
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	vector<vec3> objspostion; //攝像機照攝范圍與其物件的scale與旋轉，之後會加上位置參數
	for (int i = 0; i < parametricNode.ParametricRule[0].elementsPos.size(); i++) {
		mat4 modelMatl = modelMat;
		modelMatl = glm::scale(modelMatl, glm::vec3(2, 2, 2));
		modelMatl = glm::rotate(modelMatl, glm::radians((float)(-90)), glm::vec3(1.0f, 0.0f, 0.0f));
		//modelMatl = glm::translate(modelMatl, glm::vec3(parametricNode.ParametricRule[0].elementsPos[sameobj[i][1] - z]));
		rendsame.push_back(modelMatl);
		mat4 mvMat = modelMatl;
		//原本Procedural的生成位置
		vec3 objp = mat4tovec3(mvMat, parametricNode.ParametricRule[0].elementsPos[i]);
		modelMatl = glm::translate(modelMatl, glm::vec3(parametricNode.ParametricRule[0].elementsPos[i]));
		float d1 = dist(campos, objp);
		mvMat = modelMatl;
		//boundingbox
		vec3 bundingmin = mat4tovec3(mvMat, model.objBounding.minbounding);
		float d2 = dist(campos, bundingmin);
		vec3 bundingmax = mat4tovec3(mvMat, model.objBounding.maxbounding);
		float d3 = dist(campos, bundingmax);
		if (d1 < d2 && d1 < d3)
			objspostion.push_back(objp);
		else if (d2 < d1 && d2 < d3)
			objspostion.push_back(bundingmin);
		else
			objspostion.push_back(bundingmax);
		modelMatl = glm::translate(modelMatl, glm::vec3(parametricNode.ParametricRule[0].elementsPos[i]));
		objsMat.push_back(mvMat);
	}


	vector<int> takeobj;
	drawModelShader.Enable();
	drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	drawModelShader.UseLighting(false);
	drawModelShader.DrawTexCoord(false);
	drawModelShader.DrawWireframe(false);
	drawModelShader.SetPMat(pMat);
	drawModelShader.SetUVRotMat(uvRotMat);
	drawModelShader.SetFaceColor(colors[6]);

	vector<int> trytest;
	for (int i = 0; i < parametricNode.ParametricRule[0].elementsPos.size(); i++) {
		if (dist(campos, parametricNode.ParametricRule[0].elementsPos[i]) < d) {
			mat4 temp = glm::scale(objsMat[i], glm::vec3(0.95, 0.955, 0.999));
			glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(temp)));

			drawModelShader.SetNormalMat(normalMat);

			drawModelShader.SetMVMat(vMat * temp);
			trytest.push_back(i);
			model.Render();
		}
	}



	drawModelShader.Disable();
	//float data;
	//glReadPixels(windowWidth / 2, windowHeight / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &data);
	//cout << windowWidth / 2 << ',' << windowHeight / 2 << " , " << data << endl;



	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, linetexture, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);



	//混合或陰影貼圖比較
	vector<vec3> countourlines;
	/*for (int i = 0; i < parametricNode.ParametricRule[0].elementsPos.size(); i++) {
		vector<vec3> steplines,clearlines;
		steplines = model.coutourchangeMat4(objsMat[i]);
		//countourlines.insert(countourlines.end(), steplines.begin(), steplines.end());
		for (int j = 0; j < steplines.size(); j += 2) {
			vector<vec3> sline=CompareDepthLine(steplines[i], steplines[i+1],pMat,mvMat);
			if (sline.size() == 2) {
				countourlines.insert(countourlines.end(), sline.begin(), sline.end());
			}

		}

	}*/
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawModelLineShader.Enable();
	drawModelLineShader.SetPMat(pMat);
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(objsMat[objsMat.size() / 2])));
	drawModelLineShader.SetNormalMat(normalMat);
	drawModelLineShader.SetMVMat(vMat * objsMat[objsMat.size() / 2]);
	drawModelLineShader.SetColor(colors[3]);
	const float* pSource = (const float*)glm::value_ptr(meshWindowCam.GetModelMatrix());
	GLdouble modelMatrix[16];
	for (int i = 0; i < 16; ++i)
		modelMatrix[i] = pSource[i];
	pSource = (const float*)glm::value_ptr(meshWindowCam.GetProjectionMatrix(aspect));
	GLdouble projMatrix[16];
	for (int i = 0; i < 16; ++i)
		projMatrix[i] = pSource[i];
	pSource = (const float*)glm::value_ptr(meshWindowCam.GetViewMatrix());
	GLint viewport[4];
	for (int i = 0; i < 16; ++i)
		viewport[i] = pSource[i];
	for (int i = 0; i < parametricNode.ParametricRule[0].elementsPos.size(); i++) {
		vector<vec3> steplines, clearlines;
		steplines = model.coutourchangeMat4(objsMat[i]);
		for (int j = 0; j < steplines.size(); j += 2) {
			GLdouble x, y, z;
			GLdouble winx, winy, winz, winx1, winy1, winz1;
			float depth; float depth1;

			gluProject(steplines[i][0], steplines[i][1], steplines[i][2], modelMatrix, projMatrix, viewport, &winx, &winy, &winz);
			glReadPixels(winx, winy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
			//gluUnProject(winx, winy, depth, modelMatrix, projMatrix, viewport, &x, &y, &z);

			gluProject(steplines[i + 1][0], steplines[i + 1][1], steplines[i + 1][2], modelMatrix, projMatrix, viewport, &winx1, &winy1, &winz1);
			glReadPixels(winx1, winy1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth1);
			//gluUnProject(winx1, winy1, depth1, modelMatrix, projMatrix, viewport, &x, &y, &z);
			vector<vec3> depline;
			if (depth <= winz && depth1 <= winz1) {
				depline.push_back(steplines[i]); depline.push_back(steplines[i + 1]);
				model.renderObjConnect(depline);
			}
		}

	}

	glUseProgram(0);
}

void UnitRoofSurface() {
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthModelFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	vec3 campos = meshWindowCam.GetWorldEyePosition();
	glm::mat4 mvMat = meshWindowCam.GetViewMatrix() * meshWindowCam.GetModelMatrix();
	glm::mat4 pMat = meshWindowCam.GetProjectionMatrix(aspect);
	glm::mat4 vMat = meshWindowCam.GetViewMatrix();
	glm::mat4 modelMat = meshWindowCam.GetModelMatrix();


	float uvRotateAngle = 0.0;
	float prevUVRotateAngle = 0.0;
	float radian = uvRotateAngle * M_PI / 180.0f;
	glm::mat4 uvRotMat = glm::rotate(radian, glm::vec3(0.0, 0.0, 1.0));
	int stepnum = 10; int j = 0;
	vector<vector<vec3>> stepspos;
	drawModelShader.Enable();
	drawModelShader.SetWireColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	drawModelShader.SetFaceColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	drawModelShader.UseLighting(true);
	drawModelShader.DrawTexCoord(false);
	drawModelShader.DrawWireframe(false);
	drawModelShader.SetPMat(pMat);
	drawModelShader.SetUVRotMat(uvRotMat);
	//drawModelShader.SetFaceColor(colors[6]);

	vec3 step = roofSTest.ParametricRule[1].generateWay.startpos;

	mat4 modelMatl = modelMat;
	modelMatl = glm::translate(modelMatl, step);
	if (writemodel) {
		//modelposfile << "rotate  " << step[0] << "  " << step[1] << "  " << step[2] << "\n";
		modelposfile << "pos  " << step[0] << "  " << step[1] << "  " << step[2] << "\n";
		modelposfile.close();
		writemodel = false;
	}
	drawModelShader.SetMVMat(vMat * modelMatl);
	glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(vMat * modelMatl)));
	drawModelShader.SetNormalMat(normalMat);
	model.Render();

	drawModelShader.Disable();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, colorLineFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	model.coutourchangeMat4(vMat * modelMatl);
	vector<mat4> objsMat;
	step = roofSTest.ParametricRule[1].generateWay.startpos + vec3(roofSTest.ParametricRule[0].generateWay.x * 0, 0, roofSTest.ParametricRule[1].generateWay.z * 0);
	modelMatl = modelMat;
	modelMatl = glm::translate(modelMatl, step);
	objsMat.push_back(modelMatl);

	step = roofSTest.ParametricRule[1].generateWay.startpos + vec3(roofSTest.ParametricRule[0].generateWay.x * 1, 0, roofSTest.ParametricRule[1].generateWay.z * 0);
	modelMatl = modelMat;
	modelMatl = glm::translate(modelMatl, step);
	objsMat.push_back(modelMatl);

	drawModelLineShader.Enable();
	drawModelLineShader.SetPMat(pMat);
	//drawModelLineShader.RoundPixel(3);
	normalMat = glm::transpose(glm::inverse(glm::mat3(vMat)));
	drawModelLineShader.SetNormalMat(normalMat);
	drawModelLineShader.SetMVMat(vMat);
	drawModelLineShader.SetColor(colors[3]);
	vector<vec3> obj, horizontalline;
	horizontalline = model.showHorizontalObject(objsMat[0], 0);
	//obj = horizontalline;
	model.renderObjConnect(horizontalline);
	if (writemodelline) {
		for (int i = 0; i < horizontalline.size(); i++) {
			modellinefile << horizontalline[i][0] << "," << horizontalline[i][1] << "," << horizontalline[i][2] << "\n";
		}
	}
	vector<vec3> verticalline;
	drawModelLineShader.SetColor(colors[4]);
	verticalline = model.showVerticalObject(objsMat[0], 1);// takeobj[j + 1] - takeobj[j]
	obj.insert(obj.end(), verticalline.begin(), verticalline.end());
	model.renderObjConnect(verticalline);
	if (writemodelline) {
		for (int i = 0; i < verticalline.size(); i++) {
			modellinefile << verticalline[i][0] << "," << verticalline[i][1] << "," << verticalline[i][2] << "\n";
		}
		modellinefile.close();
		writemodelline = false;
	}
	drawModelLineShader.Disable();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	blendingShader.Enable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthModelTexture);
	glUniform1i(glGetUniformLocation(depthModelTexture, "texture1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorLineTexture);
	glUniform1i(glGetUniformLocation(colorLineTexture, "texture2"), 1);

	glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	blendingShader.Disable();

	glUseProgram(0);
}
void RenderMeshWindow()
{
	renderHouseTree();
	HouseTest();
	TwDraw();
	glutSwapBuffers();
}

void Render()
{
	glutSetWindow(mainWindow);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
}

void RenderAll()
{

	RenderMeshWindow();

}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Mouse event
void My_Mouse(int button, int state, int x, int y)
{
	//控制畫面上的GUI
	if (!TwEventMouseButtonGLUT(button, state, x, y))
	{
		if (glutGetWindow() == meshWindow)
		{
			meshWindowCam.mouseEvents(button, state, x, y);

		}
	}
	Reshape(windowWidth, windowHeight);
}

//Keyboard event
void My_Keyboard(unsigned char key, int x, int y)
{
	if (!TwEventKeyboardGLUT(key, x, y))
	{
		meshWindowCam.keyEvents(key);
	}
	if (key = 'r' || key == 'R') {
		writemodelline = true;
		writemodel = true;
	}
	//密度DENSITY參數
	if (key == 'b' || key == 'B') {
		cout << "DENSITY" << endl;
		cin >> density;
	}
	if (key == 'g' || key == 'G')
	{
		glutSetWindow(meshWindow);
		GLubyte* data = new GLubyte[windowWidth * windowHeight * 4];
		//glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
		const float zNear = 0.1;
		const float zFar = 90.0;
		vector< GLfloat > depths(windowWidth * windowHeight, 0);
		for (size_t i = 0; i < depths.size(); ++i)
		{
			depths[i] = (2.0 * zNear) / (zFar + zNear - depths[i] * (zFar - zNear));
		}
		glReadPixels(0, 0, windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		stbi_write_png("ScreenShot777.png", windowWidth, windowHeight, 4, data, windowWidth * 4);


	}
	if (key == 't' || key == 'T')
	{
		changeTestLine++;
	}
	if (key = 'a' || key == 'A') {
		rotateAngle[0] += 10;
	}
	if (key = 'd' || key == 'D') {
		rotateAngle[0] -= 10;
	}
	if (key = 'w' || key == 'W') {
		rotateAngle[1] += 10;
	}
	if (key = 's' || key == 'S') {
		rotateAngle[1] -= 10;
	}
	if (key = 'q' || key == 'Q') {
		rotateAngle[2] += 10;
	}
	if (key = 'e' || key == 'E') {
		rotateAngle[2] -= 10;
	}
	if (key = 'c' || key == 'C') {
		if (changeshow == 0)
			changeshow = 1;
		else
			changeshow = 0;
	}

	Reshape(windowWidth, windowHeight);
}


void My_Mouse_Moving(int x, int y) {
	//控制畫面上的GUI
	if (!TwEventMouseMotionGLUT(x, y))
	{
		meshWindowCam.mouseMoveEvent(x, y);

	}
	Reshape(windowWidth, windowHeight);
}

int main(int argc, char* argv[])
{
#ifdef __APPLE__
	//Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif

	glutInitWindowPosition(90, 90);
	glutInitWindowSize(windowHeight, windowHeight);
	mainWindow = glutCreateWindow(ProjectName.c_str()); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif

	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	//glutIdleFunc(RenderAll);
	glutTimerFunc(16, My_Timer, 0);
	glutSetOption(GLUT_RENDERING_CONTEXT, GLUT_USE_CURRENT_CONTEXT);


	meshWindow = glutCreateSubWindow(mainWindow, 0, 0, windowWidth, windowHeight);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutMotionFunc(My_Mouse_Moving);
	glutDisplayFunc(RenderMeshWindow);
	InitOpenGL();
	InitData();
	setupGUI();


	//Print debug information 
	Common::DumpInfo();

	// Enter main event loop.
	glutMainLoop();

	return 0;
}

