#pragma once
#include "atShader.h"

namespace atRenderEngine
{
	namespace Utility
	{
		class atLineShader :
			public atShader
		{
		public:
			atLineShader();
			~atLineShader();
			void Draw(std::vector<Vector3f>& vertexArray, std::vector<Vector3f>& colorArray, Matrix4f& MVPMatrix);
			void DrawPoints(std::vector<Vector3f>& vertexArray, std::vector<Vector3f>& colorArray, Matrix4f & MVPMatrix);			
			void DrawRay(RayData& ray, Matrix4f& MVP);
		};
	}
}
