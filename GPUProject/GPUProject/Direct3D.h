#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include "stdafx.h" 
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"

struct ConstBuffer
{
	XMVECTOR m_cameraPos;
	XMMATRIX m_IP;
	XMMATRIX m_IV;
};

class Direct3D
{
public:



	Direct3D(HWND p_hwnd);
	~Direct3D();

	void init(Input* p_pInput);
	void update();
	void draw();

private:
	HWND						m_hWnd;

	Camera*						m_pCamera;													
	IDXGISwapChain*				m_SwapChain;
	ID3D11Device*				m_Device;
	ID3D11DeviceContext*		m_DeviceContext;
	ID3D11Buffer*				m_cBuffer;											
	ID3D11UnorderedAccessView*  m_BackBufferUAV;  // compute output
													
	ComputeWrap*				m_ComputeSys;
	ComputeShader*				m_ComputeShader;
	int							m_Width;
	int							m_Height;													
	D3DTimer*					m_Timer;
	Input*						m_pInput;

	XMMATRIX					m_view;
	XMMATRIX					m_proj;
	XMMATRIX					m_IVP;
};


#endif