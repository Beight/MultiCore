#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include "stdafx.h" 
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"
#include "Mesh.h"

#define NROFTRIANGLES 10
#define NROFLIGHTS 10
#define CUBESIZE 16

struct Light
{
	XMVECTOR pos;
	XMVECTOR ambient;
	XMVECTOR diffuse;
	float range;
	XMFLOAT3 pad;
};


struct Sphere
{
	XMVECTOR center;
	XMVECTOR color;
	float radius;
	int		ID;
	XMFLOAT2 pad;
};

struct Triangle
{
	XMVECTOR	pos0;
	XMVECTOR	pos1;
	XMVECTOR	pos2;
	XMVECTOR	color;
	int			ID;
	XMFLOAT3	pad;
};



struct ConstBuffer
{
	XMVECTOR cameraPos;
	XMMATRIX IP;
	XMMATRIX IV;
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
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
	ID3D11ShaderResourceView*	m_meshTexture;												
	ComputeWrap*				m_ComputeSys;
	ComputeShader*				m_ComputeShader;
	int							m_Width;
	int							m_Height;
	D3DTimer*					m_Timer;
	Input*						m_pInput;
	float						m_fps;
	float						m_time;

	Sphere						m_sphere;
	MeshTriangle				m_meshTri;
	Triangle					m_triangles[NROFTRIANGLES];
	
	XMMATRIX					m_view;
	XMMATRIX					m_proj;
	XMMATRIX					m_IVP;

	Light						m_lightList[NROFLIGHTS];
	Mesh						m_mesh;
	ComputeBuffer*				m_meshBuffer;


	void						release();
};


#endif