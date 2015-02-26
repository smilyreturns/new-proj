#ifndef _SOLARSYSTEMAPP_H_
#define _SOLARSYSTEMAPP_H_

#include "Application.h"
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class SolarSystemApp : public Application
{
private:
	float m_cameraX;
	float m_cameraZ;
	float m_timer;
public:
	SolarSystemApp();
	~SolarSystemApp();

	int Startup();
	int Update();
	void Draw();
	void Shutdown();

	mat4 BuildOrbitMatrix(float local_rotation, float radius, float orbit_rotation);
};


#endif