#pragma once
#include "atWindow.h"
#include "atScene.h"
#include "atCamera.h"
#include "atLineShader.h"
#include "atCorrespondenceMarkerReadWrite.h"
#include <vector>

namespace atRenderEngine
{
	class atRenderer
	{
	public:
		atRenderer();
		~atRenderer();
		
		void Setup(std::string title, int width, int height);
		void AddCamera(atCamera* camera);
		void AddScene(atScene* scene);
		void Draw();
		void atKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
		void atMouseCallback(GLFWwindow * window, int button, int action, int mods);
		void atWindowResizeCallback(GLFWwindow * window, int width, int height);
	private:
		bool mWireFrame; 
		atWindow* mWindow;
		std::vector<atScene*> mScenes;
		atCamera* mCamera;
		Matrix4f rot120;
		Matrix4f rot20;

		bool mDrawRayDebug; 
		bool mSelectionActiveSrc;
		bool mSelectionActiveTgt;
		Utility::VertexSelectionResult mPreviousSelectedVertexInfo;
		Utility::VertexSelectionResult mCurrentSelectedVertexInfo;
		//int IsSourceBeingSelected;
		/*Vector3f mIntersectingTriangle[3];
		int closestVertIndex;*/
		std::vector<Utility::CorrespondenceMarkerData> markerData;
		int vertIdxForSource;
		int meshIdxForSelection; 
		Utility::RayData currentRay; 
		Utility::atLineShader* mLineShader; 
	};
}