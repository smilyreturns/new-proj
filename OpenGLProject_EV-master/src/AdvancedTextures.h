#ifndef _ADVANCEDTEXTURES_H_
#define _ADVANCEDTEXTURES_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class AdvancedTextures : public Application
{
private:
	unsigned int m_program;
public:
	FlyCamera m_flyCamera;
	OpenGLData m_quad;
	unsigned int m_diffuseTexture;
	unsigned int m_normalTexture;
	unsigned int m_specularTexture;

	vec3 m_lightDir;
	vec3 m_lightColor;
	vec3 m_ambientLight;
	float m_specularPower;

	AdvancedTextures();
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	void loadTextures();
	void generateQuat(float a_size);
};

#endif