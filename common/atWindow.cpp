#include "atWindow.h"
#include "atRenderer.h"
using namespace atRenderEngine;



atWindow::atWindow(atRenderer* renderer,std::string title, int width, int height)
	:mRenderer(renderer),mTitle(title), mWidth(width),mHeight(height)
{
	glfwSetErrorCallback(Utility::atErrorCallback);

	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!mWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	mRenderer = renderer;
	glfwSetWindowUserPointer((GLFWwindow*)getWindowHandle(), this);

#define genericCallback(functionName)\
        [](GLFWwindow* window, auto... args) {\
            auto pointer = static_cast<atWindow*>(glfwGetWindowUserPointer(window));\
            if (pointer->functionName) pointer->functionName(window, args...);\
        }

	glfwSetWindowCloseCallback((GLFWwindow*)getWindowHandle(), genericCallback(onClose));
	glfwSetMouseButtonCallback((GLFWwindow*)getWindowHandle(), genericCallback(onMouseClick));
	glfwSetKeyCallback((GLFWwindow*)getWindowHandle(), genericCallback(onKeyPressed));
	glfwSetWindowSizeCallback((GLFWwindow*)getWindowHandle(), genericCallback(onWindowResize));

	onMouseClick = [this](auto self, int button, int action, int mods)
	{
		//std::cout << "I'm such a rebel" << std::endl;
		this->mRenderer->atMouseCallback(self, button, action, mods);
	};
	onClose = [](auto self) 
	{
		std::cout << "I'm such a rebellion" << std::endl;
	};
	onKeyPressed = [this](auto window, int key, int scancode, int action, int mods)
	{
		this->mRenderer->atKeyCallback(window, key, scancode, action, mods);
		/*if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);*/
	};
	onWindowResize = [this](auto window, int width, int height)
	{
		this->mWidth = width; 
		this->mHeight = height;
		this->mRenderer->atWindowResizeCallback(window, width, height);
	};

}


atWindow::~atWindow()
{
	glfwDestroyWindow((GLFWwindow*)mWindow);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void * atWindow::getWindowHandle()
{
	return (mWindow) ? mWindow : NULL;
}
