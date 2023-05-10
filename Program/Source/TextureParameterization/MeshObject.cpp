#include "MeshObject.h"
#include <Eigen/Sparse>
#include <map>



#define Quad
//#define Harmonic

struct OpenMesh::VertexHandle const OpenMesh::PolyConnectivity::InvalidVertexHandle;

#pragma region MyMesh
/*
struct lineGroup {
	std::vector<MyMesh::EdgeIter> contourGroup;
	MyMesh::Point startpoint;
	MyMesh::Point endpoint;
};*/

MyMesh::MyMesh()
{
	request_vertex_normals();
	request_vertex_status();
	request_face_status();
	request_edge_status();
}

MyMesh::~MyMesh()
{

}

int MyMesh::FindVertex(MyMesh::Point pointToFind)
{
	int idx = -1;
	for (MyMesh::VertexIter v_it = vertices_begin(); v_it != vertices_end(); ++v_it)
	{
		MyMesh::Point p = point(*v_it);
		if (pointToFind == p)
		{
			idx = v_it->idx();
			break;
		}
	}

	return idx;
}

void MyMesh::ClearMesh()
{
	if (!faces_empty())
	{
		for (MyMesh::FaceIter f_it = faces_begin(); f_it != faces_end(); ++f_it)
		{
			delete_face(*f_it, true);
		}

		garbage_collection();
	}
}

#pragma endregion

#pragma region GLMesh

GLMesh::GLMesh()
{

}

GLMesh::~GLMesh()
{

}

bool GLMesh::Init(std::string fileName)
{
	if (LoadModel(fileName))
	{
		LoadToShader();
		for (MyMesh::EIter eit = mesh.edges_begin(); eit != mesh.edges_end(); ++eit) {

			// 檢查邊界。 （如果有邊界，一個半邊將無效）
			// 注意：您必須取消引用邊緣迭代器
			if (!mesh.is_boundary(*eit))
			{
				// if you want vertex handles use:
				auto vh1 = mesh.to_vertex_handle(mesh.halfedge_handle(*eit, 0));
				auto vh2 = mesh.from_vertex_handle(mesh.halfedge_handle(*eit, 0));
				auto thehalfedge = mesh.halfedge_handle(*eit, 0);
				// if you want handles of faces adjacent to the edge use:
				auto fh1 = mesh.face_handle(thehalfedge);
				auto fh2 = mesh.face_handle(mesh.opposite_halfedge_handle(thehalfedge));//auto fh2 = mesh.opposite_face_handle(mesh.halfedge_handle(*eit, 0));

				// if you need normal vectors of those faces use:
				MyMesh::Normal face1Norm = mesh.normal(fh1);
				MyMesh::Normal face2Norm = mesh.normal(fh2);
				face1Norm.data();
				face2Norm.data();
				double datatest = (face1Norm | face2Norm) / (sqrt(face1Norm | face1Norm) * sqrt(face2Norm | face2Norm));
				if (abs((face1Norm | face2Norm) / (sqrt(face1Norm | face1Norm) * sqrt(face2Norm | face2Norm))) < 0.1) {
					edgcontour.push_back(eit);
					//std::cout << "edgcontour：" << datatest << std::endl;
				}

			}
			else {
				edgcontour.push_back(eit);
			}
		}
		return true;
	}
	return false;
}

void GLMesh::Render()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


bool GLMesh::LoadModel(std::string fileName)
{
	OpenMesh::IO::Options ropt;
	if (OpenMesh::IO::read_mesh(mesh, fileName, ropt))
	{
		if (!ropt.check(OpenMesh::IO::Options::VertexNormal) && mesh.has_vertex_normals())
		{
			mesh.request_face_normals();
			mesh.update_normals();
			//刪除normal
			//mesh.release_face_normals();
		}

		return true;
	}

	return false;
}

void GLMesh::LoadToShader()
{
	std::vector<MyMesh::Point> vertices;
	vertices.reserve(mesh.n_vertices());
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		vertices.push_back(mesh.point(*v_it));
		MyMesh::Point p = mesh.point(*v_it);
	}

	std::vector<MyMesh::Normal> normals;
	normals.reserve(mesh.n_vertices());
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		normals.push_back(mesh.normal(*v_it));
	}

	std::vector<unsigned int> indices;
	indices.reserve(mesh.n_faces() * 3);
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			indices.push_back(fv_it->idx());
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GLMesh::LoadTexCoordToShader()
{
	if (mesh.has_vertex_texcoords2D())
	{
		std::vector<MyMesh::TexCoord2D> texCoords;
		for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
		{
			MyMesh::TexCoord2D texCoord = mesh.texcoord2D(*v_it);
			texCoords.push_back(texCoord);
		}

		glBindVertexArray(vao);

		glGenBuffers(1, &vboTexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, vboTexCoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texCoords.size(), &texCoords[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

#pragma endregion

MeshObject::MeshObject()
{

}

MeshObject::~MeshObject()
{
}

bool MeshObject::Init(std::string fileName)
{
	//selectedFace.clear();

	return model.Init(fileName);
}

void buildlinetree() {

}
void MeshObject::MeshContourStructure()//mesh contour進入分析線，形成線群，並算出結構的可能weight
{
	MyMesh mesh;
	mesh = model.mesh;
	//meshEdgeGroup
	walkpast.resize(edgcontour.size());
	//計算有交點的線
	//vector<vector<int>> intersectionline;
	intersectionline.resize(edgcontour.size());
	//vector<vector<int>> edgegs;
	vector<int> groupid; groupid.resize(edgcontour.size());//線群與對應的id
	//std::cout << "edgcontour.size() " << edgcontour.size() << endl;
	allAngleAndSame.resize(edgcontour.size());
	float disTwoPoint = 0.1;
	for (int i = 0; i < edgcontour.size(); i++) {
		MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*edgcontour[i], 1);
		MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
		MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
		MyMesh::Point contour1p = contour1p2 - contour1p1;
		vec3 vcontour1p1 = vec3(contour1p1[0], contour1p1[1], contour1p1[2]);
		vec3 vcontour1p2 = vec3(contour1p2[0], contour1p2[1], contour1p2[2]);
		float disc1 = dist(vcontour1p1, vcontour1p2);
		//算i長度
		float contour1dist = sqrt(pow((contour1p1[0] - contour1p2[0]), 2) + pow((contour1p1[1] - contour1p2[1]), 2) + pow((contour1p1[2] - contour1p2[2]), 2));
		for (int j = i + 1; j < edgcontour.size(); j++) {
			MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*edgcontour[j], 1);
			MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
			MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
			MyMesh::Point contour2p = contour2p2 - contour2p1;
			vec3 vcontour2p1 = vec3(contour2p1[0], contour2p1[1], contour2p1[2]);
			vec3 vcontour2p2 = vec3(contour2p2[0], contour2p2[1], contour2p2[2]);
			float disc2 = dist(vcontour2p1, vcontour2p2);
			//算兩條線有點相同
			bool edgehaspointthesame = false;
			if (contour1p1 == contour2p1 || contour1p1 == contour2p2 || contour1p2 == contour2p1 || contour1p2 == contour2p2)
				edgehaspointthesame = true;
			if (dist(vcontour1p1, vcontour2p1) < disTwoPoint || dist(vcontour1p1, vcontour2p2) < disTwoPoint || dist(vcontour1p2, vcontour2p1) < disTwoPoint || dist(vcontour1p2, vcontour2p2) < disTwoPoint)
				edgehaspointthesame = true;
			//算兩條線角度
			//float angle = (acos(abs(contour1p | contour2p) / (sqrt(contour1p.length()) / sqrt(contour2p.length())))) * 180;
			float angle = abs((contour1p | contour2p) / (disc1 * disc2));

			if (edgehaspointthesame) {//平行放入175-185
				if (angle > 0.98) {
					allAngleAndSame[i].push_back(j);
					allAngleAndSame[j].push_back(i);
				}

			}
			if (edgehaspointthesame) {
				intersectionline[i].push_back(j);
			}
		}
	}

	/// <summary>
	/// 此區為分群可組合
	/// 
	/// </summary>
	for (int i = 0; i < allAngleAndSame.size(); i++) {
		if (walkpast[i] != 1) {
			vector<int> s; s.push_back(i);
			edgegs.push_back(s);
			walkpast[i] = 1;
			if (allAngleAndSame[i].size() > 0) {
				for (int j = 0; j < allAngleAndSame[i].size(); j++) {
					if (walkpast[allAngleAndSame[i][j]] != 1) {
						edgegs[edgegs.size() - 1].push_back(allAngleAndSame[i][j]);
						walkpast[allAngleAndSame[i][j]] = 1;
						//cout <<"edgegs "<< edgegs.size() - 1<< " ,allAngleAndSameID " << i << " ,j " << allAngleAndSame[i][j]  << endl;
						intangleForGroup(allAngleAndSame[i][j]);
						//eachgroup.insert(eachgroup.end(), step.begin(), step.end());
					}

				}
			}

		}
	}
	//只有依水平
	for (int i = 0; i < edgegs.size(); i++) {
		EdgeGroup thisEdgegroup;
		bool input = true;
		for (int j = 0; j < edgegs[i].size(); j++) {
			int stepnum = edgegs[i][j];
			MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*edgcontour[stepnum], 1);
			MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
			MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
			vec3 vcontour2p1 = vec3(contour2p1[0], contour2p1[1], contour2p1[2]);
			vec3 vcontour2p2 = vec3(contour2p2[0], contour2p2[1], contour2p2[2]);
			vec3 originP = vec3(0, 0, 0);
			if (vcontour2p1 == originP && vcontour2p2 == originP)
				input = false;
			float disc2 = dist(vcontour2p1, vcontour2p2);
			thisEdgegroup.edgegroups.push_back(edgcontour[stepnum]);
			thisEdgegroup.edgeline.push_back(disc2);
			if (j == 0) {
				thisEdgegroup.p1 = contour2p1;
				thisEdgegroup.p2 = contour2p2;
			}
			else {
				thisEdgegroup.edgepoints.push_back(contour2p1);
				thisEdgegroup.edgepoints.push_back(contour2p2);
			}
			thisEdgegroup.edgeID.push_back(stepnum);
			//處理boundingbox放入檢測的boundingbox
			CalculationBoundingBox(&thisEdgegroup, contour2p1);
			CalculationBoundingBox(&thisEdgegroup, contour2p2);
			CalculationFarLine(&thisEdgegroup, contour2p1);
			CalculationFarLine(&thisEdgegroup, contour2p2);
			//groupid[j] = meshEdgeGroup.size();
		}
		if (input) {
			int t = 0; t = thisEdgegroup.edgegroups.size();
			meshEdgeGroup.push_back(thisEdgegroup);
		}
	}
	//排列mesh內的edgegrouup
	/*for (int i = 0; i < meshEdgeGroup.size(); i++) {
		std::vector<MyMesh::EdgeIter> step;
		vector<int> sortgroup;
		EdgeGroup edgeg;
		while (sortgroup.size() == meshEdgeGroup[i].edgegroups.size())
		{
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {

						MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
						MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
						MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
						if (sortgroup.size() == 0)
						{
							if (contour1p1 == meshEdgeGroup[i].p1 || contour1p2 == meshEdgeGroup[i].p1)
							{
								sortgroup.push_back(j);
								edgeg.edgegroups.push_back(meshEdgeGroup[i].edgegroups[j]);
								break;
							}
						}
						else {
							bool insort = false;
							for (int z = 0; z < sortgroup.size(); z++)
								if (sortgroup[z] == j)
									insort = true;
							if (insort == false)
							{
								MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*edgeg.edgegroups[edgeg.edgegroups.size() - 1], 1);
								MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
								MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
								MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
								MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
								MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
								if (contour1p1 == contour2p1 || contour1p2 == contour2p1 || contour1p1 == contour2p2 || contour1p2 == contour2p2)
								{
									edgeg.edgegroups.push_back(meshEdgeGroup[i].edgegroups[j]);
									sortgroup.push_back(j);
								}
							}
						}
					}
		}

		meshEdgeGroup[i].edgegroups.clear();
		meshEdgeGroup[i].edgegroups = edgeg.edgegroups;
	}*/

	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		std::vector<MyMesh::EdgeIter> step;

		for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {
			MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
			MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
			MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));

			if (step.size() == 0)
			{
				step.push_back(meshEdgeGroup[i].edgegroups[j]);
			}
			else {
				for (int z = 0; z < step.size(); z++) {
					MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*step[z], 1);
					MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
					MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
					if (contour1p1 == contour2p1 || contour1p2 == contour2p1) {
						step.insert(step.begin() + z, meshEdgeGroup[i].edgegroups[j]); break;
					}
					else if (contour1p1 == contour2p2 || contour1p2 == contour2p2) {
						step.insert(step.begin() + (z + 1), meshEdgeGroup[i].edgegroups[j]); break;
					}
				}
			}
		}
		meshEdgeGroup[i].edgegroups.clear();
		meshEdgeGroup[i].edgegroups = step;
	}

	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		lineTree* linetree = new lineTree;
		linetree->linep1 = meshEdgeGroup[i].p1; linetree->linep2 = meshEdgeGroup[i].p2;
		int nodenum = meshEdgeGroup[i].edgegroups.size();
		cout << "meshEdgeGroup[i]  " << i << ",edgegroups.size  " << nodenum << endl;
		vector<lineTree*> treenode;

		while (nodenum >= 2) {
			//nodenum = 0;
			if (meshEdgeGroup[i].edgegroups.size() == 2)
			{
				MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[0], 1);
				MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
				MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
				MyMesh::HalfedgeHandle contour2 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[1], 1);
				MyMesh::Point contour2p1 = mesh.point(mesh.from_vertex_handle(contour2));
				MyMesh::Point contour2p2 = mesh.point(mesh.to_vertex_handle(contour2));
				lineTree* leftnode = new lineTree; lineTree* rightnode = new lineTree;
				leftnode->linep1 = contour1p1; leftnode->linep2 = contour1p2;
				rightnode->linep1 = contour2p1; rightnode->linep2 = contour2p2;
				linetree->lchild = leftnode; linetree->rchild = rightnode;
				nodenum = 0;
			}
			else if (treenode.size() == 0) {
				for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++)
				{
					MyMesh::HalfedgeHandle contour1 = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
					MyMesh::Point contour1p1 = mesh.point(mesh.from_vertex_handle(contour1));
					MyMesh::Point contour1p2 = mesh.point(mesh.to_vertex_handle(contour1));
					lineTree* node = new lineTree;
					node->linep1 = contour1p1; node->linep2 = contour1p2;
					treenode.push_back(node);

				}
			}
			else if (treenode.size() == 2) {
				linetree->lchild = treenode[0]; linetree->rchild = treenode[1];
				treenode.clear();
				nodenum = 0;
			}
			else {
				vector<lineTree*> stepnodes;
				for (int j = 0; j < treenode.size(); j += 2) {
					if (j + 1 >= treenode.size()) {
						lineTree* node = treenode[j]; //node->lchild = treenode[j];
						stepnodes.push_back(node);
					}
					else {
						lineTree* node = new lineTree; node->lchild = treenode[j]; node->rchild = treenode[j + 1];
						vec3 obj1p1 = vec3(treenode[j]->linep1[0], treenode[j]->linep1[1], treenode[j]->linep1[2]);
						vec3 obj1p2 = vec3(treenode[j]->linep2[0], treenode[j]->linep2[1], treenode[j]->linep2[2]);
						vec3 obj2p1 = vec3(treenode[j + 1]->linep1[0], treenode[j + 1]->linep1[1], treenode[j + 1]->linep1[2]);
						vec3 obj2p2 = vec3(treenode[j + 1]->linep2[0], treenode[j + 1]->linep2[1], treenode[j + 1]->linep2[2]);
						float distance = dist(obj1p1, obj2p1);
						vec3 step1 = obj1p1, step2 = obj2p1;
						if (dist(obj1p1, obj2p2) > distance) {
							step1 = obj1p1; step2 = obj2p2;
							distance = dist(obj1p1, obj2p2);

						}
						if (dist(obj1p2, obj2p1) > distance) {
							step1 = obj1p2; step2 = obj2p1;
							distance = dist(obj1p2, obj2p1);
						}
						if (dist(obj1p2, obj2p2) > distance) {
							step1 = obj1p2; step2 = obj2p2;
							distance = dist(obj1p2, obj2p2);
						}
						node->linep1 = MyMesh::Point(step1[0], step1[1], step1[2]);
						node->linep2 = MyMesh::Point(step2[0], step2[1], step2[2]);
						stepnodes.push_back(node);
					}
				}
				treenode.clear(); treenode = stepnodes;
				nodenum = treenode.size();
			}
		}

		meshEdgeGroup[i].linestree = linetree;
		bool p1b = false, p1c = false; bool p2b = false, p2c = false;
		for (int j = 0; j < boundingline.size(); j++)
		{
			if (meshEdgeGroup[i].p1[0] == boundingline[j][0] && meshEdgeGroup[i].p1[1] == boundingline[j][1] && meshEdgeGroup[i].p1[2] == boundingline[j][2]) {
				p1b = true;
			}
			if (meshEdgeGroup[i].p2[0] == boundingline[j][0] && meshEdgeGroup[i].p2[1] == boundingline[j][1] && meshEdgeGroup[i].p2[2] == boundingline[j][2]) {
				p2b = true;
			}
		}
		for (int j = 0; j < convexline.size(); j++) {
			if (meshEdgeGroup[i].p1[0] == convexline[j][0] && meshEdgeGroup[i].p1[1] == convexline[j][1] && meshEdgeGroup[i].p1[2] == convexline[j][2]) {
				p1c = true;
			}
			if (meshEdgeGroup[i].p2[0] == convexline[j][0] && meshEdgeGroup[i].p2[1] == convexline[j][1] && meshEdgeGroup[i].p2[2] == convexline[j][2]) {
				p2c = true;
			}
		}
		if (p1b == true && p2b == true)
			meshEdgeGroup[i].Levelid == 1;
		else if(p1b==true&&p2c==true)
			meshEdgeGroup[i].Levelid == 1;
		else if(p2b=true&& p1c==true)
			meshEdgeGroup[i].Levelid == 1;
		else if(p1c==true&&p2c==true)
			meshEdgeGroup[i].Levelid == 2;
		else
			meshEdgeGroup[i].Levelid == 3;
	}
	
}

// 求兩個向量的叉積
double cross(const MyMesh::Point& a, const MyMesh::Point& b, const MyMesh::Point& c) {
	return ((b - a) % (c - a)).length();
}

// 求兩點距離的平方
double dist2(const MyMesh::Point& a, const MyMesh::Point& b) {
	double dx = a[0] - b[0];
	double dy = a[1] - b[1];
	double dz = a[2] - b[2];
	return dx * dx + dy * dy + dz * dz;
}

// 判斷三個點是否構成逆時針方向
bool ccw(const MyMesh::Point& a, const MyMesh::Point& b, const MyMesh::Point& c) {
	MyMesh::Point ab = b - a; // 將點轉換為向量
	MyMesh::Point ac = c - a; // 將點轉換為向量
	return (ab % ac).length() > 0;
	//return cross(a, b, c) > 0;
}

// 判斷兩點是否重合
bool eq(const MyMesh::Point& a, const MyMesh::Point& b) {
	MyMesh::Point ab = b - a; // 將點轉換為向量
	return ab.sqrnorm() < 1e-16; // 距離的平方設為向量的長度的平方
	//return dist2(a, b) < 1e-8;
}

// 計算凸包
std::vector<MyMesh::Point> MeshObject::convex_hull(std::vector<MyMesh::Point> points) {
	int n = points.size();
	std::sort(points.begin(), points.end(), [](const MyMesh::Point& a, const MyMesh::Point& b) {
		if (a[0] != b[0]) return a[0] < b[0];
		if (a[1] != b[1]) return a[1] < b[1];
		return a[2] < b[2];
		});

	std::vector<MyMesh::Point> hull;
	for (int i = 0; i < n; i++) {
		while (hull.size() >= 2 && !ccw(hull[hull.size() - 2], hull[hull.size() - 1], points[i]))
			hull.pop_back();
		hull.push_back(points[i]);
	}
	int t = hull.size();
	for (int i = n - 2; i >= 0; i--) {
		while (hull.size() >= t + 1 && !ccw(hull[hull.size() - 2], hull[hull.size() - 1], points[i]))
			hull.pop_back();
		hull.push_back(points[i]);
	}
	hull.pop_back();
	return hull;
}

//計算bounding box 點
std::vector<vec3> MeshObject::boundingbox_point(std::vector<MyMesh::Point> points) {
	vec3 min_pos = vec3(points[0][0], points[0][1], points[0][2]);
	vec3 max_pos = vec3(points[0][0], points[0][1], points[0][2]);
	
	for (const auto& p : points) {
		vec3 po = vec3(p[0], p[1], p[2]);
		min_pos = min(min_pos, po);
		max_pos = max(max_pos, po);
	}
	
	vector<vec3> bbox_lines;
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));

	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));

	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));

	return bbox_lines;
}
std::vector<vec3> MeshObject::boundingbox_point(std::vector<vec3> points) {
	vec3 min_pos = vec3(points[0][0], points[0][1], points[0][2]);
	vec3 max_pos = vec3(points[0][0], points[0][1], points[0][2]);

	for (const auto& p : points) {
		vec3 po = vec3(p[0], p[1], p[2]);
		min_pos = min(min_pos, po);
		max_pos = max(max_pos, po);
	}

	vector<vec3> bbox_lines;
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));

	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));

	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(min_pos.x, max_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, min_pos.y, max_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, min_pos.z));
	bbox_lines.push_back(glm::vec3(max_pos.x, max_pos.y, max_pos.z));

	return bbox_lines;
}
void MeshObject::Render()
{
	glBindVertexArray(model.vao);
	glDrawElements(GL_TRIANGLES, model.mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}

void MeshObject::RenderVertex()
{
	glBindVertexArray(model.vao);
	glLineWidth(5);
	glDrawElements(GL_LINES, model.mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



}
void MeshObject::RenderSegLine() {
	glBindVertexArray(segvao);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::RenderDetailLine() {
	glBindVertexArray(detailvao);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::RenderContours()
{
	glBindVertexArray(contourvao);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::RenderContourGroups()
{
	MyMesh mesh;
	mesh = model.mesh;



	for (int i = 0; i < linegroups.size(); i++) {
		//if (linegroups[i].contourGroup.size() > 2) {
		std::vector<MyMesh::Point> vertices;
		MyMesh::HalfedgeHandle other_hedge = mesh.halfedge_handle(*linegroups[i].contourGroup[0], 1);
		//linegroups[i].startpoint = mesh.point(mesh.from_vertex_handle(other_hedge));
		other_hedge = mesh.halfedge_handle(*linegroups[i].contourGroup[linegroups[i].contourGroup.size() - 1], 1);
		//linegroups[i].endpoint = mesh.point(mesh.to_vertex_handle(other_hedge));
		for (int j = 0; j < linegroups[i].contourGroup.size(); j++)
		{
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*linegroups[i].contourGroup[j], 1);

			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));

			vertices.push_back(curVertex);
			MyMesh::Point middlepoint = curVertex;
			curVertex = mesh.point(mesh.to_vertex_handle(_hedge));
			vertices.push_back(curVertex);
			middlepoint += curVertex;
			// if you want handles of faces adjacent to the edge use:
			MyMesh::FaceHandle  fh1 = mesh.face_handle(_hedge);
			MyMesh::FaceHandle  fh2 = mesh.face_handle(mesh.opposite_halfedge_handle(_hedge));//auto fh2 = mesh.opposite_face_handle(mesh.halfedge_handle(*eit, 0));
			// if you need normal vectors of those faces use:
			MyMesh::Normal face1Norm = mesh.normal(fh1);
			MyMesh::Normal face2Norm = mesh.normal(fh2);

			middlepoint = middlepoint / 2;


		}

		GLuint s;
		glGenVertexArrays(1, &s);
		glBindVertexArray(s);

		glGenBuffers(1, &contourg);
		glBindBuffer(GL_ARRAY_BUFFER, contourg);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		contourgroupvaos.push_back(s);
		vertices.clear();
		//}
	}


}

void MeshObject::RenderSelectedFace()
{
	if (selectedFace.size() > 0)
	{
		std::vector<unsigned int*> offsets(selectedFace.size());
		for (int i = 0; i < offsets.size(); ++i)
		{
			offsets[i] = (GLuint*)(selectedFace[i] * 3 * sizeof(GLuint));
		}

		std::vector<int> count(selectedFace.size(), 3);

		glBindVertexArray(model.vao);
		glMultiDrawElements(GL_TRIANGLES, &count[0], GL_UNSIGNED_INT, (const GLvoid**)&offsets[0], selectedFace.size());
		glBindVertexArray(0);
	}
}

bool MeshObject::AddSelectedFace(unsigned int faceID)
{
	if (std::find(selectedFace.begin(), selectedFace.end(), faceID) == selectedFace.end() &&
		faceID >= 0 && faceID < model.mesh.n_faces())
	{
		selectedFace.push_back(faceID);
		return true;
	}
	return false;
}
void MeshObject::Render_Solid()
{
	MyMesh mesh;
	mesh = model.mesh;
	MyMesh::FaceIter f_it;
	MyMesh::FaceVertexIter	fv_it;
	//glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	glPolygonOffset(2.0, 2.0);
	glBegin(GL_TRIANGLES);
	glColor4f(0.81, 0.74, 0.33, 0.3);

	for (f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{


		for (fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			MyMesh::Point p = mesh.normal(*fv_it);
			glNormal3fv(p.data());
			glVertex3fv(mesh.point(*fv_it).data());
		}
	}
	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
}
void MeshObject::Render_Wireframe()
{

	glLineWidth(5.0);
	MyMesh mesh;
	mesh = model.mesh;
	glColor3f(10.0, 0.0, 0.0);

	glBegin(GL_LINES);
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
	{

		MyMesh::HalfedgeHandle _heh = mesh.halfedge_handle(*e_it, 0);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_heh));
		glVertex3fv(curVertex.data());

		curVertex = mesh.point(mesh.to_vertex_handle(_heh));
		glVertex3fv(curVertex.data());
	}
	glEnd();

}
void MeshObject::ChangePoint(glm::mat4 modelchange) {
	MyMesh mesh;
	mesh = model.mesh;
	for (MyMesh::VIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		MyMesh::Point p = mesh.point(*v_it);
		glm::vec3 poin = glm::vec3(p[0], p[1], p[2]);
		poin = mat4tovec3(modelchange, poin);
		mesh.point(*v_it) = MyMesh::Point(poin[0], poin[1], poin[2]);
	}
}

bool MeshObject::RayTriangleIntersection(vec3 linepos1, vec3 linepos2) {
	MyMesh mesh;
	mesh = model.mesh;

	bool rv = false;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		int i = 0; vec3 V[3];
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			MyMesh::Point p = mesh.point(*fv_it);
			V[i] = vec3(p[0], p[1], p[2]);
		}
		//v1(n1,n2,n3);
		//平面方程: na * (x – n1) + nb * (y – n2) + nc * (z – n3) = 0 ;
		double na = (V[1][1] - V[0][1]) * (V[2][2] - V[0][2]) - (V[2][2] - V[0][2]) * (V[2][1] - V[0][1]);
		double nb = (V[1][2] - V[0][2]) * (V[2][0] - V[0][0]) - (V[1][0] - V[0][0]) * (V[2][2] - V[0][2]);
		double nc = (V[1][0] - V[0][0]) * (V[2][1] - V[0][1]) - (V[1][1] - V[0][1]) * (V[2][0] - V[0][0]);

		//平面法向量
		vec3 nv(na, nb, nc);

		//方向向量
		vec3 direction = linepos2 - linepos1;
		MyMesh::Point directionP = MyMesh::Point(direction[0], direction[1], direction[2]);
		//平面法向量與射線方向向量差積
		MyMesh::Point p1 = MyMesh::Point(nv[0], nv[1], nv[2]);
		double vpt = (directionP | p1);
		if (vpt == 0)
		{
			rv = false;  //此时直线与平面平行
		}
		else {
			//三角面其中一點，與射線原點的
			MyMesh::Point p2 = MyMesh::Point(V[0][0], V[0][1], V[0][2]) - MyMesh::Point(linepos1[0], linepos1[1], linepos1[2]);
			double t = (p2 | p1) / vpt;

			vec3 p3 = linepos1 + vec3(direction[0] * t, direction[1] * t, direction[2] * t);
			float dist1 = dist(linepos1, linepos2);//原本的距離
			float dist2 = dist(linepos1, p3);//相機三角面平面上的距離
			if (dist2 < dist1)
			{
				if (PointinTriangle1(V[0], V[1], V[2], p3))
				{
					rv = true;
					break;
				}
			}

		}

	}

	return rv;

}

void MeshObject::findContours()
{
	edgcontour.clear();
	//基本的contour判斷
	int startedge = 0;
	MyMesh mesh;
	mesh = model.mesh;
	std::vector<MyMesh::Point> steppoint;
	for (MyMesh::VIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		//vector<vector<MyMesh::Point>> changePoint;
		vector<MyMesh::Point> push;
		MyMesh::Point p = mesh.point(*v_it);
		push.push_back(p);
		steppoint.push_back(p);
		changePoint.push_back(push);
	}
	std::vector<MyMesh::Point> convexpoint = convex_hull(steppoint);
	boundingline=boundingbox_point(steppoint);
	for (MyMesh::EIter eit = mesh.edges_begin(); eit != mesh.edges_end(); ++eit) {

		// 檢查邊界。 （如果有邊界，一個半邊將無效）
		// 注意：您必須取消引用邊緣迭代器
		if (!mesh.is_boundary(*eit))
		{
			// if you want vertex handles use:
			auto vh1 = mesh.to_vertex_handle(mesh.halfedge_handle(*eit, 0));
			auto vh2 = mesh.from_vertex_handle(mesh.halfedge_handle(*eit, 0));
			auto thehalfedge = mesh.halfedge_handle(*eit, 0);
			// if you want handles of faces adjacent to the edge use:
			auto fh1 = mesh.face_handle(thehalfedge);
			auto fh2 = mesh.face_handle(mesh.opposite_halfedge_handle(thehalfedge));
			// if you need normal vectors of those faces use:
			MyMesh::Normal face1Norm = mesh.normal(fh1);
			MyMesh::Normal face2Norm = mesh.normal(fh2);
			face1Norm.data();
			face2Norm.data();
			double datatest = (face1Norm | face2Norm) / (sqrt(face1Norm | face1Norm) * sqrt(face2Norm | face2Norm));
			if (abs((face1Norm | face2Norm) / (sqrt(face1Norm | face1Norm) * sqrt(face2Norm | face2Norm))) < contourthreshold) {
				//刪除相同線段
				bool isinit = false;
				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*eit, 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				MyMesh::Point curVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				MyMesh::Point  middlepoint = (curVertex + curVertex1) / 2;
				for (int z = 0; z < edgcontour.size(); z++)
				{
					MyMesh::HalfedgeHandle other_hedge = mesh.halfedge_handle(*edgcontour[z], 1);
					MyMesh::Point otherVertex = mesh.point(mesh.from_vertex_handle(other_hedge));
					MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(other_hedge));
					if ((curVertex == otherVertex && curVertex1 == otherVertex1) || (curVertex == otherVertex1 && curVertex1 == otherVertex))
						isinit = true;
				}
				if (isinit == false) {
					edgcontour.push_back(eit);
				}
			}
		}
		else {

			edgcontour.push_back(eit);
		}
	}
	//vector<MyMesh::EdgeIter> Construct(edgcontour);
	// 從凸包的頂點開始，尋找邊界上的線段	
	for (int i = 0; i < edgcontour.size(); i++)
	{
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point curVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		vec3 o = vec3(curVertex1[0], curVertex1[1], curVertex1[2]);
		vec3 o1 = vec3(curVertex[0], curVertex[1], curVertex[2]);
		bool line1b = false, line2b = false;
		for (int j = 0; j < convexpoint.size(); j++) {
			if (curVertex == convexpoint[j]) line1b = true;
			if (curVertex1 == convexpoint[j]) line2b = true;
			if (line1b && line2b) break;
		}
		if (line1b && line2b) {
			convexline.push_back(o);
			convexline.push_back(o1);
		}
	}

}

int MeshObject::go_lineTree(MeshObject::lineTree &showline ,int level,vector<vec3> &lines) {
	
}

void MeshObject::contourGroup() {
	MyMesh mesh;
	mesh = model.mesh;
	std::vector<bool> isgothough;
	for (int i = 0; i < edgcontour.size(); i++) {
		isgothough.push_back(false);
	}
	for (int i = 0; i < edgcontour.size(); i++)
	{
		MyMesh::Point CircleRegularity;
		if (isgothough[i] == false) {
			isgothough[i] = true;
			lineGroup lineg;
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			MyMesh::Point curVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
			MyMesh::Point cur = curVertex1 - curVertex;
			vec3 o = vec3(curVertex1[0], curVertex1[1], curVertex1[2]);
			vec3 o1 = vec3(curVertex[0], curVertex[1], curVertex[2]);
			float dispos = dist(o, o1);
			lineg.contourGroup.push_back(edgcontour[i]);
			//與其他的線段做角度測試
			int step = 0;
			for (int j = i + 1; j < edgcontour.size(); j++) {
				if (isgothough[j] == false) {
					MyMesh::HalfedgeHandle other_hedge = mesh.halfedge_handle(*edgcontour[j], 1);
					MyMesh::Point otherVertex = mesh.point(mesh.from_vertex_handle(other_hedge));
					MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(other_hedge));
					MyMesh::Point other = otherVertex1 - otherVertex;
					vec3 o1 = vec3(otherVertex[0], otherVertex[1], otherVertex[2]);
					vec3 o11 = vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
					float disother = dist(o1, o11);
					MyMesh::Point s, s1;
					bool connect = false;
					if (curVertex == otherVertex)
					{
						connect = true;
						s = curVertex1;
						s1 = otherVertex1;
						cur = otherVertex - otherVertex1;

					}
					else if (curVertex == otherVertex1) {
						connect = true;
						s = curVertex1;
						s1 = otherVertex;

					}
					else if (curVertex1 == otherVertex) {
						connect = true;
						s = curVertex;
						s1 = otherVertex1;

					}
					else if (curVertex1 == otherVertex1) {
						connect = true;
						s = curVertex;
						s1 = otherVertex;

					}
					if (connect)
					{
						//std::cout << i << "abs(cur | other) /abs(sqrt(cur|cur))/abs(sqrt(other|other))" <<acos( abs(cur | other) / (abs(cur.length()*other.length())) ) * 180 / 3.14 << std::endl;
						//(abs((gendirection | other)/(dispos*disother)) > 0.997)
						if (abs((cur | other) / (dispos * disother)) > 0.997) {
							lineg.contourGroup.push_back(edgcontour[j]);
							if (step == 0) {
								lineg.startpoint = s;
								lineg.endpoint = s1;
							}
							else
								lineg.endpoint = s1;
							curVertex = otherVertex;
							curVertex1 = otherVertex1;
							//cur = curVertex1 - curVertex;
							step++;
							isgothough[j] == true;
						}
						else
							break;
					}
					else
						break;
				}
			}
			int s = lineg.contourGroup.size();
			linegroups.push_back(lineg);
			lineg.contourGroup.clear();
			i += step;
		}
	}

}
void MeshObject::lineSegment(MyMesh::Point gendirection)
{
	MyMesh mesh;
	mesh = model.mesh;

	std::vector<MyMesh::Point> vertices;
	std::vector<MyMesh::Normal> normals;
	std::vector<MyMesh::Point> detailvertices;
	std::vector<MyMesh::Normal> detailnormals;
	vector<MyMesh::EdgeIter> steptext;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		if (meshEdgeGroup[i].boundingth > 0) {
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {
				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);

				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				MyMesh::Point otherVertex = mesh.normal(mesh.from_vertex_handle(_hedge));
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				MyMesh::Point other = otherVertex1 - otherVertex;

				if (abs(acos(abs(gendirection | other))) > 0.1) {
					//meshEdgeGroup[i].parallelth ++;
				}
				else {
					meshEdgeGroup[i].parallelth++;
					meshEdgeGroup[i].structlines = true;
				}
			}
			meshEdgeGroup[i].structth = meshEdgeGroup[i].parallelth + meshEdgeGroup[i].boundingth;

		}
		else {
			meshEdgeGroup[i].parallelth = 0;
		}
	}


}

//與生成的線段，比較角度，此時分為垂直與水平線
//之後需要加入不同的曲線偵測
void MeshObject::lineSegment(MyMesh::Point gendirection, float dispos)
{
	MyMesh mesh;
	mesh = model.mesh;

	std::vector<MyMesh::Point> vertices;
	std::vector<MyMesh::Normal> normals;
	std::vector<MyMesh::Point> detailvertices;
	std::vector<MyMesh::Normal> detailnormals;
	vector<MyMesh::EdgeIter> steptext;


	vector<int> orginDelet;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {

		vec3 mesh1p1 = vec3(meshEdgeGroup[i].p1[0], meshEdgeGroup[i].p1[1], meshEdgeGroup[i].p1[2]);
		vec3 mesh1p2 = vec3(meshEdgeGroup[i].p2[0], meshEdgeGroup[i].p2[1], meshEdgeGroup[i].p2[2]);
		MyMesh::Point curVertex, otherV;
		curVertex = meshEdgeGroup[i].p1; otherV = meshEdgeGroup[i].p2;
		if (meshEdgeGroup[i].edgegroups.size() == 1) {
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
			curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			mesh1p1 = vec3(curVertex[0], curVertex[1], curVertex[2]);
			otherV = mesh.point(mesh.to_vertex_handle(_hedge));
			mesh1p2 = vec3(otherV[0], otherV[1], otherV[2]);
		}

		MyMesh::Point contour1p = curVertex - otherV;
		float disc1 = dist(mesh1p1, mesh1p2);
		float angle = abs((gendirection | contour1p) / (disc1 * dispos));
		vec3 originP = vec3(0, 0, 0);

		if (angle > 0.8) {
			meshEdgeGroup[i].structlines = true; meshhorizontalnum++;
		}
		else if (angle < 0.15) {
			meshEdgeGroup[i].verticallines = true; meshverticalnum++;
		}

	}



}
void MeshObject::AnalysisOfSame(mat4 pos1, mat4 pos2, MeshObject obj) {

	MeshObject obj1 = obj;
	obj1.ChangePoint(pos1);
	MeshObject obj2 = obj;
	obj2.ChangePoint(pos2);

	//依序號判斷彼此是否可以合並
	//距離小於d為可合併
	float d = 0.1;
	MyMesh mesh;
	mesh = obj1.model.mesh;
	MyMesh mesh2 = obj2.model.mesh;
	//vector<int> mergeid;
	vector<vector<int>> dis;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		int canmerge = 0;

		vec3 mesh1p1 = vec3(meshEdgeGroup[i].p1[0], meshEdgeGroup[i].p1[1], meshEdgeGroup[i].p1[2]);
		vec3 mesh1p2 = vec3(meshEdgeGroup[i].p2[0], meshEdgeGroup[i].p2[1], meshEdgeGroup[i].p2[2]);
		//mesh1p1 = mat4tovec3(pos1, mesh1p1);
		//mesh1p2 = mat4tovec3(pos1, mesh1p2);
		MyMesh::Point contour1p = meshEdgeGroup[i].p1 - meshEdgeGroup[i].p2;
		float disc1 = dist(mesh1p1, mesh1p2);
		//int j = i;
		for (int j = i; j < meshEdgeGroup.size(); j++) {
			vec3 meshjp1 = vec3(meshEdgeGroup[j].p1[0], meshEdgeGroup[j].p1[1], meshEdgeGroup[j].p1[2]);
			vec3 meshjp2 = vec3(meshEdgeGroup[j].p2[0], meshEdgeGroup[j].p2[1], meshEdgeGroup[j].p2[2]);
			float disc2 = dist(meshjp1, meshjp2);
			//meshjp1 = mat4tovec3(pos2, meshjp1);
			//meshjp2 = mat4tovec3(pos2, meshjp2);
			//每個點進行距離判斷，若距離小於d為可合併，以遠方線為準
			float objp1objp3 = abs(dist(mesh1p1, meshjp1));
			float objp1objp4 = abs(dist(mesh1p1, meshjp2));
			float objp2objp3 = abs(dist(mesh1p2, meshjp1));
			float objp2objp4 = abs(dist(mesh1p2, meshjp2));
			MyMesh::Point contour2p = meshEdgeGroup[j].p1 - meshEdgeGroup[j].p2;
			//float angle = acos(abs(contour1p | contour2p) / (abs(contour1p.length() * contour2p.length()))) * 180 / 3.14;
			float angle = abs((contour1p | contour2p) / (disc1 * disc2));
			float dischose = objp1objp3;
			if (objp1objp3 < d || objp1objp4 < d || objp2objp3 < d || objp2objp4 < d)
			{
				if (angle > 0.997) {
					meshEdgeGroup[i].MergeLine.push_back(&meshEdgeGroup[j]);
					meshEdgeGroup[i].MergeLineth++;
					meshEdgeGroup[j].MergeLine.push_back(&meshEdgeGroup[i]);
					meshEdgeGroup[j].MergeLineth++;
				}
			}
		}

	}


}

void MeshObject::renderEachObj(mat4 postions)
{
	MyMesh mesh;
	mesh = model.mesh;
	vector<MyMesh::Point> vertices;
}

vector<vec3> MeshObject::returnObjLine(mat4 postions) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<vec3> objline;
	for (int i = 0; i < edgcontour.size(); i++)
	{
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);

		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
		obj1V1 = mat4tovec3(postions, obj1V1);
		objline.push_back(obj1V1);
		curVertex = mesh.point(mesh.to_vertex_handle(_hedge));
		obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
		obj1V1 = mat4tovec3(postions, obj1V1);
		objline.push_back(obj1V1);
	}
	return objline;
}
//簡化Procedural contour線段
void MeshObject::proceduralContourLine(vector<vector<vec3>> objsline) {
	vector<vec3> lines;
	for (int i = 0; i < objsline.size() - 1; i++)
	{
		for (int j = 0; j < objsline[i].size(); j += 2) {
			vector<vec3> points;
			points.push_back(objsline[i][j]);
			points.push_back(objsline[i][j + 1]);
			points.push_back(objsline[i + 1][j + 1]);
			points.push_back(objsline[i + 1][j]);
			vector<vec3> interse = Intersection(points, 5);
			lines.insert(lines.end(), interse.begin(), interse.end());
		}
	}
	vector<MyMesh::Point> vertices;
	for (int i = 0; i < lines.size(); i++) {
		vertices.push_back(MyMesh::Point(lines[i][0], lines[i][1], lines[i][2]));
	}
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
//相交，輸入相交線段及范圍參數，輸出兩線段
vector<vec3> MeshObject::Intersection(vector<vec3> points, int range) {
	vector<vec3> lines;
	MyMesh::Point p11 = MyMesh::Point(points[0][0], points[0][1], points[0][2]);
	vec3 l1 = points[1] - points[0];
	MyMesh::Point line1 = MyMesh::Point(l1[0], l1[1], l1[2]);
	vec3 l2 = points[3] - points[2];
	MyMesh::Point line2 = MyMesh::Point(l2[0], l2[1], l2[2]);
	//算相交點	
	MyMesh::Point startPointSeg = line2 - line1;
	MyMesh::Point vecS1 = (line1 % line2);// 有向面積1
	MyMesh::Point vecS2 = (startPointSeg % line2);// 有向面積2
	float num = (startPointSeg | vecS1);
	float num2 = (vecS2 | vecS1) / vecS1.length();
	MyMesh::Point intersectP = p11 + line1 * num2;
	vec3 intersectPv = vec3(intersectP[0], intersectP[1], intersectP[2]);
	//相交在范圍內	
	//第一條線
	float orgin = dist(points[0], points[1]);
	lines.push_back(points[0]);
	lines.push_back(points[1]);
	lines.push_back(points[2]);
	lines.push_back(points[3]);
	/*if (dist(points[0], intersectPv) < orgin && dist(intersectPv, points[1]) < orgin) {
		//在線上，比三個點的情況若交點比原來兩點的距離短
		lines.push_back(points[0]);
		lines.push_back(points[1]);
	}
	else if (dist(points[0], intersectPv) < orgin+range && dist(intersectPv, points[1]) < orgin+range) {
		//在圍內
		if (dist(points[0], intersectPv) > dist(intersectPv, points[1]))
		{
			lines.push_back(points[0]);
			lines.push_back(intersectPv);
		}
		else {
			lines.push_back(intersectPv);
			lines.push_back(points[1]);
		}
	}
	else {
		//無相交點
		lines.push_back(points[0]);
		lines.push_back(points[1]);
	}
	//第二條線
	orgin = dist(points[2], points[3]);
	if (dist(points[2], intersectPv) < orgin && dist(intersectPv, points[3]) < orgin) {
		//在線上，比三個點的情況若交點比原來兩點的距離短
		lines.push_back(points[2]);
		lines.push_back(points[3]);
	}
	else if (dist(points[2], intersectPv) < orgin + range && dist(intersectPv, points[3]) < orgin + range) {
		//在圍內
		if (dist(points[2], intersectPv) > dist(intersectPv, points[3]))
		{
			lines.push_back(points[2]);
			lines.push_back(intersectPv);
		}
		else {
			lines.push_back(intersectPv);
			lines.push_back(points[3]);
		}
	}
	else {
		//無相交點
		lines.push_back(points[2]);
		lines.push_back(points[3]);
	}*/
	return lines;
}
//vector<int> connectid;
void MeshObject::renderObjConnect(vector<vec3> lines) {
	vector<MyMesh::Point> vertices;

	for (int i = 0; i < lines.size(); i++) {
		vertices.push_back(MyMesh::Point(lines[i][0], lines[i][1], lines[i][2]));
	}
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(3);
	glColor3f(1.0, 0.0, 0.0);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, lines.size());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::renderOneMeshEdgeGroupSL(int i) {
	vector<MyMesh::Point> vertices;

	vertices.push_back(meshEdgeGroup[i].p1);
	vertices.push_back(meshEdgeGroup[i].p2);
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_PATCHES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::renderOneMeshEdgeGroup(int i) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<MyMesh::Point> vertices;
	//if (meshEdgeGroup[i].verticallines) {
	for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {

		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		vertices.push_back(curVertex);
		vertices.push_back(otherVertex1);
	}
	//}
	//vertices.push_back(meshEdgeGroup[i].p1);
	//vertices.push_back(meshEdgeGroup[i].p2);   
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices[0].size(), &vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
void MeshObject::renderMeshEdgeGroup(mat4 pos1) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<int> connectid;
	vector<vec3> lines;
	vector<MyMesh::Point> vertices;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		//if (meshEdgeGroup[i].structlines) {
		for (int z = 0; z < meshEdgeGroup[i].edgegroups.size(); z++) {
			for (int j = 0; j < edgcontour.size(); j++) {
				if (meshEdgeGroup[i].edgegroups[z] == edgcontour[j])
					connectid.push_back(j);
			}
		}
		//}
	}
	for (int i = 0; i < connectid.size(); i++) {
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[connectid[i]], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
		MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
		//vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
		//vertices.push_back(MyMesh::Point(obj2V1[0], obj2V1[1], obj2V1[2]));
		vertices.push_back(curVertex); vertices.push_back(otherVertex1);
	}
	/*for (int i = 0; i < verticalEdge.size(); i++) {
		vertices.push_back(verticalEdge[i].startpoint); vertices.push_back(verticalEdge[i].endpoint);
	}
	for (int i = 0; i < horizontalEdge.size(); i++) {
		vertices.push_back(horizontalEdge[i].startpoint); vertices.push_back(horizontalEdge[i].endpoint);
	}*/
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}
vector<vec3> MeshObject::connectTwoObject(mat4 pos1, mat4 pos2) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<int> connectid;
	vector<int> meshChangeID, lineChangeID, pointChangID;
	vector<vec3> changeVec3;
	vector<vec3> lines;
	vector<int> isitGroup; isitGroup.resize(edgcontour.size());
	vector<MyMesh::Point> vertices;
	for (int i = 0; i < edgcontour.size(); i++) {
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		vec3 obj1V1, obj2V1;
		obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
		obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);

		//vec3 line11 = mat4tovec3(pos1, obj1V1);
		//vec3 line12 = mat4tovec3(pos1, obj2V1);
		vec3 line21 = mat4tovec3(pos2, obj1V1);
		vec3 line22 = mat4tovec3(pos2, obj2V1);
		//lines.push_back(line11);
		//lines.push_back(line12);
		lines.push_back(line21);
		lines.push_back(line22);
		//vertices.push_back(MyMesh::Point(line11[0], line11[1], line11[2]));
		//vertices.push_back(MyMesh::Point(line12[0], line12[1], line12[2]));
		vertices.push_back(MyMesh::Point(line21[0], line21[1], line21[2]));
		vertices.push_back(MyMesh::Point(line22[0], line22[1], line22[2]));
	}



	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
	/*for (int i = 0; i < meshEdgeGroup.size(); i++) {
		for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {
			isitGroup[meshEdgeGroup[i].edgeID[j]]+=1;
			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
			vec3 obj1V1, obj2V1;
			obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
			obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);

			vec3 line11 = mat4tovec3(pos1, obj1V1);
			vec3 line12 = mat4tovec3(pos1, obj2V1);
			vec3 line21 = mat4tovec3(pos2, obj1V1);
			vec3 line22 = mat4tovec3(pos2, obj2V1);
			lines.push_back(line11);
			lines.push_back(line12);
			lines.push_back(line21);
			lines.push_back(line22);
		}
		/*if (meshEdgeGroup[i].structlines)
		{
			connectid.push_back(i);

			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {
				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);


				vec3 line11 = mat4tovec3(pos1, obj1V1);
				vec3 line12 = mat4tovec3(pos1, obj2V1);
				vec3 line21 = mat4tovec3(pos2, obj1V1);
				vec3 line22 = mat4tovec3(pos2, obj2V1);
				float dis = 9999; vec3 r1, r2;
				r1 = line11; r2 = line12;
				dis = dist(line11, line12);
				if (dist(line11, line21) > dis)
				{
					r1 = line11; r2 = line21;
					dis = dist(line11, line21);
				}
				if (dist(line11, line22) > dis) {
					r1 = line11; r2 = line22;
					dis = dist(line11, line22);
				}
				if (dist(line12, line21) > dis)
				{
					r1 = line12; r2 = line21;
					dis = dist(line12, line21);
				}
				if (dist(line12, line22) > dis) {
					r1 = line12; r2 = line22;
					dis = dist(line12, line22);
				}

				lines.push_back(r1); lines.push_back(r2);


			}
		}
		else {
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {

				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj1V1, obj2V1;
				obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);

				vec3 line11 = mat4tovec3(pos1, obj1V1);
				vec3 line12 = mat4tovec3(pos1, obj2V1);
				vec3 line21 = mat4tovec3(pos2, obj1V1);
				vec3 line22 = mat4tovec3(pos2, obj2V1);
				lines.push_back(line11);
				lines.push_back(line12);
				lines.push_back(line21);
				lines.push_back(line22);
			}
		}*/
		//}

	return lines;
}


//水平線選取
vector<vec3> MeshObject::connectTwoObject(mat4 pos1, mat4 pos2, mat4 pos3, int choose)
{
	vector<vec3> lines;
	MyMesh mesh;
	mesh = model.mesh;
	if (choose == 0) {
		for (int i = 0; i < meshEdgeGroup.size(); i++) {

			if (meshEdgeGroup[i].structlines)
			{


				for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {



					MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
					MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
					vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
					MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
					vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);

					vec3 line11 = mat4tovec3(pos1, obj1V1);
					vec3 line12 = mat4tovec3(pos1, obj2V1);
					vec3 line21 = mat4tovec3(pos2, obj1V1);
					vec3 line22 = mat4tovec3(pos2, obj2V1);
					vec3 line31 = mat4tovec3(pos3, obj1V1);
					vec3 line32 = mat4tovec3(pos3, obj2V1);

					lines.push_back(line11);
					lines.push_back(line31);


				}
			}
		}
	}
	return lines;
}
vector<vec3> MeshObject::showHorizontalObject(mat4 pos1, int size) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<vec3> lines;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		if (meshEdgeGroup[i].structlines == true) {
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {
				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);

				vec3 line11 = mat4tovec3(pos1, obj1V1);
				vec3 line12 = mat4tovec3(pos1, obj2V1);

				lines.push_back(line11);
				lines.push_back(line12);
			}
		}
	}
	return lines;
}
vector<vec3> MeshObject::showVerticalObject(mat4 pos1, int size) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<vec3> lines;
	for (int i = 0; i < meshEdgeGroup.size(); i++) {

		if (meshEdgeGroup[i].structlines != true) {
			vec3 line11, line12, line21, line22; bool linkline = false;
			int allsize = meshEdgeGroup[i].edgegroups.size() / 2;
			int step = size / 2;
			if (size / 2 >= allsize)
				step = allsize;
			if (step == 0) step = 1;
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j += step) {

				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
				vec3 oline11 = mat4tovec3(pos1, obj1V1);
				vec3 oline12 = mat4tovec3(pos1, obj2V1);

				lines.push_back(oline11);
				lines.push_back(oline12);

			}
		}
	}
	return lines;
}

//垂直線選取
vector<vec3> MeshObject::connectTwoObject(mat4 pos1, mat4 pos2, int size) {
	MyMesh mesh;
	mesh = model.mesh;
	vector<int> connectid;
	vector<int> meshChangeID, lineChangeID, pointChangID;
	vector<vec3> changeVec3;
	vector<nearIntersection> nearID;
	vector<vec3> lines;
	vector<int> isitGroup; isitGroup.resize(edgcontour.size());

	for (int i = 0; i < changePoint.size(); i++) {
		if (changePoint[i].size() > 1)
			changePoint[i].erase(changePoint[i].begin() + 1, changePoint[i].end());
	}
	for (int i = 0; i < meshEdgeGroup.size(); i++) {

		
		if (meshEdgeGroup[i].verticallines == true) {
			vec3 line11, line12, line21, line22; bool linkline = false;
			int allsize = meshEdgeGroup[i].edgegroups.size() / 2;
			int step = size / 2;
			if (size / 2 >= allsize)
				step = allsize;
			if (step == 0) step = 1;
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j += step) {

				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
				vec3 oline11 = mat4tovec3(pos1, obj1V1);
				vec3 oline12 = mat4tovec3(pos1, obj2V1);
				vec3 oline21 = mat4tovec3(pos2, obj1V1);
				vec3 oline22 = mat4tovec3(pos2, obj2V1);

				if (j == 0) {
					line11 = mat4tovec3(pos1, obj1V1);
					line12 = mat4tovec3(pos1, obj2V1);
					line21 = mat4tovec3(pos2, obj1V1);
					line22 = mat4tovec3(pos2, obj2V1);
				}
				else {

					float dis = 9999; vec3 r1, r2;
					r1 = line11; r2 = line12;
					dis = dist(line11, line12);
					if (dist(oline11, line12) > dis)
					{
						r1 = oline11; r2 = line12;
						dis = dist(oline11, line12);
					}
					if (dist(line11, oline12) > dis) {
						r1 = line11; r2 = oline12;
						dis = dist(line11, oline12);
					}
					if (dist(oline11, oline12) > dis)
					{
						r1 = oline11; r2 = oline12;
						dis = dist(oline11, oline12);
					}
					lines.push_back(r1); lines.push_back(r2);
					dis = 9999; vec3 r3, r4;
					r3 = line21; r4 = line22;
					dis = dist(line21, line22);
					if (dist(oline21, line22) > dis)
					{
						r3 = oline21; r4 = line22;
						dis = dist(oline21, line22);
					}
					if (dist(line21, oline22) > dis) {
						r3 = line21; r4 = oline22;
						dis = dist(line21, oline22);
					}
					if (dist(oline21, oline22) > dis)
					{
						r3 = oline21; r4 = oline22;
						dis = dist(oline21, oline22);
					}
					lines.push_back(r3); lines.push_back(r4);
					line11 = oline11; line12 = oline12; line21 = oline21; line22 = oline22;
					linkline = false;
				}
				if (j + step >= meshEdgeGroup[i].edgegroups.size())
				{
					MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[meshEdgeGroup[i].edgegroups.size() - 1], 1);
					MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
					vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
					MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
					vec3 obj2V1 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
					vec3 eline11 = mat4tovec3(pos1, obj1V1);
					vec3 eline12 = mat4tovec3(pos1, obj2V1);
					vec3 eline21 = mat4tovec3(pos2, obj1V1);
					vec3 eline22 = mat4tovec3(pos2, obj2V1);
					float dis = 9999; vec3 r1, r2;
					r1 = oline11; r2 = oline12;
					dis = dist(oline11, oline12);
					if (dist(eline11, oline12) > dis)
					{
						r1 = eline11; r2 = oline12;
						dis = dist(eline11, oline12);
					}
					if (dist(oline11, eline12) > dis) {
						r1 = oline11; r2 = eline12;
						dis = dist(oline11, eline12);
					}
					if (dist(eline11, eline12) > dis)
					{
						r1 = eline11; r2 = eline12;
						dis = dist(eline11, eline12);
					}
					lines.push_back(r1); lines.push_back(r2);
					dis = 9999; vec3 r3, r4;
					r3 = oline21; r4 = oline22;
					dis = dist(oline21, oline22);
					if (dist(eline21, oline22) > dis)
					{
						r3 = eline21; r4 = oline22;
						dis = dist(eline21, oline22);
					}
					if (dist(oline21, eline22) > dis) {
						r3 = oline21; r4 = eline22;
						dis = dist(oline21, eline22);
					}
					if (dist(eline21, eline22) > dis)
					{
						r3 = eline21; r4 = eline22;
						dis = dist(eline21, eline22);
					}
					lines.push_back(r3); lines.push_back(r4);
					linkline = false;
					break;
				}

			}
		}
	}

	return lines;
}
void MeshObject::renderFarSameLine(mat4 pos1, mat4 pos2, vector<vector<int>>  sameObjectMerge, mat4 farnum, int nearfar, vec3 campos)
{
	MyMesh mesh;
	mesh = model.mesh;
	vector<MyMesh::Point> vertices;
	vector<int> groupgoing; groupgoing.resize(meshEdgeGroup.size());//走遍id
	//分辮結構線與裝飾線，並記錄權重
	//計算幾何	
	for (int i = 0; i < meshEdgeGroup.size(); i++) {
		//int i = 2;{

			//若是structth代表p1、p2方向與生成方式大相同
		if (meshEdgeGroup[i].structlines) {
			//繪制垂直線要前後兩條
			bool istwin = false;

			if (abs(abs(meshEdgeGroup[i].p2[0]) - abs(meshEdgeGroup[i].p1[0])) < 0.09 && abs(abs(meshEdgeGroup[i].p2[2]) - abs(meshEdgeGroup[i].p1[2])) < 0.09)
				istwin = true;

			MyMesh::Point line1 = meshEdgeGroup[i].p2 - meshEdgeGroup[i].p1;
			//先確認結構線的情況
			if (istwin == false) {
				if (meshEdgeGroup[i].MergeLineth > 0) {

					for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {

						MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
						MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
						MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
						vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
						obj1V1 = mat4tovec3(pos1, obj1V1);

						vec3 obj1V2 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
						obj1V2 = mat4tovec3(pos1, obj1V2);
						//vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
						//vertices.push_back(MyMesh::Point(obj1V2[0], obj1V2[1], obj1V2[2]));
						vec3 obj2V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
						obj2V1 = mat4tovec3(pos2, obj2V1);

						vec3 obj2V2 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
						obj2V2 = mat4tovec3(pos2, obj2V2);
						if (abs(dist(obj1V1, obj2V2)) > abs(dist(obj1V2, obj2V1)))
						{
							vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
							vertices.push_back(MyMesh::Point(obj2V2[0], obj2V2[1], obj2V2[2]));
						}
						else {
							vertices.push_back(MyMesh::Point(obj1V2[0], obj1V2[1], obj1V2[2]));
							vertices.push_back(MyMesh::Point(obj2V1[0], obj2V1[1], obj2V1[2]));
						}
						/*vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
						vertices.push_back(MyMesh::Point(obj1V2[0], obj1V2[1], obj1V2[2]));

						vertices.push_back(MyMesh::Point(obj2V1[0], obj2V1[1], obj2V1[2]));
						vertices.push_back(MyMesh::Point(obj2V2[0], obj2V2[1], obj2V2[2]));	*/
					}
				}
			}
		}
		//繪製裝飾線
		else {
			//前後
			for (int j = 0; j < meshEdgeGroup[i].edgegroups.size(); j++) {

				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*meshEdgeGroup[i].edgegroups[j], 1);
				MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				vec3 obj1V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				obj1V1 = mat4tovec3(pos1, obj1V1);

				vec3 obj1V2 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
				obj1V2 = mat4tovec3(pos1, obj1V2);
				//vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
				//vertices.push_back(MyMesh::Point(obj1V2[0], obj1V2[1], obj1V2[2]));
				vec3 obj2V1 = glm::vec3(curVertex[0], curVertex[1], curVertex[2]);
				obj2V1 = mat4tovec3(pos2, obj2V1);

				vec3 obj2V2 = glm::vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
				obj2V2 = mat4tovec3(pos2, obj2V2);

				vertices.push_back(MyMesh::Point(obj1V1[0], obj1V1[1], obj1V1[2]));
				vertices.push_back(MyMesh::Point(obj1V2[0], obj1V2[1], obj1V2[2]));

				vertices.push_back(MyMesh::Point(obj2V1[0], obj2V1[1], obj2V1[2]));
				vertices.push_back(MyMesh::Point(obj2V2[0], obj2V2[1], obj2V2[2]));
			}
			//中間
		}
	}



	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, model.mesh.n_edges());// model.mesh.n_edges()
	glBindVertexArray(0);
}

void MeshObject::ObjectBounding()
{
	MyMesh mesh;
	mesh = model.mesh;
	for (int i = 0; i < edgcontour.size(); i++)
	{
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);

		MyMesh::Point p = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point curVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		if (p[0] < objBounding.minx)
			objBounding.minx = p[0];
		if (p[0] > objBounding.manx)
			objBounding.manx = p[0];
		if (p[1] < objBounding.miny)
			objBounding.miny = p[1];
		if (p[1] > objBounding.many)
			objBounding.many = p[1];
		if (p[2] < objBounding.minz)
			objBounding.minz = p[2];
		if (p[2] > objBounding.manz)
			objBounding.manz = p[2];

		if (curVertex1[0] < objBounding.minx)
			objBounding.minx = curVertex1[0];
		if (curVertex1[0] > objBounding.manx)
			objBounding.manx = curVertex1[0];
		if (curVertex1[1] < objBounding.miny)
			objBounding.miny = curVertex1[1];
		if (curVertex1[1] > objBounding.many)
			objBounding.many = curVertex1[1];
		if (curVertex1[2] < objBounding.minz)
			objBounding.minx = curVertex1[2];
		if (curVertex1[2] > objBounding.manz)
			objBounding.minx = curVertex1[2];
	}
	objBounding.minbounding = vec3(objBounding.minx, objBounding.miny, objBounding.minz);
	objBounding.maxbounding = vec3(objBounding.manx, objBounding.many, objBounding.manz);
	float volume = (objBounding.manx - objBounding.minx) * (objBounding.many - objBounding.miny) * (objBounding.manz - objBounding.minz);
	objBounding.volume = volume;
}

vector<MyMesh::EdgeIter> MeshObject::Construct(std::vector<MyMesh::EdgeIter> edges)
{
	MyMesh mesh;
	int n = edges.size();
	vector<MyMesh::EdgeIter> ans;
	for (int i = 0; i < edges.size(); i++) {

		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edges[i], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex = mesh.normal(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		ply.push_back(vec3(curVertex[0], curVertex[1], curVertex[2]));
		ply.push_back(vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]));
	}
	double PR = 1e-8;


	bool tmp = true;
	fac add;
	for (int i = 0; i < 4; i++) //構建初始四面體
	{
		add.a = (i + 1) % 4; add.b = (i + 2) % 4; add.c = (i + 3) % 4; add.ok = true;
		if ((ptoplane(ply[i], add)) > 0)
		{
			width = ptoplane(ply[i], add);
			int swap = add.b;
			add.b = add.c;
			add.c = swap;
		}

		vis[add.a][add.b] = vis[add.b][add.c] = vis[add.c][add.a] = trianglecnt;
		tri[trianglecnt++] = add;
	}
	for (int i = 4; i < n; i++) //構建更新凸包
	{
		for (int j = 0; j < trianglecnt; j++)
		{
			if (tri[j].ok && (ptoplane(ply[i], tri[j])) > PR)
			{
				dfs(i, j);
				width = ptoplane(ply[i], tri[j]);
				break;
			}
		}
	}
	int cnt = trianglecnt;
	trianglecnt = 0;
	for (int i = 0; i < cnt; i++)
	{
		if (tri[i].ok)
		{
			tri[trianglecnt++] = tri[i];
		}
	}
	//返回convex edge

	for (int i = 0; i < edges.size(); i++) {

		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edges[i], 1);
		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex = mesh.normal(mesh.from_vertex_handle(_hedge));
		MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
		vec3 p1 = vec3(curVertex[0], curVertex[1], curVertex[2]);
		vec3 p2 = vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]);
		bool p1has, p2has = false;
		for (int j = 0; j < trianglecnt; j++) {
			if (p1 == ply[j])
				p1has = true;
			else if (p2 == ply[j])
				p2has = true;
		}
		if (p1has && p2has) {			
			ans.push_back(edges[i]);
		}
	}
	return ans;
	//return vector<MyMesh::EdgeIter>();
}

void MeshObject::contourShader()
{
	MyMesh mesh;
	mesh = model.mesh;
	std::vector<MyMesh::Point> vertices;
	std::vector<MyMesh::Normal> normals;

	for (int i = 0; i < edgcontour.size(); i++)
	{
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edgcontour[i], 1);

		MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
		vertices.push_back(curVertex);
		normals.push_back(mesh.normal(mesh.from_vertex_handle(_hedge)));
		curVertex = mesh.point(mesh.to_vertex_handle(_hedge));
		vertices.push_back(curVertex);
		normals.push_back(mesh.normal(mesh.to_vertex_handle(_hedge)));
	}
	glGenVertexArrays(1, &contourvao);
	glBindVertexArray(contourvao);

	glGenBuffers(1, &contourV);
	glBindBuffer(GL_ARRAY_BUFFER, contourV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
bool line_parallel(MyMesh::Point line1P1, const MyMesh::Point line1P2, const MyMesh::Point line2P1, const MyMesh::Point line2P2) {
	MyMesh::Point v1 = (line1P2 - line1P1).normalize();
	MyMesh::Point v2 = (line2P2 - line2P1).normalize();
	int dotwo = (v1 | v2);
	if (abs(v1 | v2) == 1)
	{
		//兩線平行
		return true;
	}

	else
		return false;
}

void MeshObject::contourRule()
{
	/*MyMesh mesh;
	mesh = model.mesh;
	std::vector<MyMesh::EdgeIter> stepEdge;
	std::vector<MyMesh::EdgeIter> deletEdge;//被刪除的邊，若邊界的contour與刪除的邊平行就刪除
	stepEdge = edgcontour;
	edgcontour.clear();
	//判斷平行線
	for (int i = 0; i < stepEdge.size(); i++) {
		bool inisparallelinvectorstep = false;
		MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(stepEdge[i].handle(), 0);

		MyMesh::Point vh1 = mesh.point(mesh.from_vertex_handle(_hedge));

		MyMesh::Point vh2 = mesh.point(mesh.to_vertex_handle(_hedge));
		double dantance0 = sqrt(pow(vh2[0] - vh1[0], 2) + pow(vh2[1] - vh1[1], 2) + pow(vh2[2] - vh1[2], 2));
		for (int j = 0; j < deletEdge.size(); j++) {
			MyMesh::HalfedgeHandle _hedge2 = mesh.halfedge_handle(deletEdge[j].handle(), 0);

			MyMesh::Point vh21 = mesh.point(mesh.from_vertex_handle(_hedge2));

			MyMesh::Point vh22 = mesh.point(mesh.to_vertex_handle(_hedge2));
			double dantance1 = sqrt(pow(vh22[0] - vh21[0], 2) + pow(vh22[1] - vh21[1], 2) + pow(vh22[2] - vh21[2], 2));

			if (dantance0 == dantance1)
			{
				if (line_parallel(vh1, vh2, vh21, vh22)) {
					inisparallelinvectorstep = true;
					break;
				}
			}
		}
		if (inisparallelinvectorstep == false)
			edgcontour.push_back(stepEdge[i]);
	}*/
}



void MeshObject::RenderParameterized()
{
	if (model.mesh.has_vertex_texcoords2D())
	{
		glBindVertexArray(model.vao);
		glMultiDrawElements(GL_TRIANGLES, &elemCount[0], GL_UNSIGNED_INT, (const GLvoid**)&fvIDsPtr[0], elemCount.size());
		glBindVertexArray(0);
	}
}


