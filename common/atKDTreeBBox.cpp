#include "atKDTreeBBox.h"
#include "atMesh.h"

using namespace atRenderEngine;
using namespace atRenderEngine::Utility;

Utility::atKDNodeBBox::atKDNodeBBox(std::vector<int>& indices, int cd, BoundingRect bBox)
	:data(indices, cd, bBox)
{
}

void Utility::atKDTreeBBox::insert(std::vector<int>& indices, atMeshData& mesh, atKDNodeBBox* t, int cd , int depth)
{

	//if (t = nullptr)
	//{
	//	BoundingRect bBox = mesh.ComputeBoundingBox(indices);
	//	t = new atKDNodeBBox(indices, cd, bBox);		
	//}
	if (indices.size() == 0 || depth == 3)
	{
		t->leftTree = nullptr;
		t->rightTree = nullptr;		 
	}
	else
	{
		//Split tree into two halves and recurse. 
		BoundingRect leftBBox = t->data.bbox;
		BoundingRect rightBBox = t->data.bbox;

		leftBBox.min[cd] +=  (leftBBox.max[cd] - leftBBox.min[cd] )/ 2.0f;
		rightBBox.max[cd] -= (rightBBox.max[cd] - rightBBox.min[cd]) / 2.0f;

		std::vector<int> leftTriIndices;
		std::vector<int> rightTriIndices; 

		for (int i = 0; i < indices.size(); i++)
		{
			int triIdx = indices[i];
			Vector3f v[3]; 
			v[0] = mesh.VertexList()[mesh.TriangleList()[triIdx].vertexIdx[0]];
			v[1] = mesh.VertexList()[mesh.TriangleList()[triIdx].vertexIdx[1]];
			v[2] = mesh.VertexList()[mesh.TriangleList()[triIdx].vertexIdx[2]];

			BoundingBoxContains IsContained = Utility::IsWithinBoundingBox(leftBBox, v);
			if (IsContained == BoundingBoxContains::BB_FULLY_WITHIN  || IsContained == BoundingBoxContains::BB_PARTIALLY_WITHIN)
			{
				leftTriIndices.push_back(triIdx);				
			}
			IsContained = Utility::IsWithinBoundingBox(rightBBox, v);
			if (IsContained == BoundingBoxContains::BB_FULLY_WITHIN || IsContained == BoundingBoxContains::BB_PARTIALLY_WITHIN)
			{				
				rightTriIndices.push_back(triIdx);
			}
			/*if(IsContained == BoundingBoxContains::BB_NOT_WITHIN)
			{
				rightTriIndices.push_back(i);
			}*/
		}

		// recurse down each subtree. 

		t->leftTree = new atKDNodeBBox(leftTriIndices, cd, leftBBox);
		t->leftTree->name = "Left - " + std::to_string(depth);
		t->rightTree = new atKDNodeBBox(rightTriIndices, cd, rightBBox);
		t->rightTree->name = "Right - " + std::to_string(depth);
		//std::cout << "Depth : " << depth << " Split Dim  : " << cd << std::endl;
		//std::cout << "Left tree \n";
		//std::cout << "Min : " << leftBBox.min.x() << " , " << leftBBox.min.y() << " , " << leftBBox.min.z()  << std::endl;
		//std::cout << "Max : " << leftBBox.max.x() << " , " << leftBBox.max.y() << " , " << leftBBox.max.z() << std::endl;
		//std::cout << "Right tree \n";
		//std::cout << "Min : " << rightBBox.min.x() << " , " << rightBBox.min.y() << " , " << rightBBox.min.z() << std::endl;
		//std::cout << "Max : " << rightBBox.max.x() << " , " << rightBBox.max.y() << " , " << rightBBox.max.z() << std::endl;
		//std::cout << std::endl;
		
		int newCD = (cd + 1) % 3; 
		insert(leftTriIndices, mesh, t->leftTree, newCD, depth + 1);
		insert(rightTriIndices, mesh, t->rightTree, newCD, depth + 1);		
	}	
}

void Utility::atKDTreeBBox::BuildTree( atMeshData& mesh)
{
	std::vector<int> listOfIndices;
	for (int i = 0; i < mesh.NumberOfTriangles(); i++)
	{
		listOfIndices.push_back(i);
	}
	root = new atKDNodeBBox(listOfIndices, 0, mesh.ComputeBoundingBox());
	root->name = "Root";
	insert(listOfIndices, mesh, root, 0, 0);

	programID = Utility::atLoadShaders("../shaders/SimpleColor.vs", "../shaders/SimpleColor.fs");
}

bool Utility::atKDTreeBBox::IntersectingBBox(const Utility::RayData& x, atKDNodeBBox*& nearest, Matrix4f& modelMatrix)
{
	bool found = false; 
	int depth = -1; 
	float distance;
	IntersectingBBox(root, x, nearest, modelMatrix, nullptr, found, depth, distance);
	if (found)
	{
		std::cout << "Found in node  : " << nearest->name << std::endl;
		drawableBBox = nearest; 
	}
		
	return found;
}
void Utility::atKDTreeBBox::IntersectingBBox(atKDNodeBBox* topNode, const Utility::RayData& x, atKDNodeBBox*& nearest,Matrix4f& modelMatrix, std::function<bool(atKDNodeBBox*)> conditionOperator, bool& found, int& depth , float& distance )
{
	if (topNode == nullptr)
		return;
	if (&x == nullptr)
		return;
	float distLeft = 1000000.0, distRight = 1000000.0;
	if ((conditionOperator == nullptr || conditionOperator(topNode))&& 
		Utility::TestRayOBBIntersection(x.rayOrigin, x.rayDirection, topNode->data.bbox.min, topNode->data.bbox.max, modelMatrix, distance))
	{
		nearest = topNode; 
		found = true; 
		depth = depth + 1;
		atKDNodeBBox* tempNodeLeft = nullptr, *tempNodeRight = nullptr;

		IntersectingBBox(topNode->leftTree, x, tempNodeLeft, modelMatrix, conditionOperator, found,depth, distLeft);
		IntersectingBBox(topNode->rightTree, x, tempNodeRight, modelMatrix, conditionOperator, found,depth , distRight);
		if (distLeft < distRight && tempNodeLeft->data.elementIdx.size()>0)
		{
			nearest = tempNodeLeft;
			distance = distLeft;
		}
		else if(distLeft > distRight && tempNodeRight->data.elementIdx.size()>0)
		{
			nearest = tempNodeRight;
			distance = distRight;
		}
		/*if (distLeft != 1000000.0 && distRight != 1000000.0)
		{
			
		}*/
	}
	else
		return;
}

void Utility::atKDTreeBBox::Draw(Matrix4f& MVP)
{
	if (drawableBBox == nullptr)
		DrawRecursive(root, MVP);
	else
		DrawBoundingBox(drawableBBox, MVP);
	/*DrawBoundingBox(root->leftTree, MVP);
	DrawBoundingBox(root->rightTree, MVP);*/
}
void Utility::atKDTreeBBox::DrawBoundingBox(atKDNodeBBox* node, Matrix4f& MVP)
{
	Vector3f bbMin = node->data.bbox.min;
	Vector3f bbMax = node->data.bbox.max;

	GLfloat g_vertex_buffer_data[] =
	{
		bbMin.x(), bbMin.y(),bbMin.z(),
		bbMin.x(), bbMax.y(),bbMin.z(),

		bbMin.x(), bbMax.y(),bbMin.z(),
		bbMin.x(), bbMax.y(),bbMax.z(),


		bbMin.x(), bbMax.y(),bbMax.z(),
		bbMin.x(), bbMin.y(),bbMax.z(),

		bbMin.x(), bbMin.y(),bbMax.z(),
		bbMin.x(), bbMin.y(),bbMin.z(),



		bbMax.x(), bbMin.y(),bbMin.z(),
		bbMax.x(), bbMax.y(),bbMin.z(),

		bbMax.x(), bbMax.y(),bbMin.z(),
		bbMax.x(), bbMax.y(),bbMax.z(),

		bbMax.x(), bbMax.y(),bbMax.z(),
		bbMax.x(), bbMin.y(),bbMax.z(),

		bbMax.x(), bbMin.y(),bbMax.z(),
		bbMax.x(), bbMin.y(),bbMin.z(),


		bbMin.x(), bbMin.y(),bbMin.z(),
		bbMax.x(), bbMin.y(),bbMin.z(),

		bbMin.x(), bbMax.y(),bbMin.z(),
		bbMax.x(), bbMax.y(),bbMin.z(),

		bbMin.x(), bbMax.y(),bbMax.z(),
		bbMax.x(), bbMax.y(),bbMax.z(),

		bbMin.x(), bbMin.y(),bbMax.z(),
		bbMax.x(), bbMin.y(),bbMax.z()

	};
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glUseProgram(programID);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, MVP.data());

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_LINES, 0, 8 * 3); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
}
void Utility::atKDTreeBBox::DrawRecursive(atKDNodeBBox* node, Matrix4f& MVP)
{
	if (node == nullptr)
		return;
	else
	{
		DrawBoundingBox(node, MVP);

		DrawRecursive(node->leftTree, MVP);
		DrawRecursive(node->rightTree, MVP);
	}
}
Utility::atKDTreeBBox::atKDTreeBBox()
	:drawableBBox(nullptr)
{
}

Utility::atKDTreeBBox::~atKDTreeBBox()
{
}


