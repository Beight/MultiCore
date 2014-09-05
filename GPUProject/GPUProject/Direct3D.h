#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include "stdafx.h"
#include <memory>
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"
#include "Mesh.h"

#define NROFTRIANGLES 12
#define NROFLIGHTS 10
#define CUBESIZE 16

struct Light
{
	XMFLOAT4 pos;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float range;
	XMFLOAT3 pad;
};


struct Sphere
{
	XMFLOAT4 center;
	XMFLOAT4 color;
	float radius;
	int		ID;
	XMFLOAT2 pad;
};

struct Triangle
{
	XMFLOAT4	pos0;
	XMFLOAT4	pos1;
	XMFLOAT4	pos2;
	XMFLOAT4	color;
	XMFLOAT4	normal;
	int			ID;
	XMFLOAT3	pad;
};



struct ConstBuffer
{
	XMFLOAT4 cameraPos;
	XMFLOAT4X4 IP;
	XMFLOAT4X4 IV;
	Sphere sphere;
	Sphere spherel0;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
	XMFLOAT3 pad;
};

class Direct3D
{
public:
	Direct3D(HWND p_hwnd);
	~Direct3D();

	void init(Input *p_pInput);
	void update(float dt);
	void draw();
	void release();
private:
	HWND						m_hWnd;
	std::shared_ptr<Camera>		m_pCamera;													
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
	Sphere						m_spherel0;
	MeshTriangle				m_meshTri;
	Triangle					m_triangles[NROFTRIANGLES];	
	XMFLOAT4X4					m_view;
	XMFLOAT4X4					m_proj;
	XMFLOAT4X4					m_IVP;
	Light						m_lightList[NROFLIGHTS];
	Mesh						m_mesh;
	ComputeBuffer				*m_meshBuffer, *m_materialBuffer;



};


#endif