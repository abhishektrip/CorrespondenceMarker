#include "atMesh.h"
using namespace atRenderEngine;

atRenderEngine::atMeshData::atMeshData()
	:mTranslationMatrix(Matrix4f::Identity()), mScalingMatrix(Matrix4f::Identity()), mRotationMatrix(Matrix4f::Identity()) , mRotationInY(0)
{
	SetScaling(Vector3f::Ones());
}
atRenderEngine::atMeshData::~atMeshData()
{
}

size_t atMeshData::NumberOfTriangles()
{
	return mTriangleList.size();
}
std::vector<Utility::TriangleData>& atMeshData::TriangleList()
{
	return mTriangleList;
}
void atMeshData::TriangleList(std::vector<Utility::TriangleData> triList)
{
	mTriangleList = triList;
}
void atMeshData::ConvertTriangleToVertIndexList(std::vector<unsigned int>& triVertList)
{
	for (auto tri = mTriangleList.begin(); tri != mTriangleList.end(); tri++)
	{
		triVertList.push_back(tri->vertexIdx[0]);
		triVertList.push_back(tri->vertexIdx[1]);
		triVertList.push_back(tri->vertexIdx[2]);
	}
}
void atMeshData::ConvertTriangleToVertIndexList(std::vector<unsigned int>& triVertList , std::vector<int>& triList)
{
	for (auto tri = triList.begin(); tri != triList.end(); tri++)
	{
		triVertList.push_back(mTriangleList[*tri].vertexIdx[0]);
		triVertList.push_back(mTriangleList[*tri].vertexIdx[1]);
		triVertList.push_back(mTriangleList[*tri].vertexIdx[2]);
	}
}

size_t atMeshData::NumberOfVertices()
{
	return mVertexList.size();
}
std::vector<Vector3f>& atMeshData::VertexList()
{
	return mVertexList;
}
void atMeshData::VertexList(std::vector<Vector3f> vertList)
{
	mVertexList.swap(vertList);
}

size_t atMeshData::NumberofNormals()
{
	return mNormalList.size();
}
std::vector<Vector3f>& atMeshData::NormalList()
{
	return mNormalList;
}
void atMeshData::NormalList(std::vector<Vector3f> normList)
{
	mNormalList = mNormalList;
}
void atMeshData::SetTranslation(Vector3f deltaDist)
{
	mTranslationMatrix(0, 3) += deltaDist.x();
	mTranslationMatrix(1, 3) += deltaDist.y();
	mTranslationMatrix(2, 3) += deltaDist.z();
}
void atMeshData::SetScaling(Vector3f scaleFactor)
{
	mTranslationMatrix(0, 0) = scaleFactor.x();
	mTranslationMatrix(1, 1) = scaleFactor.y();
	mTranslationMatrix(2, 2) = scaleFactor.z();
}
// Purely Y rotation for now.. 
//TODO : Change this to rotate about any arbitrary pivot & axis
void atMeshData::SetYRotation(float degree)
{
	mRotationInY = degree;
	mRotationMatrix =  Utility::eulerAngleYXZ(mRotationInY, 0, 0);
}
float atMeshData::GetYRotation()
{
	return mRotationInY;
}
Matrix4f atRenderEngine::atMeshData::GetModel()
{
	Matrix4f modelMatrix = (mTranslationMatrix*mRotationMatrix*mScalingMatrix);
	return modelMatrix;
}

Utility::BoundingRect atMeshData::ComputeBoundingBox(std::vector<int>& indices)
{
	Utility::BoundingRect bRect;
	Vector3f vMax = mVertexList[mTriangleList[indices[0]].vertexIdx[0]];
	Vector3f vMin = mVertexList[mTriangleList[indices[0]].vertexIdx[0]];

	for (int iv = 0; iv < indices.size(); iv++)
	{
		Vector3f cV = mVertexList[indices[iv]];
		vMax.x() = std::max(cV.x(), vMax.x());  vMin.x() = std::min(cV.x(), vMin.x());
		vMax.y() = std::max(cV.y(), vMax.y());  vMin.y() = std::min(cV.y(), vMin.y());
		vMax.z() = std::max(cV.z(), vMax.z());  vMin.z() = std::min(cV.z(), vMin.z());
	}
	//Vector4f vMaxTransformed = mModelMatrix*Vector4f(vMax.x(), vMax.y(), vMax.z(), 1);
	//Vector4f vMinTransformed = mModelMatrix*Vector4f(vMin.x(), vMin.y(), vMin.z(), 1);

	//bRect.max = Vector3f(vMaxTransformed.x(), vMaxTransformed.y(), vMaxTransformed.z());// +Vector3f(0.25, 0.25, 0.25);
	//bRect.min = Vector3f(vMinTransformed.x(), vMinTransformed.y(), vMinTransformed.z());;// -Vector3f(0.25, 0.25, 0.25);
	
	bRect.max = vMax;
	bRect.min = vMin;
	return bRect;

}
Utility::BoundingRect atMeshData::ComputeBoundingBox()
{
	Utility::BoundingRect bRect;
	Vector3f vMax = mVertexList[0];
	Vector3f vMin = mVertexList[0];

	/* find the bounding box of the model */
	for (int iv = 0; iv < NumberOfVertices(); iv++)
	{
		Vector3f cV = mVertexList[iv];
		vMax.x() = std::max(cV.x(), vMax.x());  vMin.x() = std::min(cV.x(), vMin.x());
		vMax.y() = std::max(cV.y(), vMax.y());  vMin.y() = std::min(cV.y(), vMin.y());
		vMax.z() = std::max(cV.z(), vMax.z());  vMin.z() = std::min(cV.z(), vMin.z());
	}

	//Vector4f vMaxTransformed = mModelMatrix*Vector4f(vMax.x(), vMax.y(), vMax.z(), 1);
	//Vector4f vMinTransformed = mModelMatrix*Vector4f(vMin.x(), vMin.y(), vMin.z(), 1);

	//bRect.max = Vector3f(vMaxTransformed.x(), vMaxTransformed.y(), vMaxTransformed.z());// +Vector3f(0.25, 0.25, 0.25);
	//bRect.min = Vector3f(vMinTransformed.x(), vMinTransformed.y(), vMinTransformed.z());;// -Vector3f(0.25, 0.25, 0.25);

	bRect.max = vMax;
	bRect.min = vMin;
	return bRect;
}

void atRenderEngine::atMeshData::GetIntersectingTriangleWithRay(Utility::RayData & ray, std::vector<int>& indices)
{
	Utility::atKDNodeBBox* nearest; 		
	if (meshCollider->IntersectingBBox(ray, nearest, GetModel()) && nearest != nullptr)
	{
		indices = std::vector<int>(nearest->data.elementIdx);
	}
}

bool atMeshData::ReadDatFileForRendering(std::string path, bool isBarrel)
{
	using namespace Utility;
	using namespace std;

	FILE* file = fopen(path.c_str(), "r");
	char prefix[3];   // prefix reader - 3 chars should be sufficient. 
	unsigned int count = 0;
	unsigned int eqTriCount = 0;
	unsigned int noneqTriCount = 0;
	Vector2f uvValues[3] = { Vector2f(0.25,0.75),Vector2f(0.75,0.75), Vector2f(0.25,0.25) };
	//mUVList.push_back(uvValues[0]);
	//mUVList.push_back(uvValues[1]);
	//mUVList.push_back(uvValues[2]);

	float degToRot = 0;
	float degToRotStep = (isBarrel) ? 20 : 120;
	int numSymmetricParts = (isBarrel) ? 18 : 3; // (360/degtorot)
	if (file == nullptr)
	{
		cout << "Could not open file :" << path << endl;
		std::fclose(file);
		return false;
	}
	else
	{
		while (1)
		{

			int res = fscanf(file, "%d,\n", &count);
			if (res == EOF)
				break;

			std::vector<Vector3f> tempVertList(numSymmetricParts * count, Vector3f(0, 0, 0));
			//mVertexList = vector<Vector3f>(numSymmetricParts * count, Vector3f(0, 0, 0));
			for (int i = 0; i < count; i++)
			{
				int idx;
				Vector3f vert;
				//Vector2f uv(0, 0);
				fscanf(file, "%d, %f, %f, %f\n", &idx, &(vert.x()), &(vert.y()), &(vert.z()));

				degToRot = 0.0f;
				for (int j = 0; j < numSymmetricParts; j++, degToRot += degToRotStep)
				{
					Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
					tempVertList[j*count + i] = rotMat*vert;
				}
			}
			
			res = fscanf(file, "%d,\n", &eqTriCount);
			if (res == EOF)
				break;
			else
			{
				for (int i = 0; i < eqTriCount; i++)
				{

					int idx;
					int vertIdx[3];
					int colorIdx[3];
					if (isBarrel)
						fscanf(file, "%d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]));
					else
						fscanf(file, "%d, %d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]), &(colorIdx[2]));

					//Change obj 1 index to c++ 0 index. 
					vertIdx[0]--;
					vertIdx[1]--;
					vertIdx[2]--;

					degToRot = 0.0f;
					for (int i = 0; i < numSymmetricParts; i++, degToRot += degToRotStep)
					{
						TriangleData triData;
						//Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
						mVertexList.push_back(tempVertList[count*i + vertIdx[0]]);
						mVertexList.push_back(tempVertList[count*i + vertIdx[1]]);
						mVertexList.push_back(tempVertList[count*i + vertIdx[2]]);

						triData.vertexIdx[0] = mVertexList.size() - 3; 
						triData.vertexIdx[1] = mVertexList.size() - 2;
						triData.vertexIdx[2] = mVertexList.size() - 1;

						if (isBarrel && i > 0)
						{
							int idx = i % 2;
							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
						}
						else
						{
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
						}

						mTriangleList.push_back(triData);
					}

					//std::cout << "COLOR IDX : " << colorIdx[0] << std::endl;
					//mUVList[triData.vertexIdx[0]] = uvValues[colorIdx[0]];
					//mUVList[triData.vertexIdx[1]] = uvValues[colorIdx[0]];
					//mUVList[triData.vertexIdx[2]] = uvValues[colorIdx[0]];
					//mTriangleList.push_back(triData);
				}
			}

			res = fscanf(file, "%d,\n", &noneqTriCount);
			if (res == EOF)
				break;
			else
			{
				for (int i = 0; i < noneqTriCount; i++)
				{
					int idx;
					int vertIdx[3];
					int colorIdx[3];
					if (isBarrel)
						fscanf(file, "%d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]));
					else
						fscanf(file, "%d, %d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]), &(colorIdx[2]));

					//Change obj 1 index to c++ 0 index. 
					vertIdx[0]--;
					vertIdx[1]--;
					vertIdx[2]--;

					degToRot = 0.0f;
					for (int i = 0; i < numSymmetricParts; i++, degToRot += degToRotStep)
					{
						TriangleData triData;
						//Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
						mVertexList.push_back(tempVertList[count*i + vertIdx[0]]);
						mVertexList.push_back(tempVertList[count*i + vertIdx[1]]);
						mVertexList.push_back(tempVertList[count*i + vertIdx[2]]);

						triData.vertexIdx[0] = mVertexList.size() - 3;
						triData.vertexIdx[1] = mVertexList.size() - 2;
						triData.vertexIdx[2] = mVertexList.size() - 1;

						if (isBarrel && i > 0)
						{
							int idx = i % 2;

							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
						}
						else
						{
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
						}

						mTriangleList.push_back(triData);
					}
				}
			}
			break;
		}
		std::fclose(file);
		return true;
	}
}
bool atMeshData::ReadDatFile(std::string path, bool isBarrel)
{
	using namespace Utility;
	using namespace std;

	FILE* file = fopen(path.c_str(), "r");
	char prefix[3];   // prefix reader - 3 chars should be sufficient. 
	unsigned int count = 0;
	unsigned int eqTriCount =0;
	unsigned int noneqTriCount =0;
	Vector2f uvValues[3] = { Vector2f(0.25,0.75),Vector2f(0.75,0.75), Vector2f(0.25,0.25) };
	//mUVList.push_back(uvValues[0]);
	//mUVList.push_back(uvValues[1]);
	//mUVList.push_back(uvValues[2]);

	float degToRot = 0;
	float degToRotStep = (isBarrel) ? 20 : 120;
	int numSymmetricParts = (isBarrel) ? 18 : 3; // (360/degtorot)
	if (file == nullptr)
	{
		cout << "Could not open file :" << path << endl;
		std::fclose(file);
		return false;
	}
	else
	{
		while (1)
		{
			
			int res = fscanf(file, "%d,\n", &count);
			if (res == EOF)
				break;

			//std::vector<Vector3f> tempVertList(3 * count, Vector3f(0, 0, 0));
			mVertexList = vector<Vector3f>(numSymmetricParts * count, Vector3f(0, 0, 0));
			for (int i = 0; i < count; i++)
			{
				int idx;
				Vector3f vert;
				//Vector2f uv(0, 0);
				fscanf(file, "%d, %f, %f, %f\n", &idx, &(vert.x()), &(vert.y()), &(vert.z()));

				degToRot = 0.0f;
				for (int j = 0; j < numSymmetricParts; j++, degToRot += degToRotStep)
				{
					Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
					mVertexList[j*count + i] = rotMat*vert;
				}
			}
			mUVList = vector<Vector2f>(numSymmetricParts * count, Vector2f(0, 0));
			res = fscanf(file, "%d,\n", &eqTriCount);
			if (res == EOF)
				break;
			else
			{
				for (int i = 0; i < eqTriCount; i++)
				{
					
					int idx;
					int vertIdx[3];
					int colorIdx[3]; 
					if(isBarrel)
						fscanf(file, "%d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]));
					else
						fscanf(file, "%d, %d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]), &(colorIdx[2]));

					//Change obj 1 index to c++ 0 index. 
					vertIdx[0]--;
					vertIdx[1]--;
					vertIdx[2]--;

					degToRot = 0.0f;
					for (int i = 0; i < numSymmetricParts; i++, degToRot +=degToRotStep)
					{
						TriangleData triData;
						//Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[0]]);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[1]]);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[2]]);
						triData.vertexIdx[0] = count*i + vertIdx[0];
						triData.vertexIdx[1] = count*i + vertIdx[1];
						triData.vertexIdx[2] = count*i + vertIdx[2];

						if (isBarrel && i > 0)
						{
							int idx = i % 2; 
							mUVList[count*i + vertIdx[0]] = uvValues[colorIdx[idx]];
							mUVList[count*i + vertIdx[1]] = uvValues[colorIdx[idx]];
							mUVList[count*i + vertIdx[2]] = uvValues[colorIdx[idx]];

							triData.uvIdx[0] = count*i + vertIdx[0];
							triData.uvIdx[1] = count*i + vertIdx[1];
							triData.uvIdx[2] = count*i + vertIdx[2];



							/*mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);*/
						}
						else
						{
							mUVList[count*i + vertIdx[0]] = uvValues[colorIdx[i]];
							mUVList[count*i + vertIdx[1]] = uvValues[colorIdx[i]];
							mUVList[count*i + vertIdx[2]] = uvValues[colorIdx[i]];

							triData.uvIdx[0] = count*i + vertIdx[0];
							triData.uvIdx[1] = count*i + vertIdx[1];
							triData.uvIdx[2] = count*i + vertIdx[2];
							/*mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);*/
						}

						mTriangleList.push_back(triData);
					}

					//std::cout << "COLOR IDX : " << colorIdx[0] << std::endl;
					//mUVList[triData.vertexIdx[0]] = uvValues[colorIdx[0]];
					//mUVList[triData.vertexIdx[1]] = uvValues[colorIdx[0]];
					//mUVList[triData.vertexIdx[2]] = uvValues[colorIdx[0]];
					//mTriangleList.push_back(triData);
				}
			}
			
			res = fscanf(file, "%d,\n", &noneqTriCount);
			if (res == EOF)
				break;
			else
			{
				for (int i = 0; i < noneqTriCount; i++)
				{
					int idx;
					int vertIdx[3];
					int colorIdx[3];
					if (isBarrel)
						fscanf(file, "%d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]));
					else
						fscanf(file, "%d, %d, %d, %d, %d, %d, %d,\n", &idx, &(vertIdx[0]), &(vertIdx[1]), &(vertIdx[2]), &(colorIdx[0]), &(colorIdx[1]), &(colorIdx[2]));

					//Change obj 1 index to c++ 0 index. 
					vertIdx[0]--;
					vertIdx[1]--;
					vertIdx[2]--;

					degToRot = 0.0f;
					for (int i = 0; i < numSymmetricParts; i++, degToRot += degToRotStep)
					{
						TriangleData triData;
						//Eigen::Matrix3f rotMat = Utility::rotateZByDegree(degToRot);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[0]]);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[1]]);
						//mVertexList.push_back(rotMat*tempVertList[triData.vertexIdx[2]]);
						triData.vertexIdx[0] = count*i + vertIdx[0];
						triData.vertexIdx[1] = count*i + vertIdx[1];
						triData.vertexIdx[2] = count*i + vertIdx[2];

						if (isBarrel && i > 0)
						{
							int idx = i % 2;
							mUVList[count*i + vertIdx[0]] = uvValues[colorIdx[idx]];
							mUVList[count*i + vertIdx[1]] = uvValues[colorIdx[idx]];
							mUVList[count*i + vertIdx[2]] = uvValues[colorIdx[idx]];

							triData.uvIdx[0] = count*i + vertIdx[0];
							triData.uvIdx[1] = count*i + vertIdx[1];
							triData.uvIdx[2] = count*i + vertIdx[2];

							/*mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);
							mUVList.push_back(uvValues[colorIdx[idx]]);*/
						}
						else
						{
							mUVList[count*i + vertIdx[0]] = uvValues[colorIdx[i]];
							mUVList[count*i + vertIdx[1]] = uvValues[colorIdx[i]];
							mUVList[count*i + vertIdx[2]] = uvValues[colorIdx[i]];

							triData.uvIdx[0] = count*i + vertIdx[0];
							triData.uvIdx[1] = count*i + vertIdx[1];
							triData.uvIdx[2] = count*i + vertIdx[2];
							/*mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);
							mUVList.push_back(uvValues[colorIdx[i]]);*/
						}

						mTriangleList.push_back(triData);
					}
				}
			}
			break;
		}
		std::fclose(file);
		return true;
	}
}

bool atMeshData::ReadObjFile(std::string path)
{
	using namespace Utility;
	using namespace std;

	FILE* file = fopen(path.c_str(), "r");
	char prefix[3];   // prefix reader - 3 chars should be sufficient. 

	if (file == nullptr)
	{
		cout << "Could not open file :" << path << endl;
		fclose(file);
		return false;
	}
	else
	{
		while (fscanf(file, "%2s", prefix) != EOF)
		{
			if (strcmp(prefix, "v") == 0)
			{
				Vector3f vert;
				fscanf(file, "%f %f %f\n", &(vert.x()), &(vert.y()), &(vert.z()));
				mVertexList.push_back(vert);
			}
			else if (strcmp(prefix, "vn") == 0)
			{
				Vector3f normal;
				fscanf(file, "%f %f %f\n", &(normal.x()), &(normal.y()), &(normal.z()));
				mNormalList.push_back(normal);
			}
			else if (strcmp(prefix, "vt") == 0)
			{
				Vector2f uv;
				fscanf(file, "%f %f \n", &(uv.x()), &(uv.y()));
				mUVList.push_back(uv);
			}
			else if (strcmp(prefix, "f") == 0)
			{
				TriangleData triData;
				for (int i = 0; i < 3; i++)
				{
					fscanf(file, "%d/", &(triData.vertexIdx[i]));
					if ((prefix[0] = (char)fgetc(file)) != '/')
					{
						//skip texture coordinates if found. 
						ungetc(prefix[0], file);  fscanf(file, "%*d/");
					}
					fscanf(file, "%d/", &(triData.normalIdx[i]));
				}
				//Change obj 1 index to c++ 0 index. 
				triData.vertexIdx[0]--;
				triData.vertexIdx[1]--;
				triData.vertexIdx[2]--;

				triData.normalIdx[0]--;
				triData.normalIdx[1]--;
				triData.normalIdx[2]--;

				triData.uvIdx[0] = triData.vertexIdx[0];
				triData.uvIdx[1] = triData.vertexIdx[1];
				triData.uvIdx[2] = triData.vertexIdx[2];

				mTriangleList.push_back(triData);
			}
			else
			{
				int readChar;
				while ((readChar = fgetc(file)) != '\n' && readChar != EOF);
			}
		}
		for (int i = 0; i < mVertexList.size(); i++)
			mUVList.push_back(Vector2f(0.5, 0.5));
		fclose(file);
		return true;
	}
}
bool atMeshData::SaveObjFile(std::string path)
{
	FILE *fd = fopen(path.c_str(), "w");
	if (fd != NULL)
	{
		fprintf(fd, "mtllib EggColorMaterial.mtl\n");
		/* save vertex information */
		for (int i = 0; i < NumberOfVertices(); i++)
		{
			fprintf(fd, "v   %12.9f   %12.9f   %12.9f\n",
				mVertexList[i].x(), mVertexList[i].y(), mVertexList[i].z());
		}

		for (int i = 0; i < mUVList.size(); i++)
		{
			fprintf(fd, "vt   %12.9f   %12.9f\n",
				mUVList[i].x(), mUVList[i].y());
		}
		/* save normal vector information */
		//for (int i = 0; i < NumberofNormals(); i++)
		//{
		//	fprintf(fd, "vn   %12.9f   %12.9f   %12.9f\n",
		//		mNormalList[i].x(), mNormalList[i].y(), mNormalList[i].y());
		//}

		/* save triangular unit information

		Notice that vertex/normvec indexes in .obj file are one-based, so
		they need a incrementation before being written to filestream. */
		fprintf(fd, "usemtl eggColors\n");
		for (int i = 0; i < NumberOfTriangles(); i++)
		{
			fprintf(fd, "f %d/%d/ %d/%d/ %d/%d/\n",
				mTriangleList[i].vertexIdx[0] + 1, mTriangleList[i].uvIdx[0]+1,
				mTriangleList[i].vertexIdx[1] + 1, mTriangleList[i].uvIdx[1]+1,
				mTriangleList[i].vertexIdx[2] + 1, mTriangleList[i].uvIdx[2]+1);
		}

		fclose(fd);
		return true;
	}
	else
	{
		return false;
	}
}
