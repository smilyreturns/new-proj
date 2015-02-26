#include "Animation.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

Application* g_Application;

bool runApp();
void onWindowResize(GLFWwindow* window, int width, int height);

int main()
{
	Animation App;
	g_Application = &App;
	App.setDefault(1280, 720, "Manny's Project", true);
	if (App.startup() == false) //Checks for error messages (< 0)
	{
		return -1;
	}
	glfwSetWindowSizeCallback(App.m_window, onWindowResize);
	while (App.update() == true)
	{
		App.draw();
	}
	App.shutdown();
	return 0;
}

bool runApp()
{
	if (g_Application->update() == false)
	{
		return false;
	}
	g_Application->draw();
	return true;
}

void onWindowResize(GLFWwindow* window, int width, int height)
{
	runApp();
}

