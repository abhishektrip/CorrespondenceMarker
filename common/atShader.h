#pragma once
#include "atUtility.h"

namespace atRenderEngine
{
	namespace Utility
	{
		class atShader
		{
		public:
			atShader();
			virtual ~atShader() = 0;
			GLuint mProgramId;
		};
	}
}
