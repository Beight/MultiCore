//--------------------------------------------------------------------------------------
// PrimaryRayStage.fx
// Direct3D 11 Shader Model 5.0
//--------------------------------------------------------------------------------------
#ifndef PRIMARYRAYSTAGE
#define PRIMARYRAYSTAGE
#include "Structs.fx"

RWStructuredBuffer<Ray> output : register(u0);

cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
};


[numthreads(NUMTHREADSX, NUMTHREADSY, NUMTHREADSZ)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	Ray r;
	float norm_X = ((threadID.x / WIDTH)*2) - 1.0f;
	float norm_Y = ((1.0f-(threadID.y / HEIGHT))*2) - 1.0f;

	float4 rayDir = float4(norm_X, norm_Y, 1, 1);

	rayDir = mul(rayDir, IP);

	rayDir = rayDir / rayDir.w;

	rayDir = mul(rayDir, IV);

	rayDir = rayDir - cameraPos;
	rayDir = normalize(rayDir);
	r.origin = cameraPos;
	r.direction = rayDir;

	output[threadID.x + (threadID.y*WIDTH)] = r;
}

#endif