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

float3 LightStage(HitData hd, Ray r);

cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
};

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	///////////////////////////////////////////////
	//Primary rays stage
	///////////////////////////////////////////////

	HitData hd;
	float hit = -1.0f;
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

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float2 paddy = sphere.pad;
	hit = RaySphereIntersect(r, sphere, hd.distance);
	if(hit > -1.0f)
	{
		hd.pos = r.origin + r.direction * hit;
		hd.normal = normalize(hd.pos - sphere.center);
		hd.color = sphere.color;
		hd.distance = hit;
	}
	
	for(int i = 0; i < 10; i++)
	{
			float3 paddy = triangles[i].pad;
			hit = RayTriangleIntersect(r, triangles[i], hd.distance);
			if(hit > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit;
				float4 e1 = triangles[i].pos1 - triangles[i].pos0;
				float4 e2 = triangles[i].pos2 - triangles[i].pos0;
				float3 temp = cross(e1.xyz, e2.xyz);
				hd.normal = normalize(float4(temp, 1.0f));
				hd.color = triangles[i].color;
				hd.ID = triangles[i].ID;
				hd.distance = hit;
			}
	}
	if(hd.distance >  0.0f)
	{

		///////////////////////////////////////////////
		//First Light Ray
		///////////////////////////////////////////////
		finalColor += LightStage(hd, r);
		///////////////////////////////////////////////
		//Bounce Rays
		///////////////////////////////////////////////
		Ray bounceRay;
		bounceRay = r;
		HitData bounceHit;
		bounceHit.distance = -1.0f;
		bounceHit.pos = hd.pos;
		bounceHit.color = hd.color;
		bounceHit.normal = hd.normal;
		bounceHit.ID = -1;

		for(int j = 0; j < 1; j++)
		{
			float bHit = -1.0f;
			bounceHit.distance = -1.0f;
			bounceHit.ID = -1;
			float4 bounceDir = reflect(bounceRay.direction, bounceHit.normal);
			bounceDir = normalize(bounceDir);
			bounceRay.origin = bounceHit.pos;
			bounceRay.direction = bounceDir;
			if(bounceHit.ID != -2)
			{
				bHit = RaySphereIntersect(bounceRay, sphere, bounceHit.distance);
				if(bHit > -1.0f)
				{
					bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
					bounceHit.normal = normalize(bounceHit.pos - sphere.center);
					bounceHit.color = sphere.color;
					bounceHit.distance = bHit;
					bounceHit.ID = -2;
				}
			}
			for(int i = 0; i < 10; i++)
			{
				float3 paddy = triangles[i].pad;
					if(bounceHit.ID != i)
					{
						bHit = RayTriangleIntersect(bounceRay, triangles[i], bounceHit.distance);
						if(bHit > -1.0f)
						{
							bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
							float4 e1 = triangles[i].pos1 - triangles[i].pos0;
							float4 e2 = triangles[i].pos2 - triangles[i].pos0;
							float3 temp = cross(e1.xyz, e2.xyz);
							bounceHit.normal = normalize(float4(temp, 1.0f));
							bounceHit.color = triangles[i].color;
							bounceHit.ID = triangles[i].ID;
							bounceHit.distance = bHit;
						}
					}
			}
			//if(bounceHit.ID == -1)
				//finalColor = float4(1,0,0,1);
			//else
				finalColor += LightStage(bounceHit, bounceRay) * 0.2f;
		}
	}

	///////////////////////////////////////////////
	//Coloring stage
	///////////////////////////////////////////////
	output[threadID.xy] = float4(finalColor, 1.0f);
}

float3 LightStage(HitData hd, Ray r)
{
		
		float3 final = float3(0.f,0.f,0.f);
		for(int i = 1; i < 2; i++)
		{
			float3 color = float3(0.0f, 0.0f, 0.0f);
			Ray lightRay;
			float hit = -1.0f;
			HitData lightHit;
			lightHit.distance = -1.0f;
			float4 lightDir = lightList[i].pos - hd.pos;
			lightDir = normalize(lightDir);
			lightRay.origin = hd.pos;
			lightRay.direction = lightDir;
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);

			
			hit = RaySphereIntersect(lightRay, sphere, hd.distance);
			if(hit > -1.0f)
			{
				lightHit.pos = lightRay.origin + lightRay.direction * hit;
				lightHit.normal = normalize(lightHit.pos - sphere.center);
				lightHit.color = sphere.color;
				lightHit.distance = hit;
			}	
			for(int j = 0; j < NROFTRIANGLES; j++)
			{
				if(hd.ID-j)
				{
					hit = RayTriangleIntersect(lightRay, triangles[j], hd.distance);
					if(hit > -1.0f)
					{
						lightHit.pos = r.origin + r.direction * hit;
						float4 e1 = triangles[j].pos1 - triangles[j].pos0;
						float4 e2 = triangles[j].pos2 - triangles[j].pos0;
						float3 temp = cross(e1.xyz, e2.xyz);
						lightHit.normal = normalize(float4(temp, 1.0f));
						lightHit.color = triangles[j].color;
						lightHit.ID = triangles[j].ID;
						hd.distance = hit;
					}
				}
			}

			if(lightHit.distance > 0.0f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd, lightList[i], r)*0.5f);
				
			}	
			else 
				color = PointLightR(hd, lightList[i], r);
			final += color;
		}
		return final;
}