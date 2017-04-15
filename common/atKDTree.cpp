#include "atKDTree.h"
using namespace atRenderEngine::Utility;

atKDNode::atKDNode(int idx, Vector3f element, int cd)
	:data(idx, element, cd)
{
}

atKDNode * atKDTree::insert(const Vector3f& element, const int elemIdx, atKDNode* t, int cd)
{
	if (t == nullptr)
	{
		t = new atKDNode(elemIdx, element, cd);
		return t;
	}
	if (t->data.elementIdx == elemIdx || t->data.element == element)
	{
		//duplicate element - Deal with this. 
	}
	else if(element[cd] < t->data.element[cd])
	{
		int newCD = (cd + 1) % dimK;
		t->leftTree = insert(element, elemIdx, (t->leftTree), newCD);
	}
	else
	{
		int newCD = (cd + 1) % dimK;
		t->rightTree = insert(element, elemIdx, (t->rightTree), newCD);
	}

	return t;
}

bool atKDTree::SearchElement(const Vector3f& elem, atKDNode* topNode, NodeInfo& result, int cd)
{
	if (topNode == nullptr)
	{
		//reached end, search unsuccessful.
		return false;
	}
	if (elem == topNode->data.element)
	{
		//found element, return data 
		result.element = topNode->data.element;
		result.elementIdx = topNode->data.elementIdx;
		return true;
	}
	else
	{
		int newCD = (cd + 1) % dimK;
		if (elem[cd] < topNode->data.element[cd])
		{
			SearchElement(elem, topNode->leftTree, result, newCD);
		}
		else
		{
			SearchElement(elem, topNode->rightTree, result, newCD);
		}
	}
}

void atKDTree::NearestNeighbor(atKDNode* topNode, const Vector3f& x, BoundingRect& boundRect, atKDNode*& nearest, float& distance , std::function<bool(atKDNode* )> conditionOperator)
{
	if (topNode == nullptr)
		return;
	if (&x == nullptr)
		return;

	atKDNode* nearSubTree; 
	atKDNode* farSubTree;
	float* nearBoundDim; 
	float* farBoundDim;
	float temp; 
	float dist_sq;
	int cd = topNode->data.splitDim;

	if (x[cd] <= topNode->data.element[cd])
	{
		nearSubTree = topNode->leftTree;
		farSubTree = topNode->rightTree;
		nearBoundDim = &(boundRect.max[cd]);
		farBoundDim = &(boundRect.min[cd]);
	}
	else
	{
		nearSubTree = topNode->rightTree;
		farSubTree = topNode->leftTree;
		nearBoundDim = &(boundRect.min[cd]);
		farBoundDim = &(boundRect.max[cd]);
	}

	if (nearSubTree != nullptr)
	{
		temp = *nearBoundDim;
		*nearBoundDim = topNode->data.element[cd];

		NearestNeighbor(nearSubTree, x, boundRect, nearest, distance, conditionOperator);
		
		*nearBoundDim = temp;
	}
	if (conditionOperator == nullptr || conditionOperator(topNode))
	{
		dist_sq = Utility::SquaredDistance(topNode->data.element, x);
		if (dist_sq < distance) {
			nearest = topNode;
			distance = dist_sq;
		}
	}
	if (farSubTree != nullptr)
	{
		/* make a slice on the hyperrect */
		temp = *farBoundDim;
		*farBoundDim = topNode->data.element[cd];

		/* Check if we have to recurse down by calculating the closest point of
		the hyperrect and see if it's closer than our minimum distance in
		nearest_dist_sq. */
		
		if (Utility::SquaredDistanceRect(&boundRect, x) < distance)
		{
			/* recurse down into further subtree */
			NearestNeighbor(farSubTree, x, boundRect, nearest, distance, conditionOperator);
		}

		/* undo the slice on the hyperrect */
		*farBoundDim = temp;
	}
}

int atKDTree::RangeSearch(atKDNode* topNode, const Vector3f& x, float& range, vector<atKDNode*>& result, vector<float>& distance, std::function<bool(atKDNode*)> conditionOperator)
{
	if (topNode == nullptr)
	{
		return 0; 
	}
	atKDNode* nearSubTree;
	atKDNode* farSubTree;
	int inBound = 0;
	float distSq = SquaredDistance(topNode->data.element, x);

	if (distSq < range*range)
	{
		inBound = 1;

		if (conditionOperator == nullptr || conditionOperator(topNode))
		{
			result.push_back(topNode);
			distance.push_back(distSq);
		}
	}
	int cd = topNode->data.splitDim;
	float dx = x[cd] - topNode->data.element[cd]; 
	if (dx <= 0)
	{
		nearSubTree = topNode->leftTree;
		farSubTree = topNode->rightTree;
	}
	else
	{
		nearSubTree = topNode->rightTree;
		farSubTree = topNode->leftTree;
	}

	int ret = RangeSearch(nearSubTree, x, range, result, distance, conditionOperator);
	inBound += ret;

	if (ret >= 0 && fabs(dx) < range)
	{
		inBound += RangeSearch(farSubTree, x, range, result, distance, conditionOperator);
	}

	return inBound;
}

atKDTree::atKDTree(int k = 3)
	:dimK(k)
{
}

atKDTree::~atKDTree()
{
}

void atKDTree::BuildTree(vector<Vector3f> listOfElements)
{
	int cd = -1;
	for (int i = 0; i < listOfElements.size(); i++)
	{
		cd = (i) % dimK;
		root = insert(listOfElements[i], i, root, cd);
	}
}
