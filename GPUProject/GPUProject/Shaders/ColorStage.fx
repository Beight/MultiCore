//--------------------------------------------------------------------------------------
// ColorStage.fx
// Direct3D 11 Shader Model 5.0
//--------------------------------------------------------------------------------------
#ifndef COLORSTAGE
#define COLORSTAGE
#include "Light.fx"

StructuredBuffer<HitData> HDin : register(t0);
StructuredBuffer<MeshTriangle> MeshTriangles : register(t1);
StructuredBuffer<Material> material : register (t2);

cbuffer ConstBuffer
{
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
	bool firstPass;
	float2 pad;
};

RWTexture2D<float4> Output : register(u0);
RWStructuredBuffer<float4> accOutput : register(u1); //Don't know what this is for.......

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{

	int index = threadID.x + (threadID.y * WIDTH);
	HitData hd = HDin[index];

	if(firstPass)
		accOutput[index] = float4(0.f, 0.f, 0.f, 0.f);

	if(hd.ID = -1)
		Output[threadID.xy] = float4(1.f, 0.f, 0.f, 1.f);
	else
	{

		float3 final = float3(0.f, 0.f, 0.f);
		//float3 paddy = lightList[0].pad;

		/// ## NUMBER OF LIGHTS ## //
		for(int i = 0; i < 2; i++)
		{	
			float3 color = float3(0.0f, 0.0f, 0.0f);
			Ray lightRay;
			float hit = -1.0f;
			HitData lightHit;
			lightHit.distance = -1.0f;
			lightRay.origin = hd.pos;
			lightRay.direction = normalize(lightList[i].pos - hd.pos);
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);
			// ## SPHERE ## //
			if(hd.ID != -2)
			{
				hit = RaySphereIntersect(lightRay, sphere, hd.distance);
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
					hit = RayTriangleIntersect(lightRay, triangles[j], hd.distance);
					if(hit > -1.0f)
					{
						lightHit.distance = hit;
					}
				}
			}
			// ## MESH ## //
			for(j = 0; j < nrOfFaces; j++)
			{
				if(hd.ID != MeshTriangles[j].ID)
				{
					float3 temp = RayTriangleIntersects(lightRay, MeshTriangles[j], hd.distance);
					if(temp.x > -1.0f)
					{
						lightHit.distance = temp.x;
					}
				}
			}
			//float3 diff = MeshTexture.Sample(DiffuseMap, ;
			if(lightHit.distance > 0.0001f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], material[0]) * 0.5f);//, diff)*0.5f);
				
			}	
			else 
				color = PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], material[0]);//, diff);
		

			final += color;
		}

		accOutput[index] += float4(final, 1.f);

		Output[threadID.xy] = float4(0.f,1.f,0.f,0.f); //saturate(accOutput[index]);
	}
}

#endif