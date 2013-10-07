#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include "stdafx.h" 
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"

struct Light
{
	XMVECTOR pos;
	XMVECTOR dir;
	XMVECTOR ambient;
	XMVECTOR diffuse;
	XMVECTOR spec;
	XMVECTOR att;
	float range;
};


struct Sphere
{
	XMVECTOR center;
	float radius;
	XMVECTOR color;
};

struct Triangle
{
	XMVECTOR pos0;
	XMVECTOR pos1;
	XMVECTOR pos2;
	XMVECTOR color;
};

struct ConstBuffer
{
	XMVECTOR cameraPos;
	XMMATRIX IP;
	XMMATRIX IV;
	Sphere sphere;
	Triangle triangle;
	Triangle triangle2;
	Light lightList[10];
};

class Direct3D
{
public:



	Direct3D(HWND p_hwnd);
	~Direct3D();

	void init(Input* p_pInput);
	void update(float dt);
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

	Sphere						m_sphere;
	Triangle					m_triangle;
	Triangle					m_triangle2;
	XMMATRIX					m_view;
	XMMATRIX					m_proj;
	XMMATRIX					m_IVP;

	const static int			m_nrLights = 10;
	Light						m_lightList[m_nrLights];

	void						release();
};


#endif