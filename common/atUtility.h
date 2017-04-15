#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glfw3.h>
#include <Eigen/Core>
#include <Eigen/LU>
using namespace Eigen;


namespace atRenderEngine
{
	namespace Utility
	{
		const double PI = 3.14159265;
		struct RayData
		{
			Vector3f rayOrigin; 
			Vector3f rayDirection; 			
		};
		struct TriangleData
		{
			int vertexIdx[3];
			int normalIdx[3];
			int uvIdx[3];
		};

		struct BoundingRect
		{
			Vector3f min;
			Vector3f max;
		};

		struct VertexSelectionResult
		{
			int meshIdx;
			Vector3f verts[3];
			int triangleListIdx; 
			int vertexListIdx; 
			int vertexInTriangleIdx;
		};

		static float ToRadians(float degree)
		{
			return (degree * PI) / 180.0f;
		}
		enum KeyPressCodes
		{
			UP = 0x00025,
			DOWN,
			ROTATE_RIGHT,
			ROTATE_LEFT,
			PAN_RIGHT,
			PAN_LEFT
		};

		enum BoundingBoxContains
		{
			BB_FULLY_WITHIN,
			BB_PARTIALLY_WITHIN,
			BB_NOT_WITHIN,
		};
		static Matrix4f rotX120 = [] {
			Eigen::Matrix4f tmp;
			tmp = Eigen::Matrix4f::Identity();
			tmp(1, 1) = cos(Utility::ToRadians(120));
			tmp(1, 2) = -sin(Utility::ToRadians(120));
			tmp(2, 1) = sin(Utility::ToRadians(120));
			tmp(2, 2) = cos(Utility::ToRadians(120));
			return tmp;
		}();
		static Matrix4f rotY120 = [] {
			Eigen::Matrix4f tmp;
			tmp = Eigen::Matrix4f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(120));
			tmp(0, 2) = sin(Utility::ToRadians(120));
			tmp(2, 0) = -sin(Utility::ToRadians(120));
			tmp(2, 2) = cos(Utility::ToRadians(120));
			return tmp;
		}();
		static Matrix4f rotZ120 = [] {
			Eigen::Matrix4f tmp;
			tmp = Eigen::Matrix4f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(-120));
			tmp(0, 1) = -sin(Utility::ToRadians(-120));
			tmp(1, 0) = sin(Utility::ToRadians(-120));
			tmp(1, 1) = cos(Utility::ToRadians(-120));
			return tmp;
		}();
		static Matrix4f rotZ240 = [] {
			Eigen::Matrix4f tmp;
			tmp = Eigen::Matrix4f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(-240));
			tmp(0, 1) = -sin(Utility::ToRadians(-240));
			tmp(1, 0) = sin(Utility::ToRadians(-240));
			tmp(1, 1) = cos(Utility::ToRadians(-240));
			return tmp;
		}();
		static Matrix3f rotateZByDegree(float degree)
		{
			if (degree == 0.0f)
				return Eigen::Matrix3f::Identity();
			Eigen::Matrix3f tmp;
			tmp = Eigen::Matrix3f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(degree));
			tmp(0, 1) = -sin(Utility::ToRadians(degree));
			tmp(1, 0) = sin(Utility::ToRadians(degree));
			tmp(1, 1) = cos(Utility::ToRadians(degree));
			return tmp;
		}
		static Matrix3f rotateYByDegree(float degree)
		{
			if (degree == 0.0f)
				return Eigen::Matrix3f::Identity();
			Eigen::Matrix3f tmp;
			tmp = Eigen::Matrix3f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(degree));
			tmp(0, 2) = sin(Utility::ToRadians(degree));
			tmp(2, 0) = -sin(Utility::ToRadians(degree));
			tmp(2, 2) = cos(Utility::ToRadians(degree));
			return tmp;
		}
		static void rotateYByDegree(float degree, Matrix4f& transMatrix)
		{
			if (degree == 0.0f)
				return;			
			Eigen::Matrix4f tmp;
			tmp = Eigen::Matrix4f::Identity();
			tmp(0, 0) = cos(Utility::ToRadians(degree));
			tmp(0, 2) = sin(Utility::ToRadians(degree));
			tmp(2, 0) = -sin(Utility::ToRadians(degree));
			tmp(2, 2) = cos(Utility::ToRadians(degree));

			transMatrix = tmp + transMatrix;
/*			transMatrix(0, 0) += cos(Utility::ToRadians(degree));
			transMatrix(0, 2) += sin(Utility::ToRadians(degree));
			transMatrix(2, 0) += -sin(Utility::ToRadians(degree));
			transMatrix(2, 2) += cos(Utility::ToRadians(degree));*/			
		}

		static Matrix4f eulerAngleYXZ
		(
			float const & yaw,
			float const & pitch,
			float const & roll
		)
		{
			float tmp_ch = cos(Utility::ToRadians(yaw));
			float tmp_sh = sin(Utility::ToRadians(yaw));
			float tmp_cp = cos(Utility::ToRadians(pitch));
			float tmp_sp = sin(Utility::ToRadians(pitch));
			float tmp_cb = cos(Utility::ToRadians(roll));
			float tmp_sb = sin(Utility::ToRadians(roll));

			Matrix4f Result = Matrix4f::Identity();
			Result(0,0) = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
			Result(0,1) = tmp_sb * tmp_cp;
			Result(0,2) = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
			//Result(0,3) = static_cast<T>(0);
			Result(1,0) = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
			Result(1,1) = tmp_cb * tmp_cp;
			Result(1,2) = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
			//Result(1,3) = static_cast<T>(0);
			Result(2,0) = tmp_sh * tmp_cp;
			Result(2,1) = -tmp_sp;
			Result(2,2) = tmp_ch * tmp_cp;
			//Result(2,3) = static_cast<T>(0);

			return Result;
		}

		GLuint atLoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath);
		void atErrorCallback(int error, const char* description);
		float SquaredDistance(const Vector3f& a, const Vector3f& b);
		float SquaredDistanceRect(const BoundingRect *rect, const Vector3f& x0);
		Utility::BoundingBoxContains IsWithinBoundingBox(const BoundingRect & rect, const Vector3f v[3], int cd);
		BoundingBoxContains IsWithinBoundingBox(const BoundingRect& rect, const Vector3f v[3]);
		void ScreenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight, Matrix4f ViewMatrix, Matrix4f ProjectionMatrix, Vector3f & out_origin, Vector3f & out_direction);
		bool TestRayOBBIntersection(Vector3f ray_origin, Vector3f ray_direction, Vector3f aabb_min, Vector3f aabb_max, Matrix4f ModelMatrix, float & intersection_distance);
		bool RayTriangleIntersection(Utility::RayData ray, Vector3f v1, Vector3f v2, Vector3f v3, float & result);
		//void DrawRay(RayData & ray, Matrix4f& MVP);
	}
}