#pragma once
#include <vector>
#include <Eigen/Core>
#include "atUtility.h"
using namespace std;
using namespace Eigen;

namespace atRenderEngine
{
	namespace Utility
	{
		//enum SplitDimension
		//{
		//	dimX = 0,
		//	dimY, 
		//	dimZ
		//};

		struct NodeInfo
		{
			int splitDim;
			int elementIdx;
			Vector3f element;

			NodeInfo(int idx, Vector3f elem, int currentDim)
				: elementIdx(idx), element(elem), splitDim(currentDim){}
		};
		class atKDNode
		{
		public:
			NodeInfo data;
			atKDNode* leftTree;
			atKDNode* rightTree;
		
			atKDNode(int idx, Vector3f element, int cd);
		};
		class atKDTree
		{
			const int dimK; 
			 
			
			atKDNode* insert(const Vector3f& element ,const int elemIdx, atKDNode* t, int cd);
			
		public:			
			atKDNode* root;
			
			atKDTree(int k);
			~atKDTree();
			void BuildTree(vector<Vector3f> listOfElements);
			bool SearchElement(const Vector3f & elem, atKDNode * topNode, NodeInfo & result, int cd);
			void NearestNeighbor(atKDNode * topNode, const Vector3f & x, BoundingRect & boundRect, atKDNode*& nearest, float & distance, std::function<bool(atKDNode*)> conditionOperator);
			int RangeSearch(atKDNode * topNode, const Vector3f & x, float & range, vector<atKDNode*>& result, vector<float>& distance, std::function<bool(atKDNode*)> conditionOperator);
		};
	}
}


