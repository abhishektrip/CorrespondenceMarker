#include "atCamera.h"
#include <Eigen/Geometry>
using namespace atRenderEngine;

atRenderEngine::atCamera::atCamera()
	:mInitialFoV(Utility::ToRadians(45.0f)),
	mLastTime(glfwGetTime()),
	mHorizontalAngle(Utility::ToRadians(180.0f)),
	mVerticalAngle(Utility::ToRadians(0.0f)),
	mSpeed(0.1f),
	mMouseSpeed(0.005f)
{
}

atRenderEngine::atCamera::~atCamera()
{
}

void atCamera::SetLookAt(const Eigen::Vector3f & position, const Eigen::Vector3f & target, const Eigen::Vector3f & up)
{
	mViewVector = position; 
	mTargetVector = target;
	mDirectionVector = target - position;
	Vector3f f = (target - position).normalized();
	Vector3f uBar = up.normalized();
	mUpVector = uBar;

	Vector3f s = (f.cross(up)).normalized();
	Vector3f u = s.cross(f);

	mViewMatrix << s.x(), s.y(), s.z(), -s.dot(position),
		u.x(), u.y(), u.z(), -u.dot(position),
		-f.x(), -f.y(), -f.z(), f.dot(position),
		0, 0, 0, 1;

}

void atCamera::SetPerspective(float fovY, float aspect, float near, float far)
{
	float theta = fovY*0.5;
	float range = far - near;
	float invtan = 1. / tan(theta);
	
	mPerspectiveMatrix = Matrix4f::Zero();
	mPerspectiveMatrix(0, 0) = invtan / aspect;
	mPerspectiveMatrix(1, 1) = invtan;
	mPerspectiveMatrix(2, 2) = -(near + far) / range;
	mPerspectiveMatrix(3, 2) = -1;
	mPerspectiveMatrix(2, 3) = -2 * near * far / range;
	mPerspectiveMatrix(3, 3) = 0;
}

void atRenderEngine::atCamera::GetPerspective(Matrix4f & outMatrix)
{
	outMatrix = Matrix4f(mPerspectiveMatrix);
}

void atRenderEngine::atCamera::GetView(Matrix4f & outMatrix)
{
	outMatrix = Matrix4f(mViewMatrix);
}

void atCamera::GetMVP(Eigen::Matrix4f & mvp)
{
	mvp = mPerspectiveMatrix*mViewMatrix;// *mModelMatrix;
}
void atCamera::HandleKeyPress(Utility::KeyPressCodes keyPressed , float aspectRatio)
{
	// Compute time difference between current and last frame
	float currentTime = glfwGetTime();
	float deltaTime = float(currentTime - mLastTime);
	//mDirectionVector  = Vector3f(
	//	cos(mVerticalAngle) * sin(mHorizontalAngle),
	//	sin(mVerticalAngle),
	//	cos(mVerticalAngle) * cos(mHorizontalAngle));

	mCameraVector = Vector3f(
		sin(mHorizontalAngle - 3.14f / 2.0f),
		0,
		cos(mVerticalAngle - 3.14f / 2.0f)
	);

	//mUpVector  = Vector3f(= mCameraVector.cross(mDirectionVector);

	switch (keyPressed)
	{
	case atRenderEngine::Utility::UP:
		mViewVector += mDirectionVector * deltaTime * mSpeed;
		break;
	case atRenderEngine::Utility::DOWN:
		mViewVector -= mDirectionVector * deltaTime * mSpeed;
		break;
	case atRenderEngine::Utility::ROTATE_RIGHT:
		mViewVector = Utility::rotateYByDegree(20)*mViewVector;// += mCameraVector * deltaTime * mSpeed;
		break;
	case atRenderEngine::Utility::ROTATE_LEFT:
		mViewVector = Utility::rotateYByDegree(-20)*mViewVector;// -= mCameraVector * deltaTime * mSpeed;
		break;
	case atRenderEngine::Utility::PAN_RIGHT:
		mViewVector += (mDirectionVector.cross(mUpVector)) *mSpeed ;
		mTargetVector += (mDirectionVector.cross(mUpVector))*mSpeed;
		break;
	case atRenderEngine::Utility::PAN_LEFT:
		mViewVector -= (mDirectionVector.cross(mUpVector)) *mSpeed;
		mTargetVector -= (mDirectionVector.cross(mUpVector)) *mSpeed;
		break;
	default:
		break;
	}
	SetPerspective(mInitialFoV, aspectRatio, 0.1f, 100.f);
	SetLookAt(mViewVector, mTargetVector, mUpVector);

	mLastTime = currentTime;
}
void atCamera::HandleSelection(float mouseX, float mouseY, float screenWidth, float screenHeight)
{


}