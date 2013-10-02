//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
//--------------------------------------------------------------------------------------
static const float WIDTH = 800.0f;
static const float HEIGHT= 800.0f;

RWTexture2D<float4> output : register(u0);

struct Ray
{
	float4 m_origin;
	float4 m_direction;
};

cbuffer ConstBuffer
{
	float4 m_cameraPos;
	float4x4 m_IP;
	float4x4 m_IV;
};

//[numthreads(32, 32, 1)]
//void main( uint3 threadID : SV_DispatchThreadID )
//{
//	output[threadID.xy] = float4(float3(1,0,1) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
//}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	float norm_X, norm_Y;
	norm_X = ((threadID.x / WIDTH)*2) - 1.0f;
	norm_Y = ((1.0f-(threadID.y / HEIGHT))*2) - 1.0f;

	float4 rayDir = float4(norm_X, norm_Y, 1, 1);

	rayDir = mul(rayDir, m_IP);

	rayDir = rayDir / rayDir.w;

	rayDir = mul(rayDir, m_IV);

	rayDir = rayDir - m_cameraPos;
	rayDir = normalize(rayDir);
	rayDir.z = 0.0f;

	
	output[threadID.xy] = rayDir;
}