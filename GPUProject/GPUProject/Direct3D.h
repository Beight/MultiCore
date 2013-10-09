#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include "stdafx.h" 
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"

#define NROFTRIANGLES 10
#define NROFLIGHTS 10
#define CUBESIZE 4

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
	XMVECTOR	pos0;
	XMVECTOR	pos1;
	XMVECTOR	pos2;
	XMVECTOR	color;
	int			ID;
};

struct ConstBuffer
{
	XMVECTOR cameraPos;
	XMMATRIX IP;
	XMMATRIX IV;
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
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
	Triangle					m_triangles[NROFTRIANGLES];
	
	XMMATRIX					m_view;
	XMMATRIX					m_proj;
	XMMATRIX					m_IVP;

	Light						m_lightList[NROFLIGHTS];

	void						release();
};


#endif