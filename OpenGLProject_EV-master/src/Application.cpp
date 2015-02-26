#include "Application.h"
#include "GL_Header.h"
#include <cstdio>

Application::Application() 
	: m_windowWidth(0)
	, m_windowHeight(0)
	, m_appName("")
{}

Application::~Application(){}

void Application::setDefault(float a_windowWidth, float a_windowHeight, char* a_appName, bool a_debugging)
{ 
	m_windowWidth = a_windowWidth;
	m_windowHeight = a_windowHeight;
	m_appName = a_appName;
	m_debugging = a_debugging;
}

bool Application::startup()
{
	//Checks if the application's attributes haven't been assigned 
	if (m_windowWidth == 0 || m_windowHeight == 0 || m_appName == "")
	{
		//Sets default values of the application
		m_windowWidth = 1280.0f;
		m_windowHeight = 700.0f;
		m_appName = "Default Project";
	}
	
	//Initialized GLFW and checks if it failed
	if (glfwInit() == false)
	{
		//Exits if GLFW failed to initialize
		return false;
	}

	//Creates a GLFW window 
	this->m_window = glfwCreateWindow((int)m_windowWidth, (int)m_windowHeight, m_appName, nullptr, nullptr);

	//Checks if the window was actually created
	if (this->m_window == nullptr)
	{
		//Exits if the window failed to get created
		return false;
	}

	//Sets the current context to the window that was just created
	glfwMakeContextCurrent(this->m_window);

	//Evaluates the graphics card driver for the OpenGL versions
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		//Unloads all of GLFW's created contents if the load failed
		glfwDestroyWindow(this->m_window);
		glfwTerminate();
		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	//Checks if debugging was set to true
	if (m_debugging == true)
	{
		GUI::create();
	}

	//Returns true if the Application startup succeeded
	return true;
}

void Application::shutdown()
{
	//Destroys GUI instance
	GUI::destroy();

	//Unloads all of GLFW resources
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Application::update()
{
	//Checks if the user is closing the window
	if(glfwWindowShouldClose(m_window) == true)
	{
		//Returns false if the window is to be closed
		return false;
	}

	//Sets up delta time
	m_deltaTime = (float)glfwGetTime();
	glfwSetTime(0.0f);

	//Updates GUI
	GUI::update(m_deltaTime);

	int width, height;
	//Refreshes the frame buffer and the viewport when the user resizes the window
	glfwGetWindowSize(m_window, &width, &height);
	glfwGetFramebufferSize(m_window, &width, &height);
	glViewport(0, 0, (int)m_windowWidth, (int)m_windowHeight);
	m_windowWidth = (float)width;
	m_windowHeight = (float)height;

	return true;
}

void Application::draw()
{
	GUI::draw();
	//Swaps the buffers for window draw buffer
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}