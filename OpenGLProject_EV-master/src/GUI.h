#ifndef _GUI_H_
#define _GUI_H_

#include <AntTweakBar.h>
#include <map>

struct GLFWwindow;

class GUI
{
private:
	std::map<char*, TwBar*> m_tweakBars;
public:
	GLFWwindow* m_window;
	static GUI* m_instance;

	GUI();
	static void create();
	static void update(float a_deltaTime);
	static void draw();
	static void	destroy();

	static void createNewBar(char* a_barTitle);
	static TwBar* getBar(char* a_barTitle);

	//AntTweakBar GLFW event callbacks
	static void onCursorPos(GLFWwindow* a_window, double a_mouseX, double a_mouseY);
	static void onMouseButton(GLFWwindow* a_window, int a_button, int a_action, int a_mods);
	static void onKey(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
	static void onWindowResize(GLFWwindow* a_window, int a_width, int a_height);
	static void onScroll(GLFWwindow* a_window, double a_xScroll, double a_yScroll);
	static void onChar(GLFWwindow* a_window, unsigned int a_char);
};

#endif