#include "CameraAndProjections.h"
#include "GL_Header.h"
#include "Gizmos.h"

CameraAndProjections::CameraAndProjections()
{
}

bool CameraAndProjections::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	Gizmos::create();
	glfwSetTime(0.0f);

	m_flyCamera = FlyCamera(60.0f, m_windowWidth / m_windowHeight, 10.0f);
	m_flyCamera.m_fieldOfView = 60.0f;
	m_flyCamera.m_aspect = m_windowWidth / m_windowHeight;
	m_flyCamera.setMoveSpeed(100.0f);
	m_flyCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_flyCamera.setFOVSpeed(100.0f);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	return true;
}



void CameraAndProjections::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}

bool CameraAndProjections::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float delta = (float)glfwGetTime();
	glfwSetTime(0.0f);

	//m_timer -= delta;
	//m_cameraX = sinf(m_timer) * 10;
	//m_cameraZ = cosf(m_timer) * 10;

	//m_view = glm::lookAt(vec3(m_cameraX, 10, m_cameraZ), vec3(0, 0, 0), vec3(0, 1, 0));
	//mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	Gizmos::addTransform(mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? blue : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? red : black);
	}

	//mat4 sun_transform = BuildOrbitMatrix(m_timer, 0, 0);
	//
	//mat4 planet_1 = sun_transform * BuildOrbitMatrix(m_timer * 5, 5, m_timer * 0.5f);
	//mat4 planet_2 = sun_transform * BuildOrbitMatrix(m_timer * 3, 3.5f, m_timer * 1.5f);
	//mat4 planet_3 = sun_transform * BuildOrbitMatrix(m_timer * 2, 6, m_timer * 0.25f);
	//mat4 planet_4 = sun_transform * BuildOrbitMatrix(m_timer * 15, 2, m_timer * 5.0f);

	//Gizmos::addSphere(sun_transform[3].xyz, 2.0f, 20, 20, red, &sun_transform);

	//Gizmos::addSphere(planet_1[3].xyz, 1.0f, 20, 20, red, &planet_1);
	//Gizmos::addSphere(planet_2[3].xyz, 0.5f, 10, 10, green, &planet_2);
	//Gizmos::addSphere(planet_3[3].xyz, 0.3f, 10, 10, blue, &planet_3);
	//Gizmos::addSphere(planet_4[3].xyz, 0.3f, 10, 10, blue, &planet_4);

	//for (int planet_index = 0;
	//	planet_index < 15;
	//	++planet_index)
	//{
	//	mat4 planet_max = sun_transform * BuildOrbitMatrix(m_timer / (float)planet_index,
	//		(5 + planet_index) / 2.0f,
	//		m_timer / (float)planet_index);
	//	Gizmos::addSphere(planet_max[3].xyz, 0.4f, 10, 10, blue, &planet_max);
	//}

	//mat4 translation_matrix = glm::translate(vec3(5, 0, 0));
	//mat4 transform = rotation * translation_matrix;
	//Gizmos::addAABBFilled(vec3(0, 5, 1), vec3(1, 1, 1), blue);
	//Gizmos::addAABB(vec3(0, 5, 1), vec3(1, 1, 1), blue);
	//Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, 1), vec3(-3, -2, 4), green);

	m_flyCamera.m_aspect = m_windowWidth / m_windowHeight;

	m_flyCamera.update(delta);

	return true;
}

void CameraAndProjections::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_flyCamera.getProjView());

	Application::draw();
}

