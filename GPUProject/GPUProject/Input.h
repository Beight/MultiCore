#pragma once

#include <Windows.h>
#include <memory>
#include "Camera.h"

class Input
{
public:
	Input();
	~Input();
	void init(std::shared_ptr<Camera> p_pCam);
	void update(LPARAM lParam);
private:
	RAWINPUTDEVICE m_rid[2];
	float m_mouseX;
	float m_mouseY;
	std::shared_ptr<Camera> m_pCamera;
};