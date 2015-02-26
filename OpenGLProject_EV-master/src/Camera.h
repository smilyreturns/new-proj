#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GLM_Header.h"

class Camera
{
public:
	vec3  m_pos;
	vec3  m_center;
	vec3  m_up;
	mat4  m_world;
	mat4  m_view;
	mat4  m_proj;
	mat4  m_projView;
	float m_fieldOfView;
	float m_aspect;
	float m_near;
	float m_far;
	float m_sensitivity;
	bool  m_mouseDown;

	float m_windowWidth;
	float m_windowHeight;

	Camera();
	Camera(vec3 a_pos, vec3 a_center, vec3 a_up, float a_fieldOfView, float a_aspect, float a_near, float a_far);
	Camera(vec3 a_pos, vec3 a_center, vec3 a_up, float a_fieldOfView, float a_width, float a_height, float a_near, float a_far);
	virtual ~Camera();
	virtual void update(float a_DeltaTime) = 0;
	void setPerspective(float a_fieldOfView, float a_aspect, float a_near, float a_far);
	void setPerspective(float a_fieldOfView, float a_width, float a_height, float a_near, float a_far);
	void setLookAt(vec3 a_from, vec3 a_to, vec3 a_up);
	void setPosition(vec3 a_pos);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjection();
	mat4 getProjView();
	void updateProjView();
};

class FlyCamera : public Camera
{
public:
	float m_moveSpeed;
	float m_fovSpeed;
	vec3  m_up;

	float m_yaw;
	float m_pitch;

	FlyCamera();
	FlyCamera(float a_fieldOfView, float a_aspect, float a_speed);
	~FlyCamera();
	virtual void update(float a_DeltaTime);
	void setMoveSpeed(float a_moveSpeed);
	void setSensitivity(float a_sensitivity);
	void setFOVSpeed(float a_fovSpeed);
};

#endif