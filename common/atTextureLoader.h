#pragma once
#include "atUtility.h"
#include <GL/glew.h>

namespace atRenderEngine
{
	class atTextureLoader
	{
	public:
		atTextureLoader();
		~atTextureLoader();
		GLuint loadBMP_custom(const char * imagepath);
		GLuint loadDDS(const char * imagepath);
	};
}