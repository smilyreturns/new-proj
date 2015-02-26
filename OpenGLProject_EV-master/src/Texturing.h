#ifndef _TEXTURING_H_
#define _TEXTURING_H_

#include "Application.h"
#include "Camera.h"

class Texturing : public Application
{
private:
	FlyCamera m_flyCamera;
	unsigned int m_program;
	unsigned int m_indexCount;
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_texture;

	float m_time;
	float m_waveSpeed;
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	bool loadTexture(const char* a_filename);
	void generateQuat(float a_size);
};

#endif