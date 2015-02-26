#ifndef _CAM_PROJ_H_
#define _CAM_PROJ_H_

#include "Application.h"
#include "Camera.h"

class CameraAndProjections : public Application
{
private:
	FlyCamera m_flyCamera;
public:
	CameraAndProjections();
	virtual bool startup();
	virtual bool update();
	virtual void draw();
	virtual void shutdown();
};


#endif //_CAM_PROJ_H_