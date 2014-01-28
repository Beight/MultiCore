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
StructuredBuffer<MeshTriangle> input : register(t0);
Texture2D meshTexture : register(t1);
StructuredBuffer<Material> material : register (t2);
SamplerState diffuseMap : register (s0);


float3 LightStage(HitData hd, Ray r);

cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
	Sphere spherel0;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
};

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	///////////////////////////////////////////////
	//Primary rays stage
	///////////////////////////////////////////////
	HitData hd;
	float hit;
	hd.distance = -1.0;
	hd.ID = -1;
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
	// ## SPHERE ## //
	hit = RaySphereIntersect(r, sphere, hd.distance);
	if(hit > -1.0f)
	{
		hd.pos = r.origin + r.direction * hit;
		hd.normal = normalize(hd.pos - sphere.center);
		hd.color = sphere.color;
		hd.distance = hit;
		hd.ID = sphere.ID;
		hd.materialID = -1;
	}
	hit = RaySphereIntersect(r, spherel0, hd.distance);
	if(hit > -1.0f)
	{
		hd.pos = r.origin + r.direction * hit;
		hd.normal = normalize(hd.pos - spherel0.center);
		hd.color = spherel0.color;
		hd.distance = hit;
		hd.ID = spherel0.ID;
		hd.materialID = -1;
	}

	//					
	// ## CUBE ## // 
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
			//padding
			float3 paddy = triangles[i].pad;
			hit = RayTriangleIntersect(r, triangles[i], hd.distance);
			if(hit > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit;
				hd.normal = triangles[i].normal;
				hd.color = triangles[i].color;					
				hd.ID = triangles[i].ID;
				hd.distance = hit;
				hd.materialID = -1;
			}
	}

	// ## MESH ## //
	for(int j = 0; j < nrOfFaces; j++)
	{
		//padding
		float paddy = input[j].pad;
		float3 temp;
		temp = RayTriangleIntersects(r, input[j], hd.distance);
		hit = temp.x;
		if(hit > -1.0f)
		{
			hd.pos = r.origin + r.direction * hit;
			hd.normal = normalize(input[j].normal);
			hd.color = /*float4(temp.yz, 0.f, 1.f);//*/ meshTexture[temp.yz*512.f];
			hd.ID = input[j].ID;
			hd.distance = hit;
			hd.materialID = 1;
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
		bounceHit.ID = hd.ID;
		int tempID = -1;
		///////////////////////////////////////////////
		//Number of Bounces
		///////////////////////////////////////////////
		for(int j = 0; j < 10; j++)
		{
			float bHit = -1.0f;
			bounceHit.distance = -1.0f;
			float4 bounceDir = reflect(bounceRay.direction, bounceHit.normal);
			bounceDir = normalize(bounceDir);
			bounceRay.origin = bounceHit.pos;
			bounceRay.direction = bounceDir;
			// ## SPHERE ## //
			if(bounceHit.ID != -2)
			{
				bHit = RaySphereIntersect(bounceRay, sphere, bounceHit.distance);
				if(bHit > -1.0f)
				{
					bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
					bounceHit.normal = normalize(bounceHit.pos - sphere.center);
					bounceHit.color = sphere.color;
					bounceHit.distance = bHit;
					tempID = sphere.ID;
					bounceHit.materialID = -1;
				}
			}
			// ## CUBE ## //
			for(int i = 0; i < NROFTRIANGLES; i++)
			{
				float3 paddy = triangles[i].pad;
				if(bounceHit.ID != triangles[i].ID)
				{
					bHit = RayTriangleIntersect(bounceRay, triangles[i], bounceHit.distance);
					if(bHit > -1.0f)
					{
						bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
						bounceHit.normal = triangles[i].normal;
						bounceHit.color = triangles[i].color;
						tempID = triangles[i].ID;
						bounceHit.distance = bHit;
						bounceHit.materialID = -1;
					}
				}
			}
			// ## MESH ## //
			for(int j = 0; j < nrOfFaces; j++)
			{
				if(bounceHit.ID != input[j].ID)
				{
					//padding
					float paddy = input[j].pad;
					float3 temp = RayTriangleIntersects(bounceRay, input[j], bounceHit.distance);
					bHit = temp.x;
					if(bHit > -1.0f)
					{
						bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
						bounceHit.normal = normalize(input[j].normal);
						bounceHit.color = meshTexture[temp.yz*512.f];
						tempID = input[j].ID;
						bounceHit.distance = bHit;
						bounceHit.materialID = 1;
					}
				}
			}
			if(tempID != -1)
				bounceHit.ID = tempID;
			///////////////////////////////////////////////
			//Bounce Light
			///////////////////////////////////////////////
			finalColor += LightStage(bounceHit, bounceRay) * 0.2f;
		}
	}
	
	output[threadID.xy] = /* hd.normal;//*/ float4(finalColor, 1.0f);
}


///////////////////////////////////////////////
//			LIGHT STAGE
///////////////////////////////////////////////
float3 LightStage(HitData hd, Ray r)
{
		
		float3 final = float3(0.f,0.f,0.f);
		float3 paddy = lightList[0].pad;

		/// ## NUMBER OF LIGHTS ## //
		for(int i = 1; i < 2; i++)
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
					hit = RayTriangleIntersect(r, triangles[j], hd.distance);
					if(hit > -1.0f)
					{
						lightHit.distance = hit;
					}
				}
			}
			// ## MESH ## //
			for(int k = 0; k < nrOfFaces; k++)
			{
				if(hd.ID != input[k].ID)
				{
					float3 temp = RayTriangleIntersects(lightRay, input[k], hd.distance);
					if(temp.x > -1.0f)
					{
						lightHit.distance = temp.x;
					}
				}
			}
			//float3 diff = tex2D(diffuseMap, )
			if(lightHit.distance > 0.0001f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], r, material[0]) * 0.5f);//, diff)*0.5f);
				
			}	
			else 
				color = PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], r, material[0]);//, diff);
		

			final += color;
		}
		return final;
}