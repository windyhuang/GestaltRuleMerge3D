#include "ParametricNode.h"

void ParametricNode::createElements()
{
	vec3 step = generateWay.startpos;
	for (int i = 0; i < generateWay.creatnum; i++) {
		vec3 pos = step + vec3(generateWay.x, generateWay.y, generateWay.z);
		
		elementsPos.push_back(pos);
		generateDirection.push_back(step - pos);
		step = pos;
	}
}
void ParametricNode::createElements(vec3 start)
{
	vec3 step = generateWay.startpos;
	for (int i = 0; i < generateWay.creatnum; i++) {
		vec3 pos = step + vec3(generateWay.x, generateWay.y, generateWay.z);

		elementsPos.push_back(pos);
		generateDirection.push_back(step - pos);
		step = pos;
	}
}
void ParametricNode::lineSegment()
{
	vec3 step = vec3(0, 0, 0);
	//�ͦ���V�Pgeometry contour���u�q���������Y
	for (int i = 0; i < generateDirection.size(); i++)
	{

		step += generateDirection[i];
	}
	step =vec3( step[0]/ generateDirection.size(), step[1] / generateDirection.size(), step[2] / generateDirection.size() );
	normalize(step);
	MyMesh::Point ss = MyMesh::Point(step[0],step[1],step[2]);
	for (int i = 0; i < differentElements.size(); i++) {
		differentElements[i].lineSegment(ss);
	}
	/*if (differentElements.size() == 1)
	{		
		
		for (int i = 0; i < differentElements[0].edgcontour.size();i++) {

			MyMesh mesh;
			if (!mesh.is_boundary(*differentElements[0].edgcontour[i]))
			{
				//MyMesh::Point s = MyMesh::Point(differentElements[0].edgcontour[i]);
				MyMesh::HalfedgeHandle _hedge = mesh.halfedge_handle(*differentElements[0].edgcontour[i], 0);
				MyMesh::Point otherVertex = mesh.point(mesh.from_vertex_handle(_hedge));
				MyMesh::Point otherVertex1 = mesh.point(mesh.to_vertex_handle(_hedge));
				MyMesh::Point other = otherVertex1 - otherVertex;
				std::cout << i << " acos(abs(ss | other)" << acos(abs(ss | other)) << std::endl;
			}
		}
	}*/
}

void ParametricNode::immediateRender(vec3 campos)
{
	//�Y�Oproximity�A���O�b�U�����P�_�A�b����إ߮ɦA����k�Y��Render�X�A�b�o���I�O�ͦ����A���]�O���i�@�B���R����¦�C
	/*if (proximity) {
		int camfar = 0;
		for (int i = 0; i < elementsPos.size(); i++) {
			float startToCamera = dist(campos, elementsPos[i]);
			if (i == 0 || i == elementsPos.size() - 1) {

			}
			else {

			}
		}
	}*/
}
//�ۦP����i��²��
void ParametricNode::sameObjectBuild(mat4 postions)
{
	//�Q�Ϋe��Ӫ���ӧP�_�O�_�i�H�۱�
	//�O�d�쥻������
	MeshObject original = differentElements[0];
	//����e��Ӫ��󪺪����J���ܪ����I
	mat4 model1pos= translate(postions, elementsPos[0]);
	MeshObject model1= differentElements[0];
	model1.ChangePoint(model1pos);
	mat4 model2pos = translate(postions, elementsPos[4]);
	MeshObject model2 = differentElements[0];
	model2.ChangePoint(model2pos);

	//�Y�i�H�۱��A�O���u�q�b���謰�@���u�q
	//sameObjectMerge=differentElements[0].AnalysisOfSame(model1, model2, elementsPos[0], elementsPos[4]);
	differentElements[0].AnalysisOfSame(model1pos, model2pos, differentElements[0]);
	sameObjectMerge;

}

void ParametricNode::renderEachObj(mat4 postions)
{
	

}

void ParametricNode::renderFarSameLine(mat4 pos1, mat4 pos2,mat4 farnum,int nearfar,vec3 campos)
{
	
	MeshObject model1 = differentElements[0];
	MeshObject model2 = differentElements[0];
	
	
	model1.ChangePoint(pos1);
	model2.ChangePoint(pos2);
	differentElements[0].renderFarSameLine(pos1, pos2, sameObjectMerge, farnum, nearfar, campos);
	//differentElements[0].renderFarSameLine(sameObjectMerge, pos1, pos2);
	
	//mesh1.ChangePoint(pos1); mesh2.ChangePoint();
	/*for (int i = 0; i < model1.mergeid.size(); i++) {
		MyMesh::HalfedgeHandle _hedge = model1.halfedge_handle(*model1.structureFarEdgcontour[model1.mergeid[i]], 1);

		MyMesh::Point curVertex = model1.mesh.point(mesh.from_vertex_handle(_hedge));
		MyMesh::HalfedgeHandle _hedge = model1.halfedge_handle(*model1.structureFarEdgcontour[model1.mergeid[i]], 1);

		MyMesh::Point curVertex1 = model1.mesh.point(mesh.to_vertex_handle(_hedge));
		vertices.push_back(sameObjectMerge[i][0]);
		vertices.push_back(sameObjectMerge[i][1]);
	}
	vector<vec3> vertices;
	/*for (int i = 0; i < sameObjectMerge.size(); i++) {
		vertices.push_back(pos1);
		//vertices.push_back(vec3(sameObjectMerge[i][0][0] - pos1[0], sameObjectMerge[i][0][1] - pos1[1], sameObjectMerge[i][0][2] - pos1[2]));
		vertices.push_back(  pos2);
		//vertices.push_back(vec3(sameObjectMerge[i][1][0] - pos2[0], sameObjectMerge[i][1][1] - pos2[1], sameObjectMerge[i][1][2] - pos2[2]));
	}*/
	/*vertices.push_back(pos1);
	//vertices.push_back(vec3(sameObjectMerge[i][0][0] - pos1[0], sameObjectMerge[i][0][1] - pos1[1], sameObjectMerge[i][0][2] - pos1[2]));
	vertices.push_back(pos2);
	glGenVertexArrays(1, &drawSameObj);
	glBindVertexArray(drawSameObj);

	glGenBuffers(1, &drawSameObjV);
	glBindBuffer(GL_ARRAY_BUFFER, drawSameObjV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);	
	glDrawArrays(GL_LINES, 0, sameObjectMerge.size());// model.mesh.n_edges()
	glBindVertexArray(0);
	/**/
}
void ParametricNode::rendtest() {
glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(drawSameObj);
	glLineWidth(5);
	//glDrawElements(GL_LINES, edgcontour.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0, 1000);// model.mesh.n_edges()
	glBindVertexArray(0);
}