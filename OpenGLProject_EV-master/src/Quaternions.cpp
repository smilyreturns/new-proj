#include "Quaternions.h"
#include "GLM_Header.h"
#include "GL_Header.h"

#include "Gizmos.h"

#include "Vertex.h"
#include "Utility.h"

#include "stb_image.h"

Quaternions::Quaternions()
{
}

bool Quaternions::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	Gizmos::create();

	loadShaders("./shaders/normal_mapped_vertex.glsl", "./shaders/normal_mapped_fragment.glsl", &m_program);
	
	m_hipFrames[0].position		= vec3(0, 5, 0);
	m_hipFrames[0].rotation		= glm::quat(vec3(-1, 0, 0));
	m_kneeFrames[0].position	= vec3(0, -2.5, 0);
	m_kneeFrames[0].rotation	= glm::quat(vec3(-1, 0, 0));
	m_ankleFrames[0].position	= vec3(0, -2.5, 0);
	m_ankleFrames[0].rotation	= glm::quat(vec3(-1, 0, 0));

	m_hipFrames[1].position		= vec3(0, 5, 0);
	m_hipFrames[1].rotation		= glm::quat(vec3(1, 0, 0));
	m_kneeFrames[1].position	= vec3(0, -2.5, 0);
	m_kneeFrames[1].rotation	= glm::quat(vec3(0, 0, 0));
	m_ankleFrames[1].position	= vec3(0, -2.5, 0);
	m_ankleFrames[1].rotation	= glm::quat(vec3(0, 0, 0));


	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	return true;
}

void Quaternions::shutdown()
{
	Application::shutdown();
}

bool Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	Gizmos::clear();

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

	m_flyCamera.m_windowWidth = m_windowWidth;
	m_flyCamera.m_windowHeight = m_windowHeight;
	m_flyCamera.update(m_deltaTime);

	m_timer += m_deltaTime;
	float sinWave = sinf(m_timer) * 0.5f + 0.5f;
	
	m_hipBone = EvaluateKeyFrames(m_hipFrames[0], m_hipFrames[1], sinWave);
	m_kneeBone = EvaluateKeyFrames(m_kneeFrames[0], m_kneeFrames[1], sinWave);
	m_ankleBone = EvaluateKeyFrames(m_ankleFrames[0], m_ankleFrames[1], sinWave);

	mat4 globalHip = m_hipBone;
	mat4 globalKnee = m_hipBone * m_kneeBone;
	mat4 globalAnkle = globalKnee * m_ankleBone;

	vec3 hipPos = globalHip[3].xyz;
	vec3 kneePos = globalKnee[3].xyz;
	vec3 anklePos = globalAnkle[3].xyz;

	Gizmos::addAABBFilled(hipPos, vec3(0.5f), vec4(1, 1, 0, 1), &globalHip);
	Gizmos::addAABBFilled(kneePos, vec3(0.5f), vec4(1, 1, 0, 1), &globalKnee);
	Gizmos::addAABBFilled(anklePos, vec3(0.5f), vec4(1, 1, 0, 1), &globalAnkle);

	Gizmos::addLine(hipPos, kneePos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(kneePos, anklePos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	return true;
}

mat4 Quaternions::EvaluateKeyFrames(KeyFrame a_start, KeyFrame a_end, float a_t)
{
	vec3 pos		= glm::mix(a_start.position, a_end.position, a_t);
	glm::quat rot	= glm::slerp(a_start.rotation, a_end.rotation, a_t);
	mat4 result		= glm::translate(pos) * glm::toMat4(rot);
	return result;
}

void Quaternions::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	unsigned int projViewHandle = glGetUniformLocation(m_program, "projView");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, GL_FALSE, (float*)&m_flyCamera.m_projView);
	}

	Gizmos::draw(m_flyCamera.m_projView);

	Application::draw();
}