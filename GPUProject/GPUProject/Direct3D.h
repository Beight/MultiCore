#ifndef _DIRECT3D__H
#define _DIRECT3D__H

#include <memory>
#include "D3DTimer.h"
#include "ComputeHelp.h"
#include "Input.h"
#include "Camera.h"
#include "Mesh.h"
#include "Structs.h"

class Direct3D
{
public:
	Direct3D(HWND p_hwnd);
	~Direct3D();

	void init(Input *p_pInput);
	void update(float dt);
	void draw();
	void createConstantBuffers();
	void updateConstantBuffers();
	void release();
private:
	HWND						m_hWnd;
	std::shared_ptr<Camera>		m_pCamera;													
	IDXGISwapChain				*m_SwapChain;
	ID3D11Device				*m_Device;
	ID3D11DeviceContext			*m_DeviceContext;
	ID3D11Buffer				*m_cBuffer;											
	ID3D11Buffer				*m_PrimaryCBuffer;											
	ID3D11Buffer				*m_IntersectionCBuffer;											
	ID3D11Buffer				*m_ColorCBuffer;			
	ID3D11Buffer				*m_FirstPassCBuffer;
	ID3D11UnorderedAccessView	*m_BackBufferUAV;  // compute output
	ID3D11ShaderResourceView	*m_meshTexture;												
	ComputeWrap					*m_ComputeSys;
	ComputeShader				*m_ComputeShader;
	ComputeShader				*m_PrimaryShader;
	ComputeShader				*m_IntersectionShader;
	ComputeShader				*m_ColorShader;
	int							m_Width;
	int							m_Height;
	D3DTimer					*m_Timer;
	Input						*m_pInput;
	float						m_fps;
	float						m_time;
	Sphere						m_sphere;
	Sphere						m_spherel0;
	Triangle				m_meshTri;
	Triangle					m_triangles[NROFTRIANGLES];	
	XMFLOAT4X4					m_view;
	XMFLOAT4X4					m_proj;
	XMFLOAT4X4					m_IVP;
	Light						m_lightList[NROFLIGHTS];
	Mesh						m_mesh;
	ComputeBuffer				*m_meshBuffer;
	ComputeBuffer				*m_materialBuffer;
	ComputeBuffer				*m_RayBuffer;
	ComputeBuffer				*m_HitDataBuffer;
	ComputeBuffer				*m_FinalColorBuffer;
	FirstPassConstBuffer		m_FirstPassStruct;


};


#endif