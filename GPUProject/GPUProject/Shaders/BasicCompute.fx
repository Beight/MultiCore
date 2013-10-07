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
	Triangle tri2;
	Light lightList[10];
};

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	///////////////////////////////////////////////
	//Primary rays stage
	///////////////////////////////////////////////

	HitData hd, hds;
	hd.distance = -1.0;
	hd.color	= (0.0f, 0.0f, 0.0f, 0.0f);
	hd.pos		= (0.0f, 0.0f, 0.0f, 0.0f);
	hd.normal	= (0.0f, 0.0f, 0.0f, 0.0f);
	hds.distance = -1.0;
	hds.color	= (0.0f, 0.0f, 0.0f, 0.0f);
	hds.pos		= (0.0f, 0.0f, 0.0f, 0.0f);
	hds.normal	= (0.0f, 0.0f, 0.0f, 0.0f);


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

	///////////////////////////////////////////////
	//Intersection stage
	///////////////////////////////////////////////

	float3 color = (0.0f, 0.0f, 0.0f);


	hd = RaySphereIntersect(r, sphere, hd);

	hd = RayTriangleIntersect(r, tri, hd);

	hd = RayTriangleIntersect(r, tri2, hd);

	if(hd.distance >  0.0f)
	{
		
		///////////////////////////////////////////////
		//Secondary Rays
		///////////////////////////////////////////////
		Ray lightRay;
		float4 lightDir = float4(lightList[0].pos, 1.0f) - hd.pos;
		lightDir = normalize(lightDir);
		lightRay.origin = hd.pos;
		lightRay.direction = lightDir;

		hds = RaySphereIntersect(lightRay, sphere, hds);

		hds = RayTriangleIntersect(lightRay, tri, hds);

		hds = RayTriangleIntersect(lightRay, tri2, hds);
		if(hds.distance > 0.0f)
		{
			
		}	
		else 
			color += PointLight(hd, lightList[0], r);
	}




	///////////////////////////////////////////////
	//Coloring stage
	///////////////////////////////////////////////
	
	//float3 color = PointLight(hd, lightList[0], r);
	//color += PointLight(hd, lightList[0], r);
	//for(int i = 1; i < 10; i++)
	//{
	//	color += PointLight(hd, lightList[i], r);
	//}
	hd.color = float4(color, 1.0f);
	output[threadID.xy] = hd.color;
}