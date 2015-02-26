#include "Camera.h"
#include "GL_Header.h"

#include "Application.h"

//Camera - Public:
Camera::Camera()
: m_pos(vec3(10, 10, 10))
, m_center(vec3(0))
, m_up(vec3(0, 1, 0))
, m_aspect(60.0f)
, m_near(0.1f)
, m_far(1000.0f)
, m_sensitivity(1.0f)
{
	m_view = glm::lookAt(m_pos, m_center, m_up);
	m_world = glm::inverse(m_view);
	m_proj = glm::perspective(glm::radians(m_fieldOfView), m_aspect, m_near, m_far);
	m_mouseDown = false;
	updateProjView();
}

Camera::Camera(vec3 a_pos, vec3 a_center, vec3 a_up, float a_fieldOfView, float a_aspect, float a_near, float a_far) 
	: m_pos(a_pos)
	, m_center(a_center)
	, m_up(a_up)
	, m_aspect(a_aspect)
	, m_fieldOfView(a_fieldOfView)
	, m_near(a_near)
	, m_far(a_far)
	, m_sensitivity(1.0f)
	, m_windowWidth(0.0f)
	, m_windowHeight(0.0f)
{
	m_view = glm::lookAt(a_pos, a_center, a_up);
	m_world = glm::inverse(m_view);
	m_proj = glm::perspective(glm::radians(a_fieldOfView), a_aspect, a_near, a_far);
	m_mouseDown = false;
	updateProjView();
}

Camera::Camera(vec3 a_pos, vec3 a_center, vec3 a_up, float a_fieldOfView, float a_width, float a_height, float a_near, float a_far)
: m_pos(a_pos)
, m_center(a_center)
, m_up(a_up)
, m_aspect(a_width / a_height)
, m_fieldOfView(a_fieldOfView)
, m_near(a_near)
, m_far(a_far)
, m_sensitivity(1.0f)
, m_windowWidth(a_width)
, m_windowHeight(a_height)
{
	m_view = glm::lookAt(a_pos, a_center, a_up);
	m_world = glm::inverse(m_view);
	m_proj = glm::perspective(glm::radians(a_fieldOfView), a_width / a_height, a_near, a_far);
	m_mouseDown = false;
	updateProjView();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float a_fieldOfView, float a_aspect, float a_near, float a_far)
{
	m_aspect = a_aspect;
	m_fieldOfView = a_fieldOfView;
	m_proj = glm::perspective(glm::radians(a_fieldOfView), a_aspect, a_near, a_far);
	updateProjView();
}

void Camera::setPerspective(float a_fieldOfView, float a_width, float a_height, float a_near, float a_far)
{
	m_aspect = a_width / a_height;
	m_fieldOfView = a_fieldOfView;
	m_proj = glm::perspective(glm::radians(a_fieldOfView), m_aspect, a_near, a_far);
	updateProjView();
}

void Camera::setLookAt(vec3 a_from, vec3 a_to, vec3 a_up)
{ 
	m_view = glm::lookAt(a_from, a_to, a_up);
	updateProjView();
}

void Camera::setPosition(vec3 a_pos)
{
	m_pos = a_pos;
}

mat4 Camera::getWorldTransform()
{
	return m_world;
}

mat4 Camera::getView()
{
	return m_view;
}

mat4 Camera::getProjection()
{
	return m_proj;
}

mat4 Camera::getProjView()
{
	return m_projView;
}

//Camera - Private:
void Camera::updateProjView()
{
	m_projView = m_proj * m_view;
}

//--- Camera types ---

//FlyCamera - Public:
FlyCamera::FlyCamera()
	: m_moveSpeed(60.0f)
	, m_fovSpeed(10.0f)
	, m_up(vec3(0, 1, 0))
	, m_yaw(0)
	, m_pitch(0)
	, Camera(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0), 60.0f, 1280, 720, 0.1f, 1000.0f)
{
}

FlyCamera::FlyCamera(float a_fieldOfView, float a_aspect, float a_moveSpeed) 
	: Camera(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0), a_fieldOfView, a_aspect, 0.1f, 1000.0f)
	, m_moveSpeed(a_moveSpeed)
	, m_fovSpeed(10.0f)
	, m_yaw(0)
	, m_pitch(0)
{
}

FlyCamera::~FlyCamera()
{
}

void FlyCamera::update(float a_DeltaTime)
{
	if (m_windowWidth == 0 || m_windowHeight == 0)
	{
		m_aspect = 1280.0f / 720.0f;
	}
	else
	{
		m_aspect = m_windowWidth / m_windowHeight;
	}

	GLFWwindow* curr_window = glfwGetCurrentContext();

	//GET SIZE OF SCREEN FOR WINDOWWIDTH AND WINDOWHEIGHT

	if (glfwGetKey(curr_window,
		GLFW_KEY_W) == GLFW_PRESS)
	{
		m_world[3] -= m_world[2] * m_moveSpeed * a_DeltaTime;
	}

	if (glfwGetKey(curr_window,
		GLFW_KEY_S) == GLFW_PRESS)
	{
		m_world[3] += m_world[2] * m_moveSpeed * a_DeltaTime;
	}
	if (glfwGetKey(curr_window,
		GLFW_KEY_A) == GLFW_PRESS)
	{
		m_world[3] -= m_world[0] * m_moveSpeed * a_DeltaTime;
	}
	if (glfwGetKey(curr_window,
		GLFW_KEY_D) == GLFW_PRESS)
	{
		m_world[3] += m_world[0] * m_moveSpeed * a_DeltaTime;
	}

	static double prevMouseX = 0, prevMouseY = 0;

	if (glfwGetMouseButton(curr_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && m_mouseDown == false)
	{
		m_mouseDown = true;
		glfwGetCursorPos(curr_window, &prevMouseX, &prevMouseY);

		glfwSetCursorPos(curr_window, m_windowWidth / 2.0f, m_windowHeight / 2.0f);
		glfwSetInputMode(curr_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else if (m_mouseDown == true)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(curr_window, &mouseX, &mouseY);
		glfwSetCursorPos(curr_window, m_windowWidth / 2.0f, m_windowHeight / 2.0f);

		mouseX -= (m_windowWidth / 2.0f);
		mouseY -= (m_windowHeight / 2.0f);

		mouseX /= (m_windowWidth / 2.0f);
		mouseY /= (m_windowHeight / 2.0f);

		mouseX *= -m_sensitivity;
		mouseY *= -m_sensitivity;

		m_yaw += mouseX;
		m_pitch += mouseY;

		if (m_pitch >= glm::radians(90.f))
		{
			m_pitch = glm::radians(90.f);
		}
		if (m_pitch <= glm::radians(-90.f))
		{
			m_pitch = glm::radians(-90.f);
		}

		mat4 yaw = glm::rotate(m_yaw, vec3(0, 1, 0));
		mat4 pitch = glm::rotate(m_pitch, vec3(1, 0, 0));

		mat4 transform = yaw * pitch;
		transform[3] = m_world[3];
		m_world = transform;
	}
	if (glfwGetMouseButton(curr_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && m_mouseDown == true)
	{
		m_mouseDown = false;
		glfwSetInputMode(curr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(curr_window, prevMouseX, prevMouseY);
	}

	if (glfwGetKey(curr_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_fieldOfView += m_fovSpeed * a_DeltaTime;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_fieldOfView -= m_fovSpeed * a_DeltaTime;
	}

	m_world[3][3] = 1;

	m_proj = glm::perspective(glm::radians(m_fieldOfView), m_aspect, m_near, m_far);
	m_view = glm::inverse(m_world);
	updateProjView();
}

void FlyCamera::setMoveSpeed(float a_moveSpeed)
{
	m_moveSpeed = a_moveSpeed;
}

void FlyCamera::setSensitivity(float a_sensitivity)
{
	m_sensitivity = a_sensitivity;
}

void FlyCamera::setFOVSpeed(float a_fovSpeed)
{
	m_fovSpeed = a_fovSpeed;
}