#ifndef _INPUT__H
#define _INPUT__H

#include <Windows.h>
#include "Camera.h"

class Input
{
public:
	Input();
	~Input();
	void init(Camera* p_pCam);
	void update(LPARAM lParam);
private:
	RAWINPUTDEVICE m_rid[2];
	float m_mouseX;
	float m_mouseY;
	Camera* m_pCamera;

};



#endif