#pragma once
#include "atUtility.h"
#include "atMesh.h"
#include "atTextureLoader.h"
namespace atRenderEngine
{
	class atScene
	{
	public:
		atScene() {}
		~atScene() {}
		void Setup();
		void Draw(Matrix4f& viewMatrix, Matrix4f& projectionMatrix);
		void Cleanup();
		void LoadMesh(std::vector<Vector3f> vertexData);
		void LoadMesh(const std::string path, bool isBarrel, bool forRender ,Vector3f position = Vector3f::Zero(), Vector3f scaleFactor = Vector3f::Ones(), float degree = 0.0);
		void SetRotationForMesh(int i, float degree);
		void SetTRSForMesh(int i, Vector3f translate, float degree, Vector3f scaleFactor);
		void GetModelForMesh(int i, Matrix4f& outMat);
		bool GetIntersection(Utility::RayData& ray, Utility::VertexSelectionResult& result);
		GLuint ProgramID() { return mProgramID; }
		GLuint MatrixID() { return mMatrixID; }
	private:
		GLuint mElementbuffer;
		GLuint mVertexArrayID;
		GLuint mVertexBuffer;
		GLuint mUVVertexBuffer;
		GLuint mNormalBuffer;
		std::vector<unsigned int> indices;
		std::vector<GLfloat> mVertexBufferData;
		std::vector<atMeshData *> mMeshesInScene;
		GLuint mMatrixID;
		GLuint mViewMatrixID;
		GLuint mModelMatrixID;
		GLuint mProgramID;
		GLuint mTextureID;
		atTextureLoader* texLoader; 
		bool mUpdateIndices; 
	};

}
