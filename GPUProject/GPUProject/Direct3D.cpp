#include "Direct3D.h"

Direct3D::Direct3D(HWND p_hwnd)
	: m_hWnd(p_hwnd),
	m_SwapChain(nullptr),
	m_Device(nullptr),
	m_DeviceContext(nullptr),
	m_BackBufferUAV(nullptr),
	m_Timer(nullptr),
	m_ComputeSys(nullptr),
	m_ComputeShader(nullptr),
	m_PrimaryShader(nullptr),
	m_IntersectionShader(nullptr),
	m_ColorShader(nullptr),
	m_FinalColorBuffer(nullptr),
	m_RayBuffer(nullptr),
	m_HitDataBuffer(nullptr),
	m_time(0.f),
	m_fps(0.f),
	m_mesh(Mesh()),
	m_meshBuffer(nullptr),
	m_materialBuffer(nullptr),
	m_cBuffer(nullptr),
	m_PrimaryCBuffer(nullptr),		
	m_IntersectionCBuffer(nullptr),	
	m_ColorCBuffer(nullptr),
	m_FirstPassCBuffer(nullptr),
	m_Height(0),
	m_Width(0),
	m_IVP(XMFLOAT4X4()),
	m_lightList(),
	m_meshTexture(nullptr),
	m_meshTri(Triangle()),
	m_pCamera(nullptr),
	m_pInput(nullptr),
	m_sphere(Sphere()),
	m_spherel0(Sphere()),
	m_triangles(),
	m_view(XMFLOAT4X4()),
	m_proj(XMFLOAT4X4()),
	m_FirstPassStruct()
{}

Direct3D::~Direct3D()
{
}

void Direct3D::init(Input *p_pInput)
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

	//Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return;

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = m_Device->CreateUnorderedAccessView( pBackBuffer, NULL, &m_BackBufferUAV );
	if( FAILED(hr) )
		return;

	SAFE_RELEASE(pBackBuffer);
	m_ComputeSys = new ComputeWrap(m_Device, m_DeviceContext);
	m_Timer = new D3DTimer(m_Device, m_DeviceContext);

	m_PrimaryShader = m_ComputeSys->CreateComputeShader(_T("Shaders/PrimaryRayStage.fx"), NULL, "main", NULL);
	m_IntersectionShader = m_ComputeSys->CreateComputeShader(_T("Shaders/IntersectionStage.fx"), NULL, "main", NULL);
	m_ColorShader = m_ComputeSys->CreateComputeShader(_T("Shaders/ColorStage.fx"), NULL, "main", NULL);

	m_RayBuffer = m_ComputeSys->CreateBuffer( STRUCTURED_BUFFER, sizeof(Ray), m_Width*m_Height, true, true, nullptr, true, "Structured Buffer: RayBuffer");
	m_HitDataBuffer = m_ComputeSys->CreateBuffer( STRUCTURED_BUFFER, sizeof(HitData), m_Width*m_Height, true, true, nullptr, true, "Structured Buffer: HitDataBuffer");
	m_FinalColorBuffer = m_ComputeSys->CreateBuffer(STRUCTURED_BUFFER, sizeof(XMFLOAT4), m_Height*m_Width, true, true, nullptr,true, "Structured Buffer:FinalColorBuffer");



///////////////////////////////////////////////////////////////////////////////////////////
//Camera
///////////////////////////////////////////////////////////////////////////////////////////
	m_pCamera = std::shared_ptr<Camera>(new Camera);
	XMVECTOR cameraPos = XMVectorSet(0.f, 0.f, -10.f, 0.f);
	XMVECTOR cameraDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	XMVECTOR cameraUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_pCamera->init(cameraPos, cameraUp, cameraDir, (float)m_Width, (float)m_Height);

	m_pInput->init(m_pCamera);


///////////////////////////////////////////////////////////////////////////////////////////
//Constant Buffer
///////////////////////////////////////////////////////////////////////////////////////////
	createConstantBuffers();


///////////////////////////////////////////////////////////////////////////////////////////
//Sphere
///////////////////////////////////////////////////////////////////////////////////////////
	m_sphere.center = XMFLOAT4(10.f, 0.f, 0.f, 1.f);
	m_sphere.radius = 2.f;
	m_sphere.color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	m_sphere.pad = XMFLOAT2(0.f, 0.f);
	m_sphere.ID = -2;

#pragma region Cube

///////////////////////////////////////////////////////////////////////////////////////////
//Cube
///////////////////////////////////////////////////////////////////////////////////////////
	
	
	//Back
	m_triangles[0].pos0 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[0].pos1 = XMFLOAT4(-1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[0].pos2 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[0].normal = XMFLOAT4(0.f, 0.f, -1.f, 0.f);

	m_triangles[1].pos0 = XMFLOAT4( 1.f*CUBESIZE,  -1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[1].pos1 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[1].pos2 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[1].normal = XMFLOAT4(0.f, 0.f, -1.f, 0.f);
																 		 
	//Left														 		 
	m_triangles[2].pos0 = XMFLOAT4(-1.f*CUBESIZE,  -1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[2].pos1 = XMFLOAT4(-1.f*CUBESIZE,  -1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[2].pos2 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[2].normal = XMFLOAT4(1.f, 0.f, 0.f, 0.f);

	m_triangles[3].pos0 = XMFLOAT4(-1.f*CUBESIZE,  -1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[3].pos1 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[3].pos2 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[3].normal = XMFLOAT4(1.f, 0.f, 0.f, 0.f);
																 		 
	////Right													 		 
	m_triangles[4].pos0 = XMFLOAT4( 1.f*CUBESIZE,  -1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[4].pos1 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[4].pos2 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[4].normal = XMFLOAT4(-1.f, 0.f, 0.f, 0.f);
	  															 		 
	m_triangles[5].pos0 = XMFLOAT4( 1.f*CUBESIZE,  -1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[5].pos1 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[5].pos2 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[5].normal = XMFLOAT4(-1.f, 0.f, 0.f, 0.f);
																 		 
	////Top														 		 
	m_triangles[6].pos0 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[6].pos1 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[6].pos2 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[6].normal = XMFLOAT4(0.f, -1.f, 0.f, 0.f);
	  															 		 
	m_triangles[7].pos0 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[7].pos1 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[7].pos2 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,  -1.f*CUBESIZE, 1.f);
	m_triangles[7].normal = XMFLOAT4(0.f, -1.f, 0.f, 0.f);
																 		 
	////Bottom													 		 
	m_triangles[8].pos1 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[8].pos0 = XMFLOAT4(-1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[8].pos2 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE, -1.f*CUBESIZE, 1.f);
	m_triangles[8].normal = XMFLOAT4(0.f, 1.f, 0.f, 0.f);
	  														 			 
	m_triangles[9].pos1 = XMFLOAT4(-1.f*CUBESIZE,	-1.f*CUBESIZE,	 1.f*CUBESIZE, 1.f);
	m_triangles[9].pos0 = XMFLOAT4(-1.f*CUBESIZE,	-1.f*CUBESIZE, -1.f*CUBESIZE, 1.f);
	m_triangles[9].pos2 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE, -1.f*CUBESIZE, 1.f);
	m_triangles[9].normal = XMFLOAT4(0.f, 1.f, 0.f, 0.f);

	//Front
	m_triangles[10].pos0 = XMFLOAT4( 1.f*CUBESIZE,	-1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[10].pos1 = XMFLOAT4(-1.f*CUBESIZE,	-1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[10].pos2 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[10].normal = XMFLOAT4(0.f, 0.f, 1.f, 0.f);

	m_triangles[11].pos0 = XMFLOAT4( 1.f*CUBESIZE,  -1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[11].pos1 = XMFLOAT4(-1.f*CUBESIZE,	1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[11].pos2 = XMFLOAT4( 1.f*CUBESIZE,	1.f*CUBESIZE,	 -1.f*CUBESIZE, 1.f);
	m_triangles[11].normal = XMFLOAT4(0.f, 0.f, 1.f, 0.f);

	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		m_triangles[i].color = XMFLOAT4(0.f, 0.5f, 0.5f, 1.f);
		m_triangles[i].ID = i;
		m_triangles[i].pad = 0.f;
		m_triangles[i].textureCoordinate0 = XMFLOAT2(0.f, 0.f);
		m_triangles[i].textureCoordinate1 = XMFLOAT2(0.f, 0.f);
		m_triangles[i].textureCoordinate2 = XMFLOAT2(0.f, 0.f);
	}

#pragma endregion


///////////////////////////////////////////////////////////////////////////////////////////
//Light
///////////////////////////////////////////////////////////////////////////////////////////

	//Staticly adding light pos
	m_lightList[0].pos = XMFLOAT4(0.f,	0.f, -2000.f, 1.f);
	m_lightList[1].pos = XMFLOAT4(10.f, 10.f, -20.f, 1.f);
	m_lightList[2].pos = XMFLOAT4(10.f, -10.f, -20.f, 1.f);

	//Spinning outside
	m_lightList[3].pos = XMFLOAT4(-10.f, 10.f, -20.f, 1.f);
	m_lightList[4].pos = XMFLOAT4(0.f,	-5.f, -50.f, 1.f);
	m_lightList[5].pos = XMFLOAT4(25.f, 5.f, -50.f, 1.f); 

	//Extra light inside
	m_lightList[6].pos = XMFLOAT4(30.f, 0.f, -50.f, 1.f);
	m_lightList[7].pos = XMFLOAT4(-5.f, 30.f,	-50.f, 1.f);
	m_lightList[8].pos = XMFLOAT4(0.f,	-30.f, -50.f, 1.f);
	m_lightList[9].pos = XMFLOAT4(-30.f, 0.f,	-50.f, 1.f);

	for(int i = 0; i < NROFLIGHTS; i++)
	{
		m_lightList[i].ambient  = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.f);
		m_lightList[i].diffuse  = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.f);
		m_lightList[i].range	= 75.f;
		m_lightList[i].pad		= XMFLOAT3(0.f, 0.f, 0.f);
	}

	
///////////////////////////////////////////////////////////////////////////////////////////
//Mesh
///////////////////////////////////////////////////////////////////////////////////////////
	m_mesh.loadObj("Meshi/kub.obj");

	m_meshBuffer = m_ComputeSys->CreateBuffer( STRUCTURED_BUFFER, sizeof(Triangle), m_mesh.getFaces(), true, false, m_mesh.getTriangles2(), false, "Structured Buffer: Mesh Texture");
	
	D3DX11CreateShaderResourceViewFromFile(m_Device, m_mesh.getMaterial()->map_Kd.c_str(), NULL, NULL, &m_meshTexture, &hr);

	m_materialBuffer = m_ComputeSys->CreateBuffer( STRUCTURED_BUFFER, sizeof(Material2), 1, true, false, &m_mesh.getMaterial2(), false, 0);

	m_FirstPassStruct.firstPass = 1;
}

void Direct3D::update(float dt)
{
	static float rotDT = 0.f;
	rotDT = dt;
	XMMATRIX rot = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotDT);
	for(int i = 0; i < NROFLIGHTS; i++)
	{
		XMVECTOR vLightPos = XMLoadFloat4(&m_lightList[i].pos);
		vLightPos = XMVector4Transform(vLightPos, rot);
		XMStoreFloat4(&m_lightList[i].pos, vLightPos);
	}
	m_pCamera->update();
	//m_fps = 1/dt;
	m_time += dt;
	static int frameCnt = 0;
    static float t_base = 0.f;
	frameCnt++;
	
	if(m_time - t_base >= 1.f)
	{
		frameCnt /= 1;
		m_fps = frameCnt;
		frameCnt = 0;
		t_base += 1.f;
	}

	updateConstantBuffers();
}

void Direct3D::draw()
{
	ID3D11UnorderedAccessView* uav[] = { m_BackBufferUAV, m_FinalColorBuffer->GetUnorderedAccessView() };
	ID3D11UnorderedAccessView* RayUAV[] = {m_RayBuffer->GetUnorderedAccessView()};
	ID3D11UnorderedAccessView* IntersectionUAV[] = {m_RayBuffer->GetUnorderedAccessView(), m_HitDataBuffer->GetUnorderedAccessView()};
	ID3D11ShaderResourceView* ColorSRV[] = {m_HitDataBuffer->GetResourceView(), m_meshBuffer->GetResourceView(), m_materialBuffer->GetResourceView()};
	ID3D11UnorderedAccessView* clearuav[]	= { 0,0,0,0,0,0,0 };
	ID3D11ShaderResourceView* clearsrv[]	= { 0,0,0,0,0,0,0 };
	
	//Primary
	m_DeviceContext->CSSetConstantBuffers(0, 1, &m_PrimaryCBuffer);
	m_DeviceContext->CSSetUnorderedAccessViews(0, 1, RayUAV, 0);
	m_PrimaryShader->Set();
	m_Timer->Start();
	m_DeviceContext->Dispatch(25, 25, 1);
	m_Timer->Stop();
	m_PrimaryShader->Unset();	
	m_DeviceContext->CSSetUnorderedAccessViews(0,1, clearuav, 0);

	int NrBounces = 4;
	for(int i = 0; i < NrBounces; i++)
	{
		//Intersection
		m_DeviceContext->UpdateSubresource(m_FirstPassCBuffer, 0, 0, &m_FirstPassStruct, 0, 0);
		ID3D11Buffer *ICB[] = {m_IntersectionCBuffer, m_FirstPassCBuffer};
		m_DeviceContext->CSSetConstantBuffers(0, 2, ICB);
		m_DeviceContext->CSSetUnorderedAccessViews(0, 2, IntersectionUAV, 0);
		ID3D11ShaderResourceView* srv[] = { m_meshTexture,
											m_meshBuffer->GetResourceView(),
											};

		m_DeviceContext->CSSetShaderResources(0, 2, srv);

		m_IntersectionShader->Set();

		m_DeviceContext->Dispatch(25, 25, 1);

		m_IntersectionShader->Unset();
		m_DeviceContext->CSSetUnorderedAccessViews(0,2, clearuav, 0);
		m_DeviceContext->CSSetShaderResources(0,2, clearsrv);
		
		//Color
		ID3D11Buffer *CCB[] = {m_ColorCBuffer, m_FirstPassCBuffer};
		m_DeviceContext->CSSetConstantBuffers(0, 2, CCB);
		m_DeviceContext->CSSetUnorderedAccessViews(0, 2, uav, 0);
		m_DeviceContext->CSSetShaderResources(0, 3, ColorSRV);
		m_ColorShader->Set();

		m_DeviceContext->Dispatch(25, 25, 1);

		m_ColorShader->Unset();
		m_DeviceContext->CSSetUnorderedAccessViews(0,2, clearuav, 0);
		m_DeviceContext->CSSetShaderResources(0,3, clearsrv);

		if(m_FirstPassStruct.firstPass == 1)
		{
			m_FirstPassStruct.firstPass = 0;
			m_DeviceContext->UpdateSubresource(m_FirstPassCBuffer, 0, 0, &m_FirstPassStruct, 0, 0);
			
		}
	}

	if(FAILED(m_SwapChain->Present( 0, 0 )))
		return;

	XMFLOAT4 camPos = m_pCamera->getPosition();
	
	
	char title[256];
	sprintf_s(
		title,
		sizeof(title),
		"DirectCompute DEMO - Dispatch time: %f. CameraPos X: %f, Y: %f, Z: %f, fps: %f",
		m_Timer->GetTime(), camPos.x, camPos.y, camPos.z, m_fps
	);
	SetWindowText(m_hWnd, title);
}

void Direct3D::createConstantBuffers()
{	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	bd.CPUAccessFlags = NULL;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.MiscFlags = 0;

	//Primary
	if(sizeof(PrimaryConstBuffer) % 16 > 0)
		bd.ByteWidth = ( int )(( sizeof( PrimaryConstBuffer ) / 16 )  + 1) * 16;
	else
		bd.ByteWidth = sizeof(PrimaryConstBuffer);

	m_Device->CreateBuffer( &bd, NULL, &m_PrimaryCBuffer);

	//Intersection
	if(sizeof(IntersectionConstBuffer) % 16 > 0)
		bd.ByteWidth = ( int )(( sizeof( IntersectionConstBuffer ) / 16 )  + 1) * 16;
	else
		bd.ByteWidth = sizeof(IntersectionConstBuffer);
	
	m_Device->CreateBuffer( &bd, NULL, &m_IntersectionCBuffer);

	//Color
	if(sizeof(ColorConstBuffer) % 16 > 0)
		bd.ByteWidth = ( int )(( sizeof( ColorConstBuffer ) / 16 )  + 1) * 16;
	else
		bd.ByteWidth = sizeof(ColorConstBuffer);

	m_Device->CreateBuffer( &bd, NULL, &m_ColorCBuffer);

	//First pass
	if(sizeof(FirstPassConstBuffer) % 16 > 0)
		bd.ByteWidth = ( int )(( sizeof( FirstPassConstBuffer ) / 16 )  + 1) * 16;
	else
		bd.ByteWidth = sizeof(FirstPassConstBuffer);
	m_Device->CreateBuffer( &bd, NULL, &m_FirstPassCBuffer);
}

void Direct3D::updateConstantBuffers()
{
	//First pass constant buffer update	
	m_FirstPassStruct.firstPass = 1;
	m_DeviceContext->UpdateSubresource(m_FirstPassCBuffer, 0, 0, &m_FirstPassStruct, 0, 0);

	m_DeviceContext->CSSetConstantBuffers(0, 1, &m_FirstPassCBuffer);
	//Primary Constant buffer
	PrimaryConstBuffer PCBufferStruct;
	PCBufferStruct.cameraPos = m_pCamera->getPosition();
	
	//Inverse view matrix
	XMMATRIX mInvView = XMLoadFloat4x4(&m_pCamera->getViewMat());
	XMVECTOR mViewDet = XMMatrixDeterminant(mInvView);
	mInvView = XMMatrixInverse(&mViewDet, mInvView);
	mInvView = XMMatrixTranspose(mInvView);
	XMStoreFloat4x4(&PCBufferStruct.IV, mInvView);
	
	//Inverse projection matrix
	XMMATRIX mInvProj = XMLoadFloat4x4(&m_pCamera->getProjMat());
	XMVECTOR mProjDet = XMMatrixDeterminant(mInvProj);
	mInvProj = XMMatrixInverse(&mProjDet, mInvProj);
	mInvProj = XMMatrixTranspose(mInvProj);
	XMStoreFloat4x4(&PCBufferStruct.IP, mInvProj);

	m_DeviceContext->UpdateSubresource(m_PrimaryCBuffer, 0, 0, &PCBufferStruct, 0, 0);
	//Intersection constant buffer
	IntersectionConstBuffer ICBufferStruct;
	ICBufferStruct.sphere = m_sphere;
	
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		ICBufferStruct.triangles[i] = m_triangles[i];
	}
	ICBufferStruct.nrOfFaces = m_mesh.getFaces();
	ICBufferStruct.pad = XMFLOAT3(0.f, 0.f, 0.f);

	m_DeviceContext->UpdateSubresource(m_IntersectionCBuffer, 0, 0, &ICBufferStruct, 0, 0);
	//Color constant buffer
	ColorConstBuffer CCBufferStruct;
	CCBufferStruct.sphere = m_sphere;

	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		CCBufferStruct.triangles[i] = m_triangles[i];
	}
	for(int i = 0; i < NROFLIGHTS; i++)
	{
		CCBufferStruct.lightList[i] = m_lightList[i];
	}

	CCBufferStruct.nrOfFaces = m_mesh.getFaces();
	CCBufferStruct.pad = XMFLOAT3(0.f, 0.f, 0.f);

	m_DeviceContext->UpdateSubresource(m_ColorCBuffer, 0, 0, &CCBufferStruct, 0, 0);
}


void Direct3D::release()
{
	SAFE_RELEASE(m_BackBufferUAV);
	SAFE_RELEASE(m_cBuffer);
	SAFE_RELEASE(m_PrimaryCBuffer);
	SAFE_RELEASE(m_IntersectionCBuffer);
	SAFE_RELEASE(m_ColorCBuffer);
	SAFE_RELEASE(m_FirstPassCBuffer);
	SAFE_RELEASE(m_SwapChain);
	SAFE_DELETE(m_ComputeSys);
	SAFE_DELETE(m_ComputeShader);
	SAFE_DELETE(m_PrimaryShader);
	SAFE_DELETE(m_IntersectionShader);
	SAFE_DELETE(m_ColorShader);
	SAFE_RELEASE(m_meshTexture);
	SAFE_RELEASE(m_materialBuffer);
	SAFE_RELEASE(m_RayBuffer);
	SAFE_RELEASE(m_HitDataBuffer);
	SAFE_RELEASE(m_FinalColorBuffer);
	SAFE_DELETE(m_meshBuffer);
	SAFE_RELEASE(m_Device);
	SAFE_RELEASE(m_DeviceContext);
	SAFE_DELETE(m_Timer);
	m_pCamera.reset();
	m_pInput = nullptr;
}

