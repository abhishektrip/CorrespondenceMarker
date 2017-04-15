#pragma once
#include <vector>
#include "atUtility.h"
#include "atKDTreeBBox.h"

namespace atRenderEngine
{
	class atMeshData
	{
	public:
		atMeshData();
		~atMeshData();
		std::vector<Vector2f> mUVList;

		size_t NumberOfTriangles();
		std::vector<Utility::TriangleData>& TriangleList();
		void TriangleList(std::vector<Utility::TriangleData> triList);
		void ConvertTriangleToVertIndexList(std::vector<unsigned int>& triVertList);

		void ConvertTriangleToVertIndexList(std::vector<unsigned int>& triVertList, std::vector<int>& triList);

		size_t NumberOfVertices();
		std::vector<Vector3f>& VertexList();
		void VertexList(std::vector<Vector3f> vertList);

		size_t NumberofNormals();
		std::vector<Vector3f>& NormalList();
		void NormalList(std::vector<Vector3f> normList);

		void SetTranslation(Vector3f deltaDist);
		void SetYRotation(float degree);
		float GetYRotation();
		void SetScaling(Vector3f scaleFactor);
		
		Matrix4f GetModel();		
		Utility::BoundingRect ComputeBoundingBox(std::vector<int>& indices);
		Utility::BoundingRect ComputeBoundingBox();
		void GetIntersectingTriangleWithRay(Utility::RayData& ray, std::vector<int>& indices );

		bool ReadDatFileForRendering(std::string path, bool isBarrel);
		bool ReadDatFile(std::string path, bool isBarrel = false);
		bool ReadObjFile(std::string path);
		bool SaveObjFile(std::string path);
		Utility::atKDTreeBBox* meshCollider; 
	private:
		Matrix4f mRotationMatrix;
		Matrix4f mTranslationMatrix;
		Matrix4f mScalingMatrix;
		float mRotationInY;
		std::vector<Utility::TriangleData> mTriangleList;
		std::vector<Vector3f> mVertexList;
		std::vector<Vector3f> mNormalList;
		

	};
}