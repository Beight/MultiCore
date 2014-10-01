//--------------------------------------------------------------------------------------
// Intersection.fx
// Direct3D 11 Shader Model 5.0
//--------------------------------------------------------------------------------------
#ifndef INTERSECTIONSTAGE
#define INTERSECTIONSTAGE
#include "Structs.fx"
#include "RayIntersect.fx"

RWStructuredBuffer<Ray> Rays : register(u0);
RWStructuredBuffer<HitData> Output : register(u1);
Texture2D MeshTexture : register(t0);
StructuredBuffer<Triangle> MeshTriangles : register(t1);

cbuffer ConstBuffer : register(b0)
{
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	int nrOfFaces;
	float3 pad;
};

cbuffer FirstPass : register (b1)
{
	int firstpass;
};



[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	int index = threadID.x + (threadID.y * WIDTH);
	Ray r = Rays[index];

	HitData hd;
	int tempID = -1;

	if(firstpass == 1)
	{
		hd.pos = float4(0.f, 0.f, 0.f, 0.f);
		hd.normal = float4(0.f, 0.f, 0.f, 0.f);
		hd.distance = -1.0;
		hd.color = float4(0.f, 1.f, 0.f, 0.f);
		hd.ID = -1;
		hd.rayPower = 1.f;
		hd.materialID = -1;
	}
	else
	{
		hd = Output[index];
		hd.distance = -1;
	}

	// ## SPHERE ## //
	if(hd.ID != sphere.ID)
	{
		float hit = RaySphereIntersect(r, sphere, hd.distance);
		if(hit > -1.0f)
		{
			hd.pos = r.origin + r.direction * hit;
			hd.normal = normalize(hd.pos - sphere.center);
			hd.color = sphere.color;
			hd.distance = hit;
			tempID = sphere.ID;
			hd.materialID = -1;
		}
	}

	// ## CUBE ## // 
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
		if(hd.ID != triangles[i].ID)
		{
			float3 hit = RayTriangleIntersect(r, triangles[i], hd.distance);
			if(hit.x > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit.x;
				hd.normal = triangles[i].normal;
				hd.color = triangles[i].color;					
				tempID = triangles[i].ID;
				hd.distance = hit.x;
				hd.materialID = -1;
			}
		}
	}

	// ## MESH ## //
	for(int j = 0; j < nrOfFaces; j++)
	{
		if(hd.ID != MeshTriangles[j].ID)
		{
			float3 hit = RayTriangleIntersect(r, MeshTriangles[j], hd.distance);
			if(hit.x > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit.x;
				hd.normal = MeshTriangles[j].normal;
				hd.color = MeshTexture[hit.yz*512.f];
				tempID = MeshTriangles[j].ID;
				hd.distance = hit.x;
				hd.materialID = 1;
			}
		}
	}

	if(firstpass == 1)
		hd.ID = tempID;

	if(tempID != -1)
		hd.ID = tempID;

	Output[index] = hd;

	if(hd.ID != -1)
	{
		float4 bounceDir = reflect(r.direction, hd.normal);
		r.origin = hd.pos;
		r.direction = bounceDir;
		Rays[index] = r;
		if(firstpass != 1)
		Output[index].rayPower *= REFLECTIVITY;
	}
}

#endif