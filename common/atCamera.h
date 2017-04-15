#pragma once
#include "atUtility.h"

namespace atRenderEngine
{
	class atCamera
	{
	public:
		atCamera();
		~atCamera();
		void SetLookAt(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up);
		void SetPerspective(float fovY, float aspect, float near, float far);	
		void GetPerspective(Matrix4f& outMatrix);
		void GetView(Matrix4f& outMatrix);
		void GetMVP(Eigen::Matrix4f& mvp);
		void HandleKeyPress(Utility::KeyPressCodes keyPressed , float aspectRatio);
		void HandleSelection(float mouseX, float mouseY, float screenWidth, float screenHeight);
	private:
		Vector3f mViewVector,mDirectionVector, mCameraVector, mUpVector, mTargetVector;
		Matrix4f mPerspectiveMatrix;
		Matrix4f mViewMatrix;
		
		Matrix4f mModelViewProjection;

		float mInitialFoV; 
		float mSpeed; 
		float mMouseSpeed; 
		float mLastTime;
		float mHorizontalAngle; 
		float mVerticalAngle;

	};

}
