//--------------------------------------------------------------------------------------
// ColorStage.fx
// Direct3D 11 Shader Model 5.0
//--------------------------------------------------------------------------------------
#ifndef COLORSTAGE
#define COLORSTAGE
#include "Light.fx"

StructuredBuffer<HitData> HDin : register(t0);
StructuredBuffer<Triangle> MeshTriangles : register(t1);
StructuredBuffer<Material> material : register (t2);

cbuffer ConstBuffer : register (b0)
{
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
	float3 pad;
};

cbuffer FirstPass : register (b1)
{
	bool firstPass;
	float3 firstpad;
};

RWTexture2D<float4> Output : register(u0);
RWStructuredBuffer<float4> FinalColorBuffer : register(u1);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	int index = threadID.x + (threadID.y * WIDTH);
	HitData hd = HDin[index];
	

	if(firstPass)
		FinalColorBuffer[index] = float4(0.f, 0.f, 0.f, 0.f);

	if(hd.ID == -1)
		Output[threadID.xy] = float4(0.f, 0.f, 0.f, 1.f);
	else
	{

		float3 final = float3(0.f, 0.f, 0.f);

		/// ## NUMBER OF LIGHTS ## //
		for(int i = 0; i < 3; i++)
		{	
			float3 color = float3(0.0f, 0.0f, 0.0f);
			Ray lightRay;
			HitData lightHit;
			lightHit.distance = -1.0f;
			lightRay.origin = hd.pos;
			lightRay.direction = normalize(lightList[i].pos - hd.pos);
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);
			// ## SPHERE ## //
			if(hd.ID != sphere.ID)
			{
				float hit = RaySphereIntersect(lightRay, sphere, hd.distance);
				if(hit > -1.0f)
				{
					lightHit.distance = hit;
				}
			}
			// ## CUBE ## //
			for(int j = 0; j < NROFTRIANGLES; j++)
			{
				if(hd.ID != triangles[j].ID)
				{
					float3 hit = RayTriangleIntersect(lightRay, triangles[j], hd.distance);
					if(hit.x > -1.0f)
					{
						lightHit.distance = hit.x;
					}
				}
			}
			// ## MESH ## //
			for(int k = 0; k < nrOfFaces; k++)
			{
				if(hd.ID != MeshTriangles[k].ID)
				{
					float3 hit = RayTriangleIntersect(lightRay, MeshTriangles[k], hd.distance);
					if(hit.x > -1.0f)
					{
						lightHit.distance = hit.x;
					}
				}
			}
			if(lightHit.distance > 0.0001f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], material[0]) * 0.5f);
				
			}	
			else 
				color = PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], material[0]);
		

			final += color;
		}

		FinalColorBuffer[index] += float4(final, 1.f);

		Output[threadID.xy] = saturate(FinalColorBuffer[index]);
	}
}

#endif