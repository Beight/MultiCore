#include "Direct3D.h"

Direct3D::Direct3D(HWND p_hwnd)
{
	m_hWnd			= p_hwnd;
	m_SwapChain		= nullptr;
	m_Device		= nullptr;
	m_DeviceContext = nullptr;
	m_BackBufferUAV = nullptr;
	m_Timer			= nullptr;
	m_ComputeSys	= nullptr;
	m_ComputeShader	= nullptr;
	
}

Direct3D::~Direct3D()
{
	release();
}

void Direct3D::init(Input* p_pInput)
{
	HRESULT hr = S_OK;;
	m_pInput = p_pInput;
	
	RECT rc;
	GetClientRect( m_hWnd, &rc );
	m_Width = rc.right - rc.left;
	m_Height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width =  m_Width;
	sd.BufferDesc.Height = m_Height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&m_SwapChain,
			&m_Device,
			&initiatedFeatureLevel,
			&m_DeviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"Direct3D 11.0 | Direct3D 11.0 device initiated with Direct3D 11 feature level"
				
			);
			SetWindowText(m_hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return;

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = m_Device->CreateUnorderedAccessView( pBackBuffer, NULL, &m_BackBufferUAV );
	if( FAILED(hr) )
		return;

	m_ComputeSys = new ComputeWrap(m_Device, m_DeviceContext);
	m_ComputeShader = m_ComputeSys->CreateComputeShader(_T("Shaders/BasicCompute.fx"), NULL, "main", NULL);
	m_Timer = new D3DTimer(m_Device, m_DeviceContext);

///////////////////////////////////////////////////////////////////////////////////////////
//Camera
///////////////////////////////////////////////////////////////////////////////////////////
	m_pCamera = new Camera();
	XMVECTOR cameraPos = XMVectorSet(0.0f,		0.0f,	 -20.0f, 0.0f);
	XMVECTOR cameraDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_pCamera->init(cameraPos, cameraUp, cameraDir, (float)m_Width, (float)m_Height);

	m_pInput->init(m_pCamera);


///////////////////////////////////////////////////////////////////////////////////////////
//Constant Buffer
///////////////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if(sizeof(ConstBuffer) % 16 > 0)
		bd.ByteWidth = ( int )(( sizeof( ConstBuffer ) / 16 )  + 1) * 16;
	else
		bd.ByteWidth = sizeof(ConstBuffer);

	bd.CPUAccessFlags = NULL;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.MiscFlags = 0;
	
	m_Device->CreateBuffer( &bd, NULL, &m_cBuffer);

///////////////////////////////////////////////////////////////////////////////////////////
//Sphere
///////////////////////////////////////////////////////////////////////////////////////////
	m_sphere.center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_sphere.radius = 2.0f;
	m_sphere.color = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

///////////////////////////////////////////////////////////////////////////////////////////
//Triangle
///////////////////////////////////////////////////////////////////////////////////////////
	
	
	//Back
	m_triangles[0].pos0 = XMVectorSet( 4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[0].pos1 = XMVectorSet(-4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[0].pos2 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	  															 
	m_triangles[1].pos0 = XMVectorSet( 4.0f*CUBESIZE,  -4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[1].pos1 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[1].pos2 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
																 
	//Left														 
	m_triangles[2].pos0 = XMVectorSet(-4.0f*CUBESIZE,  -4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[2].pos1 = XMVectorSet(-4.0f*CUBESIZE,  -4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	m_triangles[2].pos2 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
		  														 
	m_triangles[3].pos0 = XMVectorSet(-4.0f*CUBESIZE,  -4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[3].pos1 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	m_triangles[3].pos2 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
																 
	////Right													 
	m_triangles[4].pos0 = XMVectorSet( 4.0f*CUBESIZE,  -4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	m_triangles[4].pos1 = XMVectorSet( 4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[4].pos2 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	  															 
	m_triangles[5].pos0 = XMVectorSet( 4.0f*CUBESIZE,  -4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	m_triangles[5].pos1 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[5].pos2 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
																 
	////Top														 
	m_triangles[6].pos0 = XMVectorSet( 4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[6].pos1 = XMVectorSet(-4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[6].pos2 = XMVectorSet(-4.0f*CUBESIZE,	-4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	  															 
	m_triangles[7].pos0 = XMVectorSet( 4.0f*CUBESIZE,	-4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[7].pos1 = XMVectorSet( -4.0f*CUBESIZE,	-4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
	m_triangles[7].pos2 = XMVectorSet( 4.0f*CUBESIZE,	-4.0f*CUBESIZE,  -4.0f*CUBESIZE, 1.0f);
																 
	////Bottom													 
	m_triangles[8].pos0 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[8].pos1 = XMVectorSet(-4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[8].pos2 = XMVectorSet(4.0f*CUBESIZE,	4.0f*CUBESIZE, -4.0f*CUBESIZE, 1.0f);
	  															 
	m_triangles[9].pos0 = XMVectorSet( -4.0f*CUBESIZE,	4.0f*CUBESIZE,	 4.0f*CUBESIZE, 1.0f);
	m_triangles[9].pos1 = XMVectorSet( -4.0f*CUBESIZE,	4.0f*CUBESIZE, -4.0f*CUBESIZE, 1.0f);
	m_triangles[9].pos2 = XMVectorSet( 4.0f*CUBESIZE,	4.0f*CUBESIZE, -4.0f*CUBESIZE, 1.0f);

	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		m_triangles[i].color = XMVectorSet(0.0f,	1.0f,		0.0f, 1.0f);
	}
///////////////////////////////////////////////////////////////////////////////////////////
//Light
///////////////////////////////////////////////////////////////////////////////////////////

	//Staticly adding light pos
	m_lightList[0].pos = XMVectorSet(-10.0f,	-10.0f,	 -20.0f, 1.0f);
	m_lightList[1].pos = XMVectorSet(10.0f,		10.0f,	 -20.0f, 1.0f);
	m_lightList[2].pos = XMVectorSet(10.0f,	    -10.0f,	 -20.0f, 1.0f);

	//Spinning outside
	m_lightList[3].pos = XMVectorSet(-10.0f,		10.0f,	 -20.0f, 1.0f);
	//m_lightList[4].pos = XMVectorSet(20.0f,		10.0f,	-15.0f, 1.0f);
	//m_lightList[5].pos = XMVectorSet(25.0f,		5.0f,	-15.0f, 1.0f); 

	////Extra light inside
	//m_lightList[6].pos = XMVectorSet(30.0f,		0.0f,	-15.0f, 1.0f);
	//m_lightList[7].pos = XMVectorSet(-5.0f,		30.0f,	-15.0f, 1.0f);
	//m_lightList[8].pos = XMVectorSet(0.0f,		-30.0f, -15.0f, 1.0f);
	//m_lightList[9].pos = XMVectorSet(-30.0f,	0.0f,	-30.0f, 1.0f);

	for(int i = 0; i < NROFLIGHTS; i++)
	{
		m_lightList[i].ambient  = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f);
		m_lightList[i].diffuse  = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		m_lightList[i].spec		= XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		m_lightList[i].att		= XMVectorSet(0.0f, 0.1f, 0.0f, 0.0f);
		m_lightList[i].range	= 100.0f;
	}
	//ID3D11Debug* debug;
	//m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
	//debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void Direct3D::update(float dt)
{
	XMMATRIX rot;
	static float rotDT = 0.0f;
	rotDT = dt;
	rot = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotDT);
	for(int i = 0; i < NROFLIGHTS; i++)
	{
		//m_lightList[i].pos = XMVector4Transform(m_lightList[i].pos, rot);
	}
	m_pCamera->update();
	ConstBuffer cRayBufferStruct;
	cRayBufferStruct.cameraPos = m_pCamera->getPosition();
	cRayBufferStruct.IV = XMMatrixInverse(NULL, m_pCamera->getViewMat());
	cRayBufferStruct.IP = XMMatrixInverse(NULL, m_pCamera->getProjMat());
	cRayBufferStruct.IV = XMMatrixTranspose(cRayBufferStruct.IV);
	cRayBufferStruct.IP = XMMatrixTranspose(cRayBufferStruct.IP);
	cRayBufferStruct.sphere = m_sphere;
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		cRayBufferStruct.triangles[i] = m_triangles[i];
	}
	for(int i = 0; i < NROFLIGHTS; i++)
	{
		cRayBufferStruct.lightList[i] = m_lightList[i];
	}
	m_DeviceContext->UpdateSubresource(m_cBuffer, 0, NULL, &cRayBufferStruct, 0, 0);
	m_DeviceContext->CSSetConstantBuffers(0, 1, &m_cBuffer);


}

void Direct3D::draw()
{
	ID3D11UnorderedAccessView* uav[] = { m_BackBufferUAV };
	m_DeviceContext->CSSetUnorderedAccessViews(0, 1, uav, NULL);

	m_ComputeShader->Set();
	m_Timer->Start();
	m_DeviceContext->Dispatch( 25, 25, 1 );
	m_Timer->Stop();
	m_ComputeShader->Unset();

	uav[0] = NULL;
	m_DeviceContext->CSSetUnorderedAccessViews(0, 1, uav, NULL);


	if(FAILED(m_SwapChain->Present( 0, 0 )))
		return;

	float x = XMVectorGetX(m_pCamera->getPosition());
	float y = XMVectorGetY(m_pCamera->getPosition());
	float z = XMVectorGetZ(m_pCamera->getPosition());
	char title[256];
	sprintf_s(
		title,
		sizeof(title),
		"DirectCompute DEMO - Dispatch time: %f. CameraPos X: %f, Y: %f, Z: %f",
		m_Timer->GetTime(), x, y, z
	);
	SetWindowText(m_hWnd, title);
}

void Direct3D::release()
{
	SAFE_RELEASE(m_BackBufferUAV);
	SAFE_RELEASE(m_cBuffer);
	SAFE_RELEASE(m_SwapChain);
	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_DeviceContext);

	SAFE_DELETE(m_ComputeShader);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_ComputeSys);
}

