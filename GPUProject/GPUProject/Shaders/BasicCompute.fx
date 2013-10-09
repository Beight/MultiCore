//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
//--------------------------------------------------------------------------------------
#include "Light.fx"
static const float WIDTH = 800.0f;
static const float HEIGHT= 800.0f;
#define NROFTRIANGLES 10
#define NROFLIGHTS 10

RWTexture2D<float4> output : register(u0);


cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
};

[numthreads(16, 16, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	///////////////////////////////////////////////
	//Primary rays stage
	///////////////////////////////////////////////

	HitData hd;
	hd.distance = -1.0;
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

	float3 color = float3(0.0f, 0.0f, 0.0f);


	hd = RaySphereIntersect(r, sphere, hd);
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
			hd = RayTriangleIntersect(r, triangles[i], hd);
	}
	if(hd.distance >  0.0f)
	{

		///////////////////////////////////////////////
		//Secondary Rays
		///////////////////////////////////////////////
		for(int i = 0; i < NROFLIGHTS; i++)
		{
			Ray lightRay;
			HitData lightHit;
			lightHit.distance = -1.0f;
			float4 lightDir = lightList[i].pos - hd.pos;
			lightDir = normalize(lightDir);
			lightRay.origin = hd.pos;
			lightRay.direction = lightDir;
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);

			lightHit = RaySphereIntersect(lightRay, sphere, lightHit);
			
			for(int j = 0; j < NROFTRIANGLES; j++)
			{
				if(hd.ID-j)
				{
					lightHit = RayTriangleIntersect(lightRay, triangles[j], lightHit);
					//lightHit.distance = j;
				}
				
			}

			if(lightHit.distance > 0.0f && lightLength > lightHit.distance)
			{
				color += (PointLight(hd, lightList[i], r)*0.1f);
				
			}	
			else 
				color += PointLight(hd, lightList[i], r);
		}
		//Calculate bounce
	}




	///////////////////////////////////////////////
	//Coloring stage
	///////////////////////////////////////////////
	//hd.color = float4(color, 1.0f);

	output[threadID.xy] = float4(color, 1.0f);
}