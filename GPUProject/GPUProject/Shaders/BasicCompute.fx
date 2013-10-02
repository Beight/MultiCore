//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
//--------------------------------------------------------------------------------------
#include "Light.fx"
static const float WIDTH = 800.0f;
static const float HEIGHT= 800.0f;

RWTexture2D<float4> output : register(u0);


cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
	Triangle tri;
	Light light;
};

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	HitData hd;
	Ray r;
	float norm_X, norm_Y;
	norm_X = ((threadID.x / WIDTH)*2) - 1.0f;
	norm_Y = ((1.0f-(threadID.y / HEIGHT))*2) - 1.0f;

	float4 rayDir = float4(norm_X, norm_Y, 1, 1);

	rayDir = mul(rayDir, IP);

	rayDir = rayDir / rayDir.w;

	rayDir = mul(rayDir, IV);

	rayDir = rayDir - cameraPos;
	rayDir = normalize(rayDir);
	r.origin = cameraPos;
	r.direction = rayDir;

	hd = RaySphereIntersect(r, sphere);
	if(hd.distance == -1.0f)
		hd = RayTriangleIntersect(r, tri);

	float3 color = PointLight(hd, light, r);
	//hd.color = float4(color, 1.0f);
	output[threadID.xy] = hd.color;
}