#include "Input.h"


Input::Input() :
	m_pCamera(nullptr),
	m_mouseX(0.f),
	m_mouseY(0.f)
{

}

Input::~Input()
{

}

void Input::init(std::shared_ptr<Camera> p_pCam)
{
	m_pCamera = p_pCam;
	//Mouse
	m_rid[0].usUsagePage = 1;
	m_rid[0].usUsage = 2;
	m_rid[0].dwFlags = 0;
	m_rid[0].hwndTarget = NULL;

	//Keyboard
	m_rid[1].usUsagePage = 1;
	m_rid[1].usUsage = 6;
	m_rid[1].dwFlags = 0;
	m_rid[1].hwndTarget = NULL;

	if(RegisterRawInputDevices(m_rid, 2, sizeof(m_rid[0])) == false)
	{
		MessageBox(0, "Error Creating input device", "", MB_OK);
		return;
	}
}

void Input::update(LPARAM lParam)
{
	UINT dwSize;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if(lpb == NULL)
	{
		return;
	}
	if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		MessageBox(0, "Wrong size raw input", "", MB_OK);

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if(raw->header.dwType == RIM_TYPEMOUSE)
	{
		raw->data.mouse.usFlags = MOUSE_MOVE_RELATIVE;
		m_mouseX = (float)raw->data.mouse.lLastX;
		m_mouseY = (float)raw->data.mouse.lLastY;
		m_pCamera->moveDirection(-m_mouseX, m_mouseY);
	}
	delete raw;
}

