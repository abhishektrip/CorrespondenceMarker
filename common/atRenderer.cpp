#include "atRenderer.h"
using namespace atRenderEngine;

atRenderer::atRenderer()
	:mWireFrame(false) , mSelectionActiveSrc(false) , mSelectionActiveTgt(false) , meshIdxForSelection(0)
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initalize GLFW" << std::endl;	
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old 

	//rot120 = Eigen::Matrix4f::Identity();
	//rot120(1, 1) = cos(Utility::ToRadians(120));
	//rot120(1, 2) = -sin(Utility::ToRadians(120));
	//rot120(2, 1) = sin(Utility::ToRadians(120));
	//rot120(2, 2) = cos(Utility::ToRadians(120));
}

atRenderer::~atRenderer()
{
	for(int i = 0; i < mScenes.size(); i++)
		mScenes[i]->Cleanup();
}

void atRenderer::Setup(std::string title, int width, int height)
{	
	mWindow = new atWindow(this, title, width, height);
	//atWindow* anotherWidow = new atWindow(this, "Blah", 640, 480);

	//Initialize Glew
	glfwMakeContextCurrent((GLFWwindow*)mWindow->getWindowHandle());
	glfwSwapInterval(1);

	glewExperimental = true; // Needed in core profile 
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	mLineShader = new Utility::atLineShader();
}

void atRenderEngine::atRenderer::AddCamera(atCamera * camera)
{
	mCamera = camera;
}

void atRenderEngine::atRenderer::AddScene(atScene * scene)
{
	mScenes.push_back(scene);
}

void atRenderer::Draw()
{
	// Get a handle for our "MVP" uniform
	//GLuint MatrixID = glGetUniformLocation(scene->ProgramID(), "MVP");
	Eigen::Matrix4f MVPMatrix;
	Eigen::Matrix4f viewMatrix;
	Eigen::Matrix4f perspectiveMatrix;
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	
	while (!glfwWindowShouldClose((GLFWwindow*)mWindow->getWindowHandle()))
	{
		//Add Drawing functions here
		glClearColor(0.0, 0.0, 0.4, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < mScenes.size(); i++)
		{
			// Use our shader
			glUseProgram(mScenes[i]->ProgramID());
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			//camera->SetModel(Eigen::Matrix4f::Identity());
			if (mWireFrame)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			mCamera->GetMVP(MVPMatrix);						
			mCamera->GetView(viewMatrix);
			mCamera->GetPerspective(perspectiveMatrix);
			mScenes[i]->Draw(viewMatrix,perspectiveMatrix);

			if (mSelectionActiveSrc)
			{
				Matrix4f cModelMatrix; 
				mScenes[mPreviousSelectedVertexInfo.meshIdx]->GetModelForMesh(0, cModelMatrix);
				Matrix4f MVPLine = MVPMatrix*cModelMatrix;
				mLineShader->Draw(std::vector<Vector3f>{mPreviousSelectedVertexInfo.verts[0], mPreviousSelectedVertexInfo.verts[1], mPreviousSelectedVertexInfo.verts[1], mPreviousSelectedVertexInfo.verts[2], mPreviousSelectedVertexInfo.verts[2] , mPreviousSelectedVertexInfo.verts[0]}, std::vector<Vector3f>{ Vector3f(1.0, 0.0, 0.0) , Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0)}, MVPLine);
				mLineShader->DrawPoints(std::vector<Vector3f>{mPreviousSelectedVertexInfo.verts[mPreviousSelectedVertexInfo.vertexInTriangleIdx]}, std::vector<Vector3f>{ Vector3f(0.0, 0.0, 1.0)}, MVPLine);
			}			
			
			if (mSelectionActiveTgt)
			{
				Matrix4f cModelMatrix;
				mScenes[mCurrentSelectedVertexInfo.meshIdx]->GetModelForMesh(0, cModelMatrix);
				Matrix4f MVPLine = MVPMatrix*cModelMatrix;
				mLineShader->Draw(std::vector<Vector3f>{mCurrentSelectedVertexInfo.verts[0], mCurrentSelectedVertexInfo.verts[1], mCurrentSelectedVertexInfo.verts[1], mCurrentSelectedVertexInfo.verts[2], mCurrentSelectedVertexInfo.verts[2], mCurrentSelectedVertexInfo.verts[0]}, std::vector<Vector3f>{ Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 0.0, 0.0)}, MVPLine);
				mLineShader->DrawPoints(std::vector<Vector3f>{mCurrentSelectedVertexInfo.verts[mCurrentSelectedVertexInfo.vertexInTriangleIdx]}, std::vector<Vector3f>{ Vector3f(0.0, 0.0, 1.0)}, MVPLine);
			}
			
			if (mDrawRayDebug)
			{
				mLineShader->DrawRay(currentRay, MVPMatrix);
			}
		}
		glfwSwapBuffers((GLFWwindow*)mWindow->getWindowHandle());
		glfwPollEvents();
	}
}

void atRenderer::atKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int width, height; 
	glfwGetWindowSize(window, &width, &height);
	float aspectRatio = (float)width / (float)height;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		mCamera->HandleKeyPress(Utility::UP , aspectRatio);
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		mCamera->HandleKeyPress(Utility::DOWN, aspectRatio);
	}
	// Strafe right
	if (key == GLFW_KEY_RIGHT && mods == 0 && action == GLFW_PRESS) {
		mCamera->HandleKeyPress(Utility::ROTATE_RIGHT, aspectRatio);
	}
	// Strafe left
	if (key == GLFW_KEY_LEFT && mods == 0 && action == GLFW_PRESS) {
		mCamera->HandleKeyPress(Utility::ROTATE_LEFT, aspectRatio);
	}

	if (key == GLFW_KEY_RIGHT && mods == GLFW_MOD_ALT && action == GLFW_PRESS)
	{
		mCamera->HandleKeyPress(Utility::PAN_RIGHT, aspectRatio);
	}

	if (key == GLFW_KEY_LEFT && mods == GLFW_MOD_ALT && action == GLFW_PRESS)
	{
		mCamera->HandleKeyPress(Utility::PAN_LEFT, aspectRatio);
	}



	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		for each(auto scene in mScenes)
		{
			scene->SetRotationForMesh(0, 20);
		}		
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		for each(auto scene in mScenes)
		{
			scene->SetRotationForMesh(0, -20);
		}
	}
	//Wireframe
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		mWireFrame = (!mWireFrame);
	if (key == GLFW_KEY_S && mods == GLFW_MOD_CONTROL && action == GLFW_PRESS)
	{
		std::cout << "Saving Correspondence file " << std::endl;
		Utility::atCorrespondenceMarker writer; 
		writer.SaveCorrespondenceFile("../data/horse_camel.corres", markerData);
	}		
}

void atRenderer::atMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		Vector3f ray_origin;
		Vector3f ray_direction;

		Matrix4f viewMatrix; 
		Matrix4f projectionMatrix;
		mCamera->GetView(viewMatrix);
		mCamera->GetPerspective(projectionMatrix);

		Utility::ScreenPosToWorldRay(xpos, ypos,
			mWindow->mWidth, mWindow->mHeight,
			viewMatrix,
			projectionMatrix,
			ray_origin,
			ray_direction);
		mDrawRayDebug = true; 
		
		currentRay.rayOrigin = ray_origin;
		currentRay.rayDirection = ray_direction;

		
		if (meshIdxForSelection == 0)
		{
			if (mScenes[meshIdxForSelection]->GetIntersection(currentRay, mPreviousSelectedVertexInfo))
			{
				mSelectionActiveSrc = true;
				meshIdxForSelection = 1; 
				mPreviousSelectedVertexInfo.meshIdx = 0;
			}
			else
			{
				mSelectionActiveSrc = false;
				mSelectionActiveTgt = false;
			}
		}
		if (meshIdxForSelection == 1)
		{
			if (mScenes[meshIdxForSelection]->GetIntersection(currentRay, mCurrentSelectedVertexInfo))
			{
				mSelectionActiveTgt = true;
				meshIdxForSelection = 0; 
				mCurrentSelectedVertexInfo.meshIdx = 1;

				Utility::CorrespondenceMarkerData markerPair;
				markerPair.vertIdxSource = mPreviousSelectedVertexInfo.vertexListIdx;
				markerPair.vertIdxTarget = mCurrentSelectedVertexInfo.vertexListIdx;
				markerData.push_back(markerPair);
			}
			else
			{
				mSelectionActiveTgt = false;
			}
		}



		//for (int i = 0; i<mScenes.size(); i++) 
		//{

		//	float intersection_distance; // Output of TestRayOBBIntersection()
		//	if (mScenes[i]->GetIntersection(currentRay, mPreviousSelectedVertexInfo))
		//	{
		//		mSelectionActiveSrc = true;
		//		mPreviousSelectedVertexInfo.meshIdx = i; 
		//		//if (i == 0 ) //Source mesh selected.
		//		//{
		//		//	// vertex from same mesh selected again. 
		//		//	//Reject the selection. 
		//		//	std::cout << "Same mesh selected again, rejecting vertex! "<< std::endl;
		//		//	//mPreviousSelectedVertexInfo = mCurrentSelectedVertexInfo;
		//		//	mSelectionActiveSrc = true;
		//		//}
		//		//else
		//		//{
		//		//	Utility::CorrespondenceMarkerData markerPair; 
		//		//	markerPair.vertIdxSource = mPreviousSelectedVertexInfo.vertexListIdx;
		//		//	markerPair.vertIdxTarget = mCurrentSelectedVertexInfo.vertexListIdx;
		//		//	markerData.push_back(markerPair);

		//		//	mPreviousSelectedVertexInfo = mCurrentSelectedVertexInfo;
		//		//	mSelectionActive = true;
		//		//	std::cout << "Mouse Postion : " << xpos << " , " << ypos << std::endl;
		//		//	break;
		//		//}
		//		
		//	}
		//	else
		//		mSelectionActiveSrc = false; 
		//}

	}
}

void atRenderer::atWindowResizeCallback(GLFWwindow* window, int width, int height)
{
	int fwidth, fheight;
	glfwGetFramebufferSize(window, &fwidth, &fheight);
	glViewport(0, 0, fwidth, fheight);

	mCamera->SetPerspective(Utility::ToRadians(45.0f), width / height, 0.1f, 100.0f);
	//mCamera->SetLookAt(Vector3f(-3, 0.5, 0), Vector3f(0, 0.5, 0), Vector3f(0, 1, 0));
}
