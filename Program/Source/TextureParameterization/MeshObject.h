#pragma once

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <Common.h>
#include "Convex.h"
#include <fstream>
typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMesh;

class MyMesh : public TriMesh
{
public:
	MyMesh();
	~MyMesh();

	int FindVertex(MyMesh::Point pointToFind);
	void ClearMesh();
};

class GLMesh
{
public:
	GLMesh();
	~GLMesh();

	bool Init(std::string fileName);
	void Render();
	void LoadTexCoordToShader();

	MyMesh mesh;
	GLuint vao;
	GLuint ebo;
	GLuint vboVertices, vboNormal, vboTexCoord;
	std::vector<MyMesh::EdgeIter> edgcontour;
private:

	bool LoadModel(std::string fileName);
	void LoadToShader();
};

class MeshObject
{
	struct lineTree {
		int levelid;
		MyMesh::Point linep1, linep2;
		lineTree* lchild = NULL; lineTree* rchild = NULL;
	};
	struct lineGroup {
		std::vector<MyMesh::EdgeIter> contourGroup;
		vector<vec3> convexlines;
		vector<vec3> boundinglines;
		MyMesh::Point startpoint;
		MyMesh::Point endpoint;
		lineTree linetree;
	};
	struct fac
	{
		int a, b, c;//凸包一個面上的三個點的編號
		bool ok;//該面是否是最終凸包中的面  
	};
	struct BoundingBox {
		float minx = 999;
		float manx = -999;
		float miny = 999;
		float many = -999;
		float minz = 999;
		float manz = -999;
		vec3 minbounding, maxbounding;
		float volume = (manx - minx) * (many - miny) * (manz - minz);//體積

	};
	struct  nearIntersection {
		int meshID;
		int edgeID, edgeS;
		MyMesh::Point edgep1, edgep2;
		vec3 p1, p2;
	};
	struct EdgeGroup {
		int Levelid;
		lineTree* linestree;
		//edge
		std::vector<MyMesh::EdgeIter> edgegroups;
		//vertex(point)
		std::vector<MyMesh::Point> edgepoints;
		//角度
		std::vector<float> angle;

		//長度
		std::vector<float> edgeline;
		//intersection交點
		std::vector<EdgeGroup*> edgeintersection;
		//是否可合併
		bool merge = false;
		//圓形=0、規則曲線=1、不規則曲線=2、直線=3
		int style = 4;
		float parallelth = 0;
		float boundingth = 0;//bounding 權重
		//結構線權重
		float structth = parallelth + boundingth;
		//結構與否
		bool structlines = false;
		bool verticallines = false;
		//每個線段的id
		vector<int> edgeID;

		//物件本身
		std::vector<EdgeGroup*> MergeOwnLine;
		//跨物件
		std::vector<EdgeGroup*> MergeLine;
		//暫存點
		std::vector<MyMesh::Point> renderFarP;
		int MergeLineth = 0;
		//遠處顯示線段
		MyMesh::Point p1 = MyMesh::Point(0, 0, 0); MyMesh::Point p2 = MyMesh::Point(0, 0, 0);
		BoundingBox boundingbox;
	};
	//EdgeGroup群結構線、裝飾線排序
	bool StructComplare(EdgeGroup edgegroup1, EdgeGroup edgegroup2) {
		return edgegroup1.structth > edgegroup2.structth;
	}
public:
	MeshObject();
	~MeshObject();
	//contour line draw
	GLuint contourvao, segvao, segV;
	GLuint detailvao, detailV;
	GLuint drawSameObj, drawSameObjV;
	std::vector<GLuint> contourgroupvaos;
	GLuint contourg;
	GLuint contourV, contourN, contourE;
	//contour
	float contourthreshold = 0.5;
	std::vector<MyMesh::EdgeIter> edgcontour;
	std::vector<vec3> convexline;
	std::vector<vec3> boundingline;
	//edgecontour角度
	vector<vector<int>> allAngleAndSame;
	std::vector<int> walkpast;
	vector<vector<int>> edgegs;
	std::vector<MyMesh::EdgeIter> structureNearEdgcontour;
	std::vector<MyMesh::EdgeIter> structureFarEdgcontour;
	std::vector<MyMesh::EdgeIter> detailEdgcontour;
	std::vector<EdgeGroup> meshEdgeGroup;
	std::vector<EdgeGroup> verticalEdgeGroup;
	vector<lineGroup> verticalEdge;
	vector<lineGroup> horizontalEdge;
	vector<vector<MyMesh::Point>> changePoint;
	std::vector<MyMesh::EdgeIter> edgcontournormal;
	std::vector<MyMesh::Point> meshvertices;
	vector<int> mergeid;
	bool Init(std::string fileName);
	void MeshContourStructure();
	void ComputeConvexHull(const std::vector<MyMesh::Point>& points, std::vector<MyMesh::Point>& hull_verts);
	std::vector<MyMesh::Point> convex_hull(std::vector<MyMesh::Point> points);
	std::vector<vec3> boundingbox_point(std::vector<MyMesh::Point> points);//回傳bounding_line
	std::vector<vec3> boundingbox_point(std::vector<vec3> points);
	void Render();
	void RenderSelectedFace();
	bool AddSelectedFace(unsigned int faceID);
	void Render_Solid();
	void Render_Wireframe();
	void ChangePoint(glm::mat4 modelchange);
	bool RayTriangleIntersection(vec3 linepos1, vec3 linepos2);
	void RenderParameterized();
	void RenderVertex();
	void RenderSegLine();
	void RenderDetailLine();

	void RenderContours();
	void RenderContourGroups();
	void findContours();
	int go_lineTree(MeshObject::lineTree& showline, int level, vector<vec3>& lines);
	void contourShader();
	void contourRule();
	void contourGroup();
	void lineSegment(MyMesh::Point gendirection);
	//vector<MyMesh::EdgeIter> ans;
	bool proximity;
	bool similarity;
	void createElements();
	void lineSegment(mat4 Matm);
	void immediateRender(vec3 campos);
	void lineSegment(MyMesh::Point gendirection, float dispos);
	//vector<vector<int>> AnalysisOfSame(MeshObject obj1, MeshObject obj2,vec3 pos1,vec3 pos2);
	void AnalysisOfSame(mat4 pos1, mat4 pos2, MeshObject obj);
	void renderEachObj(mat4 postions);
	vector<vec3> returnObjLine(mat4 postions);
	void proceduralContourLine(vector<vector<vec3>> objsline);
	vector<vec3> Intersection(vector<vec3> points, int range);
	void renderObjConnect(vector<vec3> lines);
	void renderOneMeshEdgeGroupSL(int i);
	void renderOneMeshEdgeGroup(int i);
	void renderMeshEdgeGroup(mat4 pos1);
	vector<vec3>  connectTwoObject(mat4 pos1, mat4 pos2);
	vector<vec3> connectTwoObject(mat4 pos1, mat4 pos2, mat4 pos3, int choose);
	vector<vec3> showHorizontalObject(mat4 pos1, int size);
	vector<vec3> showVerticalObject(mat4 pos1, int size);

	vector<vec3> connectTwoObject(mat4 pos1, mat4 pos2, int size);
	void renderFarSameLine(mat4 pos1, mat4 pos2, vector<vector<int>>  sameObjectMerge, mat4 farnum, int nearfar, vec3 campos);
	void ObjectBounding();
	int meshverticalnum, meshhorizontalnum;


	//計算有交點的線
	vector<vector<int>> intersectionline;
	double PR = 1e-8;
	int n = 0;//初始點数  
	vector<vec3> ply;//初始點  
	int trianglecnt;//凸包上三角形數  
	fac* tri;//凸包三角形  
	int** vis;//點i到點j是屬於哪個面
	vec3 scareObj;//物件輸入的大小
	double width;
	//vector<MyMesh::EdgeIter> ans;
	BoundingBox objBounding;

	//算boundingbox
	void CalculationBoundingBox(EdgeGroup* edgegroup, MyMesh::Point check) {
		if (check[0] < edgegroup->boundingbox.minx)
			edgegroup->boundingbox.minx = check[0];
		if (check[0] > edgegroup->boundingbox.manx)
			edgegroup->boundingbox.manx = check[0];
		if (check[1] < edgegroup->boundingbox.miny)
			edgegroup->boundingbox.miny = check[1];
		if (check[1] > edgegroup->boundingbox.many)
			edgegroup->boundingbox.many = check[1];
		if (check[2] < edgegroup->boundingbox.minz)
			edgegroup->boundingbox.minx = check[2];
		if (check[2] > edgegroup->boundingbox.manz)
			edgegroup->boundingbox.minx = check[2];
	}
	//int算分群
	void intangleForGroup(int allAngleAndSameID) {
		//vector<int> egroup;
		if (allAngleAndSame[allAngleAndSameID].size() != 0) {
			for (int j = 0; j < allAngleAndSame[allAngleAndSameID].size(); j++) {
				if (walkpast[allAngleAndSame[allAngleAndSameID][j]] != 1) {

					edgegs[edgegs.size() - 1].push_back(allAngleAndSame[allAngleAndSameID][j]);
					walkpast[allAngleAndSame[allAngleAndSameID][j]] = 1;
					intangleForGroup(allAngleAndSame[allAngleAndSameID][j]);
					//cout <<"edgegs " << edgegs.size() - 1 << " ,allAngleAndSameID " << allAngleAndSameID << " ,j " << allAngleAndSame[allAngleAndSameID][j] << endl;
				}
			}
		}
		//cout << allAngleAndSameID << "  ," << egroup.size() << endl;
		//return egroup;
	}

	//算分群
	void angleForGroup(MyMesh::EdgeIter edge1, EdgeGroup* edgegroup, vector<int> groupid, int allAngleAndSameID) {
		MyMesh mesh;
		mesh = model.mesh;
		MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*edge1, 1);
		MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
		MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
		MyMesh::Point contour1p = contour1p2 - contour1p1;

		for (int j = 0; j < allAngleAndSame[allAngleAndSameID].size(); j++) {
			MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*edgcontour[j], 1);
			MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
			MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
			MyMesh::Point contour2p = contour2p2 - contour2p1;
			float contour2dist = sqrt(pow((contour2p1[0] - contour2p2[0]), 2) + pow((contour2p1[1] - contour2p2[1]), 2) + pow((contour2p1[2] - contour2p2[2]), 2));

			if (allAngleAndSame[allAngleAndSameID][j] > 275 && allAngleAndSame[allAngleAndSameID][j] < 280 || allAngleAndSame[allAngleAndSameID][j] == 0) {

			}
			else if (walkpast[j] != 1) {

				edgegroup->angle.push_back(allAngleAndSame[allAngleAndSameID][j]);
				edgegroup->edgegroups.push_back(edgcontour[j]);
				edgegroup->edgeline.push_back(contour2dist);
				edgegroup->edgepoints.push_back(contour2p1);
				edgegroup->edgepoints.push_back(contour2p2);
				//處理boundingbox放入檢測的boundingbox
				CalculationBoundingBox(edgegroup, contour2p1);
				CalculationBoundingBox(edgegroup, contour2p2);
				CalculationFarLine(edgegroup, contour2p1);
				CalculationFarLine(edgegroup, contour2p2);
				groupid[j] = meshEdgeGroup.size();
				walkpast[j] = 1;
				angleForGroup(edgcontour[j], edgegroup, groupid, allAngleAndSameID);

			}
		}
	}

	//countour轉換回傳
	vector<vec3> MeshObject::coutourchangeMat4(mat4 pos1) {
		MyMesh mesh;
		mesh = model.mesh;
		vector<vec3> lines;

		for (int i = 0; i < edgcontour.size(); i++)
		{
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
			MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
			vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
			vec3 oline11 = mat4tovec3(pos1, obj1V1);
			vec3 oline12 = mat4tovec3(pos1, obj2V1);
			lines.push_back(oline11); lines.push_back(oline12);
		}



		return lines;
	}

	//boundingline轉換回傳
	vector<vec3>MeshObject::boundinglinchangeMat4(mat4 pos1) {
		MyMesh mesh;
		mesh = model.mesh;
		vector<vec3> lines;

		for (int i = 0; i < boundingline.size(); i++)
		{
			vec3 oline11 = mat4tovec3(pos1, boundingline[i]);
			lines.push_back(oline11);
		}

		return lines;
	}

	//boundingline轉換回傳
	vector<vec3>MeshObject::convexlinechangeMat4(mat4 pos1) {
		MyMesh mesh;
		mesh = model.mesh;
		vector<vec3> lines;

		for (int i = 0; i < convexline.size(); i++)
		{
			vec3 oline11 = mat4tovec3(pos1, convexline[i]);
			lines.push_back(oline11);
		}

		return lines;
	}

	//三角形面積
	double volume(vec3 a, vec3 b, vec3 c, vec3 d)
	{
		vec3 e = (b - a);
		vec3 f = (c - a);
		vec3 g = (d - a);
		vec3 h = Multiply(e, f);

		return dot(h, g);
	}
	//四面體有向體積*6 
	vec3 Multiply(vec3 a, vec3 b)
	{
		vec3 step = vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		return step;
	}
	double ptoplane(glm::vec3 p, fac f) {
		glm::vec3 m = ply[f.b] - ply[f.a], n = ply[f.c] - ply[f.a], t = ply[f.a] - ply[f.a];
		double tmp = (double)dot(Multiply(m, n), t);
		return tmp;
	};
	void deal(int p, int a, int b)
	{
		int f = vis[a][b];
		fac add;
		if (tri[f].ok)
		{
			if ((ptoplane(ply[p], tri[f])) > PR)
			{
				width = ptoplane(ply[p], tri[f]);
				dfs(p, f);
			}
			else
			{
				add.a = b; add.b = a; add.c = p; add.ok = true;
				vis[p][b] = vis[a][p] = vis[b][a] = trianglecnt;
				tri[trianglecnt++] = add;
			}
		}
	}
	void dfs(int p, int cnt)//维護凸包，如果點p在凸包外更新凸包  
	{
		tri[cnt].ok = false;
		deal(p, tri[cnt].b, tri[cnt].a);
		deal(p, tri[cnt].c, tri[cnt].b);
		deal(p, tri[cnt].a, tri[cnt].c);
	}
	vector<MyMesh::EdgeIter> Construct(std::vector<MyMesh::EdgeIter> edges);
	// Utility functions for square and cube, to go along with sqrt and cbrt
	template <class T>
	static inline T sqr(const T& x)
	{
		return x * x;
	}
	float dist(vec3& v1, vec3& v2)
	{
		return sqrt(dist2(v1, v2));
	}
	float dist2(vec3& v1, vec3& v2)
	{
		float d2 = sqr(v2[0] - v1[0]);
		for (int i = 1; i < 3; i++)
			d2 += sqr(v2[i] - v1[i]);
		return d2;
	}
	//算遠方長度
	void CalculationFarLine(EdgeGroup* edgegroup, MyMesh::Point check) {
		vec3 objp1 = vec3(edgegroup->p1[0], edgegroup->p1[1], edgegroup->p1[2]);
		vec3 objp2 = vec3(edgegroup->p2[0], edgegroup->p2[1], edgegroup->p2[2]);
		vec3 checkp = vec3(check[0], check[1], check[2]);
		float distance = dist(objp1, objp2);
		vec3 step1 = objp1, step2 = objp2, step = vec3(0, 0, 0);

		if (dist(objp1, checkp) > distance) {
			step1 = objp1; step2 = checkp;
			distance = dist(step1, checkp);

		}
		if (dist(checkp, objp2) > distance) {
			step1 = checkp; step2 = objp2;
			distance = dist(checkp, objp2);
		}
		edgegroup->p1 = MyMesh::Point(step1[0], step1[1], step1[2]);
		edgegroup->p2 = MyMesh::Point(step2[0], step2[1], step2[2]);
	}
	vec3 mat4tovec3(glm::mat4 xf, glm::vec3 v) {
		float* pSource = glm::value_ptr(xf);
		float h = pSource[3] * v.x + pSource[7] * v.y + pSource[11] * v.z + pSource[15];
		h = 1 / h;

		return glm::vec3(float((pSource[0] * v.x + pSource[4] * v.y + pSource[8] * v.z + pSource[12])),
			float((pSource[1] * v.x + pSource[5] * v.y + pSource[9] * v.z + pSource[13])),
			float((pSource[2] * v.x + pSource[6] * v.y + pSource[10] * v.z + pSource[14])));
	}
	void SaveTreeNode(lineTree* tree, mat4 changep, ofstream& file) {
		if (tree == NULL) {
			file << "null" << "\n";
			return;
		}
		vec3 obj1V1 = mat4tovec3(changep, glm::vec3(tree->linep1[0], tree->linep1[1], tree->linep1[2]));
		vec3 obj1V2 = mat4tovec3(changep, glm::vec3(tree->linep2[0], tree->linep2[1], tree->linep2[2]));
		file << obj1V1[0] << "," << obj1V1[1] << "," << obj1V1[2] << "," << obj1V2[0] << "," << obj1V2[1] << "," << obj1V2[2] << "\n";
		SaveTreeNode(tree->lchild, changep, file);
		SaveTreeNode(tree->rchild, changep, file);
	}
	// v1 = Cross(AB, AC)
	// v2 = Cross(AB, AP)
	// 判断矢量v1和v2是否同向
	bool SameSide(vec3 A, vec3 B, vec3 C, vec3 P)
	{
		vec3 AB = B - A; MyMesh::Point ab = MyMesh::Point(AB[0], AB[1], AB[2]);
		vec3 AC = C - A; MyMesh::Point ac = MyMesh::Point(AC[0], AC[1], AC[2]);
		vec3 AP = P - A; MyMesh::Point ap = MyMesh::Point(AP[0], AP[1], AP[2]);
		//彼此的外積cross 
		MyMesh::Point v1 = (ab % ac);
		MyMesh::Point v2 = ab % ap;

		// v1 and v2 should point to the same direction
		//return v1.Dot(v2) >= 0 ;
		return (v1 | v2) > 0;
	}

	// 判斷點P是否在三角形ABC内(同向法)
	bool PointinTriangle1(vec3 A, vec3 B, vec3 C, vec3 P) {
		return SameSide(A, B, C, P) && SameSide(B, C, A, P) && SameSide(C, A, B, P);
	}
	// 判斷點P是否在三角形ABC内(同向法)
	bool PointinTriangle1(MyMesh::Point Aa, MyMesh::Point Bb, MyMesh::Point Cc, MyMesh::Point Pp) {
		vec3 A = vec3(Aa[0], Aa[1], Aa[2]); vec3 B = vec3(Bb[0], Bb[1], Bb[2]);
		vec3 C = vec3(Cc[0], Cc[1], Cc[2]); vec3 P = vec3(Pp[0], Pp[1], Pp[2]);
		return SameSide(A, B, C, P) && SameSide(B, C, A, P) && SameSide(C, A, B, P);
	}
	vector<vec3> GetContour() {
		MyMesh mesh;
		mesh = model.mesh;
		vector<vec3> lines;
		for (int i = 0; i < edgcontour.size(); i++) {
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
			MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
			vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
			lines.push_back(obj1V1);
			lines.push_back(obj2V1);
		}
		return lines;
	}

private:
	GLMesh model;
	std::vector<unsigned int> selectedFace;
	std::vector<unsigned int*> fvIDsPtr;
	std::vector<lineGroup> linegroups;
	std::vector<int> elemCount;



};

