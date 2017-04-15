#include "atScene.h"

using namespace atRenderEngine;

void atScene::Setup()
{
	//Setup Vertex Array
	glGenVertexArrays(1, &mVertexArrayID);
	glBindVertexArray(mVertexArrayID);

	// Create and compile our GLSL program from the shaders
	mProgramID = Utility::atLoadShaders("../shaders/StandardShading.vs", "../shaders/StandardShading.fs");

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &mVertexBuffer);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);

	// Give our vertices to OpenGL.
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mVertexBufferData.size(), &mVertexBufferData.front(), GL_STATIC_DRAW);
	//for(auto mesh = mMeshesInScene.begin(); mesh != mMeshesInScene.end(); mesh++)
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mMeshesInScene[0]->NumberOfVertices() , &(mMeshesInScene[0]->VertexList()[0]), GL_STATIC_DRAW);


	if (mMeshesInScene[0]->mUVList.size() > 0)
	{
		glGenBuffers(1, &mUVVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mUVVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, mMeshesInScene[0]->mUVList.size() * sizeof(Vector2f), &(mMeshesInScene[0]->mUVList[0]), GL_STATIC_DRAW);
	}

	if (mMeshesInScene[0]->NumberofNormals() > 0)
	{
		glGenBuffers(1, &mNormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, mMeshesInScene[0]->NumberofNormals() * sizeof(Vector3f), mMeshesInScene[0]->NormalList().data(), GL_STATIC_DRAW);
	}

	texLoader = new atTextureLoader();
	// Load the texture
	GLuint Texture = texLoader->loadBMP_custom("../data/LightGray.bmp");//  loadDDS("uvmap.DDS");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture); 

	// Get a handle for our "myTextureSampler" uniform
	mTextureID = glGetUniformLocation(mProgramID, "myTextureSampler");
	// Get a handle for our "MVP" uniform
	mMatrixID = glGetUniformLocation(mProgramID, "MVP");
	mViewMatrixID = glGetUniformLocation(mProgramID, "V");
	mModelMatrixID = glGetUniformLocation(mProgramID, "M");

	//std::cout << "Number of Vertices : " << mMeshesInScene[0]->NumberOfVertices() << std::endl;
}

void atScene::Draw(Matrix4f& viewMatrix , Matrix4f& projectionMatrix)
{
	Matrix4f currentModeMat = mMeshesInScene[0]->GetModel();
	Matrix4f MVPMatrix = projectionMatrix * viewMatrix * currentModeMat;
	glUniformMatrix4fv(mMatrixID, 1, GL_FALSE, MVPMatrix.data());	
	glUniformMatrix4fv(mModelMatrixID, 1, GL_FALSE, mMeshesInScene[0]->GetModel().data());
	glUniformMatrix4fv(mViewMatrixID, 1, GL_FALSE,viewMatrix.data());

	GLuint LightID = glGetUniformLocation(mProgramID, "LightPosition_worldspace");
	Vector3f lightPos(-4, 10, 4);
	glUniform3f(LightID, lightPos.x(), lightPos.y(), lightPos.z());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	 //2nd attribute buffer : UVs
	if (mMeshesInScene[0]->mUVList.size() > 0)
	{
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mUVVertexBuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}
	if (mMeshesInScene[0]->NumberofNormals() > 0)
	{
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

	}

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(mTextureID, 0);

	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	// Generate a buffer for the indices as well
	if (!mUpdateIndices)
	{
		indices.clear();
		mMeshesInScene[0]->ConvertTriangleToVertIndexList(indices);
	}
		
	
	glGenBuffers(1, &mElementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	 //Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	//glDrawArrays(GL_TRIANGLES, 0, mMeshesInScene[0]->NumberOfVertices()); // Starting from vertex 0; 3 vertices total -> 1 triangle
	//glDrawArrays(GL_TRIANGLES, 0, mVertexBufferData.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
	
	mMeshesInScene[0]->meshCollider->Draw(MVPMatrix);
}

void atScene::Cleanup()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mElementbuffer);
	glDeleteProgram(mProgramID);
	glDeleteVertexArrays(1, &mVertexArrayID);
}

void atScene::LoadMesh(std::vector<Vector3f> vertexData)
{
	atMeshData* meshToLoad = new atMeshData();
	meshToLoad->VertexList(vertexData);
	mMeshesInScene.push_back(meshToLoad);
}

void atScene::LoadMesh(const std::string path,bool isBarrel, bool forRender , Vector3f position , Vector3f scaleFactor, float degree )
{
	mUpdateIndices = false; 
	atMeshData* meshToLoad = new atMeshData(); 
	std::string ext = ".DAT";
	std::size_t found = path.find(ext);

	if (found == std::string::npos)
	{
		meshToLoad->ReadObjFile(path);
		meshToLoad->SetTranslation(position);
		meshToLoad->SetScaling(scaleFactor);
		meshToLoad->SetYRotation(degree);
		meshToLoad->meshCollider = new Utility::atKDTreeBBox();		
		meshToLoad->meshCollider->BuildTree(*meshToLoad);
		
	}
	else
	{
		if (forRender)
			meshToLoad->ReadDatFileForRendering(path, isBarrel);
		else
		{
			meshToLoad->ReadDatFile(path, isBarrel);
			std::string savePath = path;
			savePath.replace(found, ext.length(), ".obj");
			meshToLoad->SaveObjFile(savePath);
		}
	}
	//meshToLoad->SaveObjFile("../data/POINT.obj");
	mMeshesInScene.push_back(meshToLoad);
	Setup();
}
void atRenderEngine::atScene::SetRotationForMesh(int i, float degree /*Y axis for now*/)
{
	float newRot = mMeshesInScene[i]->GetYRotation() + degree;
	mMeshesInScene[i]->SetYRotation(newRot);
}
void atRenderEngine::atScene::SetTRSForMesh(int i, Vector3f translate , float degree /*Y axis for now*/ , Vector3f scaleFactor)
{
	mMeshesInScene[i]->SetTranslation(translate);
	mMeshesInScene[i]->SetScaling(scaleFactor);
	mMeshesInScene[i]->SetYRotation(degree);
}

void atRenderEngine::atScene::GetModelForMesh(int i, Matrix4f& outMat)
{
	outMat = Matrix4f(mMeshesInScene[i]->GetModel());
}

bool atScene::GetIntersection(Utility::RayData& ray, Utility::VertexSelectionResult& selectionResult)
{
	bool hitSuccess = false;
	for (int i = 0; i < mMeshesInScene.size(); i++)
	{
		std::vector<int> intersectingIndices;
		mMeshesInScene[i]->GetIntersectingTriangleWithRay(ray, intersectingIndices);
		float minDist = 100000.0;		
		if(intersectingIndices.size()>0)
		{
			//mUpdateIndices = true;
			//indices.clear();
			//mMeshesInScene[0]->ConvertTriangleToVertIndexList(indices, intersectingIndices);
			std::cout << "intersection found" << std::endl;
			for (int j = 0; j < intersectingIndices.size(); j++)
			{
				int triIdx = intersectingIndices[j];
				Vector3f v[3] , vTrans[3]; 
				v[0] = mMeshesInScene[i]->VertexList()[mMeshesInScene[i]->TriangleList()[triIdx].vertexIdx[0]];
				v[1] = mMeshesInScene[i]->VertexList()[mMeshesInScene[i]->TriangleList()[triIdx].vertexIdx[1]];
				v[2] = mMeshesInScene[i]->VertexList()[mMeshesInScene[i]->TriangleList()[triIdx].vertexIdx[2]];

				Matrix4f modelMat = mMeshesInScene[i]->GetModel(); 
				vTrans[0] = (modelMat* Vector4f(v[0].x(), v[0].y(), v[0].z(),1.0f)).head<3>();
				vTrans[1] = (modelMat* Vector4f(v[1].x(), v[1].y(), v[1].z(), 1.0f)).head<3>();
				vTrans[2] = (modelMat* Vector4f(v[2].x(), v[2].y(), v[2].z(), 1.0f)).head<3>();

				float result = 0.0; 
				if (Utility::RayTriangleIntersection(ray, vTrans[0], vTrans[1], vTrans[2], result))
				{
					if (result < minDist)
					{
						minDist = result;
						selectionResult.verts[0] = v[0];
						selectionResult.verts[1] = v[1];
						selectionResult.verts[2] = v[2];

						selectionResult.triangleListIdx = triIdx;						

						hitSuccess = true;

					}
					std::cout << "intersecting with triangle : " << triIdx << std::endl;
					//return true;
				}
			}			
			if (hitSuccess)
			{
				float closestPointDist = 100000.0;
				Vector3f hitPoint = ray.rayOrigin + minDist * ray.rayDirection;
				float dist; 

				for (int k = 0; k < 3; k++)
				{
					dist = Utility::SquaredDistance(selectionResult.verts[k], hitPoint);
					if (dist < closestPointDist)
					{
						closestPointDist = dist;
						selectionResult.vertexListIdx = mMeshesInScene[i]->TriangleList()[selectionResult.triangleListIdx].vertexIdx[k];
						selectionResult.vertexInTriangleIdx = k;
					}						
				}				
			}
			return hitSuccess;
		}
	}
	return hitSuccess;
}

