#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

struct KeyFrame
{
	vec3 position;
	glm::quat rotation;
};

class Quaternions : public Application
{
private:
	unsigned int m_program;
public:
	FlyCamera m_flyCamera;
	float m_timer;

	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	mat4 m_hipBone;
	mat4 m_kneeBone;
	mat4 m_ankleBone;

	Quaternions();
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	mat4 Quaternions::EvaluateKeyFrames(KeyFrame a_start, KeyFrame a_end, float a_t);
};

#endif