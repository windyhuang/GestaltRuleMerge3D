#include <math.h>
#include "../../Include/GLM/glm/glm.hpp"
#include "MeshObject.h"
using namespace std;
using namespace glm;
struct fac
	{
		int a, b, c;//凸包一個面上的三個點的編號
		bool ok;//該面是否是最終凸包中的面  
	};
class Convex {
	
public:
	double PR = 1e-8;
	int n = 0;//初始點数  
	vector<vec3> ply ;//初始點  
	int trianglecnt;//凸包上三角形數  
	fac* tri;//凸包三角形  
	int** vis;//點i到點j是屬於哪個面
	double width;
	//vector<MyMesh::EdgeIter> ans;
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
				vis[p][ b] = vis[a][ p] = vis[b][ a] = trianglecnt;
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
	/*void construct(std::vector<MyMesh::EdgeIter> edges) {
		MyMesh mesh;
		int n = edges.size();
		ans.clear();
		for (int i = 0; i < edges.size(); i++) {

			MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*edges[i], 1);
			MyMesh::Point curVertex = mesh.point(mesh.from_vertex_handle(_hedge));
			MyMesh::Point otherVertex = mesh.normal(mesh.from_vertex_handle(_hedge));
			MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
			ply.push_back(vec3(curVertex[0], curVertex[1], curVertex[2]));
			ply.push_back(vec3(otherVertex1[0], otherVertex1[1], otherVertex1[2]));
		}
		double PR = 1e-8;
		
		if (n < 4)
		{
			return;
		}
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

			vis[add.a][ add.b] = vis[add.b][ add.c] = vis[add.c][ add.a] = trianglecnt;
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
				else if(p2 == ply[j])
					p2has = true;
			}
			if (p1has && p2has)
				ans.push_back(edges[i]);
		}
		
	};*/
};