#pragma once
#include <GL/glew.h>
#include <glfw3.h>
#include <string>
#include "atUtility.h"
//#include "atRenderer.h"
//#include "atCamera.h"


namespace atRenderEngine
{
	class atRenderer; 
	class atWindow
	{
	public:
		//void atKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
		//void atMouseCallback(GLFWwindow * window, int button, int action, int mods);
		std::function<void(GLFWwindow*)> onClose;
		std::function<void(GLFWwindow*, int, int, int)> onMouseClick;// = [](auto window, int, int, int);
		std::function<void(GLFWwindow*, int, int, int, int)> onKeyPressed;
		std::function<void(GLFWwindow*, int, int)> onWindowResize;
		/*= [this](auto window, int key, int scancode, int action, int mods)
		{
			this->atKeyCallback(window, key, scancode, action, mods);
		};*/
		atWindow(atRenderer* renderer,const std::string title, int width, int height);
		~atWindow();
		void* getWindowHandle();
		int mHeight;
		int mWidth;
		std::string mTitle;
	private:
		atRenderer* mRenderer;
		GLFWwindow* mWindow;

	};
}