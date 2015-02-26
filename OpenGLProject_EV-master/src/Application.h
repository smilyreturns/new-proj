#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "GLM_Header.h"

#include "GUI.h"

struct GLFWwindow;

class Application
{
protected:
	char* m_appName;
	float m_windowWidth;
	float m_windowHeight;
	float m_deltaTime;
	bool m_debugging;
public:
	GLFWwindow* m_window;

	Application();
	virtual ~Application();
	void setDefault(float a_windowWidth, float a_windowHeight, char* a_appName, bool m_debugging = false);

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
};

#endif //_APPLICATION_H_