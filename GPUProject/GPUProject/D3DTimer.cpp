#include "D3DTimer.h"
#include "stdafx.h"

D3DTimer::D3DTimer(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dDeviceContext)
	: mDevice(d3dDevice), mDeviceContext(d3dDeviceContext)
{
	//ugly, should not be done in constructor :-)
    D3D11_QUERY_DESC desc;
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
    desc.MiscFlags = 0;
	mDevice->CreateQuery(&desc, &mDisjoint);

	desc.Query = D3D11_QUERY_TIMESTAMP;
	mDevice->CreateQuery(&desc, &mStart);
	mDevice->CreateQuery(&desc, &mStop);
}

D3DTimer::~D3DTimer()
{
	if(mStart)		mStart->Release();
	if(mStop)		mStop->Release();
	if(mDisjoint)	mDisjoint->Release();
}

void D3DTimer::Start()
{
	mDeviceContext->Begin(mDisjoint);
 
	mDeviceContext->End(mStart);
}

void D3DTimer::Stop()
{
    mDeviceContext->End(mStop);

    mDeviceContext->End(mDisjoint);
}

double D3DTimer::GetTime()
{
	UINT64 startTime = 0;
	while(mDeviceContext->GetData(mStart, &startTime, sizeof(startTime), 0) != S_OK);

	UINT64 endTime = 0;
	while(mDeviceContext->GetData(mStop, &endTime, sizeof(endTime), 0) != S_OK);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	while(mDeviceContext->GetData(mDisjoint, &disjointData, sizeof(disjointData), 0) != S_OK);

	double time = -1.0f;
	if(disjointData.Disjoint == FALSE)
	{
		UINT64 delta = endTime - startTime;
		double frequency = static_cast<double>(disjointData.Frequency);
		time = (delta / frequency) * 1000.0f;
	}

	return time;
}