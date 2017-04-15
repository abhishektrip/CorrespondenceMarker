#pragma once
#include "atUtility.h"
//#include "atMesh.h"

namespace atRenderEngine
{
	class atMeshData;
	namespace Utility
	{
		struct NodeInfoBBox
		{
			int splitDim;
			std::vector<int> elementIdx;
			BoundingRect bbox;
			//TriangleData element;

			NodeInfoBBox(std::vector<int>& idx, int currentDim, BoundingRect bBox)
				: elementIdx(idx), splitDim(currentDim), bbox(bBox) {}
		};

		class atKDNodeBBox
		{
		public:
			std::string name; 
			NodeInfoBBox data;
			atKDNodeBBox* leftTree;
			atKDNodeBBox* rightTree;

			atKDNodeBBox(std::vector<int>& indices, int cd, BoundingRect bBox);
			
		};
		class atKDTreeBBox
		{
			void insert(std::vector<int>& indices, atMeshData& mesh, atKDNodeBBox* t, int cd, int depth);
		public:
			atKDNodeBBox* root;
			atKDNodeBBox* drawableBBox; 
			void BuildTree(atMeshData& mesh);
			bool IntersectingBBox(const Utility::RayData & x, atKDNodeBBox *& nearest, Matrix4f & modelMatrix);
			void IntersectingBBox(atKDNodeBBox * topNode, const Utility::RayData & x, atKDNodeBBox *& nearest, Matrix4f& modelMatrix, std::function<bool(atKDNodeBBox*)> conditionOperator, bool& found, int& depth , float& distance);
			atKDTreeBBox();
			~atKDTreeBBox();
			void Draw(Matrix4f&);
			void DrawBoundingBox(atKDNodeBBox * node, Matrix4f & MVP);
			void DrawRecursive(atKDNodeBBox * node, Matrix4f & MVP);
		private:
			GLuint programID; 
		};

		

	}
}

